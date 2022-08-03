/*
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：本文件原则上要捕获所有异常，防止异常扩散到异常不安全的模块
 */
#include "module_ipc/service.h"

#include <algorithm>
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <regex>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include "ability_manager_client.h"
#include "ability_util.h"
#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_file_info.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_process/b_multiuser.h"
#include "b_process/b_process.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
#include "module_ipc/svc_backup_connection.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, false);

void Service::OnStart()
{
    bool res = SystemAbility::Publish(sptr(this));
    HILOGI("End, res = %{public}d", res);
}

void Service::OnStop()
{
    HILOGI("Called");
}

UniqueFd Service::GetLocalCapabilities()
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        struct statfs fsInfo = {};
        if (statfs(BConstants::SA_BUNDLE_BACKUP_ROOT_DIR.data(), &fsInfo) == -1) {
            throw BError(errno);
        }

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(
            UniqueFd(open(BConstants::SA_BUNDLE_BACKUP_ROOT_DIR.data(), O_TMPFILE | O_RDWR, 0600)));
        auto cache = cachedEntity.Structuralize();
        cache.SetFreeDiskSpace(fsInfo.f_bfree);
        cache.SetOSFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        cachedEntity.Persist();

        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        return UniqueFd(-e.GetCode());
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    session_.Deactive(obj, force);
    sched_ = nullptr;
}

string Service::VerifyCallerAndGetCallerName()
{
    try {
        uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
        int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
        if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
            Security::AccessToken::HapTokenInfo hapTokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Get hap token info failed");
            }
            session_.VerifyBundleName(hapTokenInfo.bundleName);
            return hapTokenInfo.bundleName;
        } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
            if (IPCSkeleton::GetCallingUid() != BConstants::SYSTEM_UID) { // REM: uid整改后调整
                throw BError(BError::Codes::SA_BROKEN_IPC, "Calling uid is invalid");
            }

            Security::AccessToken::NativeTokenInfo tokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenCaller, tokenInfo) != 0) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Get native token info failed");
            }
            if (tokenInfo.processName != "backup_tool" &&
                tokenInfo.processName != "hdcd") { // REM: hdcd整改后删除 { // REM: backup_sa整改后删除
                throw BError(BError::Codes::SA_INVAL_ARG, "Process name is invalid");
            }
            return "simulate";
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, string("Invalid token type ").append(to_string(tokenType)));
        }
    } catch (const BError &e) {
        return "";
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return "";
    } catch (...) {
        HILOGE("Unexpected exception");
        return "";
    }
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote, const vector<BundleName> &bundleNames)
{
    try {
        map<BundleName, BackupExtInfo> backupExtNameMap;
        auto SetbackupExtNameMap = [](const BundleName &bundleName) {
            BackupExtInfo info {};
            return make_pair(bundleName, info);
        };
        transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
                  SetbackupExtNameMap);
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::RESTORE,
            .backupExtNameMap = move(backupExtNameMap),
            .clientProxy = remote,
        });

        sched_ = make_unique<SchedScheduler>(wptr(this));

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, const vector<BundleName> &bundleNames)
{
    try {
        map<BundleName, BackupExtInfo> backupExtNameMap;
        auto SetbackupExtNameMap = [](const BundleName &bundleName) {
            BackupExtInfo info {};
            return make_pair(bundleName, info);
        };
        transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
                  SetbackupExtNameMap);
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::BACKUP,
            .backupExtNameMap = move(backupExtNameMap),
            .clientProxy = remote,
        });

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        uint64_t size = cache.GetFreeDiskSpace();
        if (size == 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Invalid field FreeDiskSpace or unsufficient space");
        }

        sched_ = make_unique<SchedScheduler>(wptr(this));

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    }
}

ErrCode Service::Start()
{
    try {
        HILOGE("begin");
        vector<pair<string, string>> extNameVec;
        session_.GetBackupExtNameVec(extNameVec);
        for (auto [bundleName, backupExtName] : extNameVec) {
            sched_->QueueSetExtBundleName(bundleName, backupExtName);
        }
        sched_->SchedConn();
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    try {
        HILOGE("begin");
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }

        auto backUpConnection = session_.GetExtConnection(fileInfo.owner);

        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        ErrCode res = proxy->PublishFile(fileInfo.fileName);
        if (res) {
            HILOGE("Failed to publish file for backup extension");
        }

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd)
{
    try {
        HILOGE("begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_.OnBunleExtManageInfo(callerName, move(fd));
        }

        session_.GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd));

        if (session_.OnBunleFileReady(callerName, fileName)) {
            auto backUpConnection = session_.GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 通知TOOL 备份完成
            session_.GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), callerName);
            // 断开extension
            backUpConnection->DisconnectBackupExtAbility();
            session_.RemoveExtInfo(callerName);
            // 移除调度器
            sched_->RemoveExtConn(callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::AppDone(ErrCode errCode)
{
    try {
        HILOGE("begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (session_.OnBunleFileReady(callerName)) {
            auto backUpConnection = session_.GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            IServiceReverse::Scenario scenario = session_.GetScenario();
            if (scenario == IServiceReverse::Scenario::BACKUP) {
                session_.GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), callerName);
            } else if (scenario == IServiceReverse::Scenario::RESTORE) {
                session_.GetServiceReverseProxy()->RestoreOnBundleFinished(BError(BError::Codes::OK), callerName);
            }
            backUpConnection->DisconnectBackupExtAbility();
            session_.RemoveExtInfo(callerName);
            sched_->RemoveExtConn(callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName, const string &backupExtName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_.GetScenario();
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }

        AAFwk::Want want;
        want.SetElementName(bundleName, backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));

        auto backUpConnection = session_.GetExtConnection(bundleName);
        ErrCode ret = backUpConnection->ConnectBackupExtAbility(want);
        return BError(ret);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::GetExtFileName(string &bundleName, string &fileName)
{
    try {
        HILOGE("begin");
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);
        if (!sched_) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Invalid sched scheduler");
        }
        sched_->QueueGetFileRequest(bundleName, fileName);
        sched_->Sched(bundleName);

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

void Service::OnBackupExtensionDied(const string &&bundleName, ErrCode ret)
{
    try {
        HILOGI("extension died. Died bundleName = %{public}s", bundleName.data());
        string callName = move(bundleName);
        session_.VerifyBundleName(callName);
        auto scenario = session_.GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_.GetServiceReverseProxy()->BackupOnBundleFinished(ret, callName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_.GetServiceReverseProxy()->RestoreOnBundleFinished(ret, callName);
        }
        session_.RemoveExtInfo(callName);
        sched_->RemoveExtConn(callName);
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

bool Service::TryExtConnect(const string &bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        auto backUpConnection = session_.GetExtConnection(bundleName);
        bool flag = backUpConnection->IsExtAbilityConnected();
        HILOGE("flag = %{public}d", flag);
        if (!flag) {
            return false;
        }
        return true;
    } catch (const BError &e) {
        return false;
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return false;
    } catch (...) {
        HILOGE("Unexpected exception");
        return false;
    }
}

void Service::ExtStart(const std::string &bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_.GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            auto backUpConnection = session_.GetExtConnection(bundleName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            auto ret = proxy->HandleBackup();
            session_.GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_.GetServiceReverseProxy()->RestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
            sched_->Sched(bundleName);
        }
        return;
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

void Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    try {
        HILOGE("begin");
        IServiceReverse::Scenario scenario = session_.GetScenario();
        if (scenario != IServiceReverse::Scenario::RESTORE) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
        }
        auto backUpConnection = session_.GetExtConnection(bundleName);
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        UniqueFd fd = proxy->GetFileHandle(fileName);
        if (fd < 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to get file handle");
        }
        session_.GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd));
        return;
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return;
    } catch (...) {
        HILOGE("Unexpected exception");
        return;
    }
}

int Service::Dump(int fd, const std::vector<std::u16string> &args)
{
    if (fd < 0) {
        HILOGI("HiDumper handle invalid");
        return -1;
    }

    session_.DumpInfo(fd, args);
    return 0;
}
} // namespace OHOS::FileManagement::Backup
