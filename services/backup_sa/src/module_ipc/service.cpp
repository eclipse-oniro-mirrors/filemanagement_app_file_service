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

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    }
}

ErrCode Service::Start()
{
    try {
        session_.Start();
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

tuple<ErrCode, TmpFileSN, UniqueFd> Service::GetFileOnServiceEnd(string &bundleName)
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        TmpFileSN tmpFileSN = seed++;
        string tmpPath =
            string(BConstants::SA_BUNDLE_BACKUP_DIR).append(bundleName).append(BConstants::SA_BUNDLE_BACKUP_TMP_DIR);
        if (!ForceCreateDirectory(tmpPath)) { // 强制创建文件夹
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Failed to create folder");
        }

        tmpPath.append(to_string(tmpFileSN));
        if (access(tmpPath.data(), F_OK) == 0) {
            // 约束服务启动时清空临时目录，且生成的临时文件名必不重复
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Tmp file to create is existed");
        }
        // REM : 文件权限777 会在8月10日之前解决
        UniqueFd fd(open(tmpPath.data(), O_RDWR | O_CREAT, 0777));
        if (fd < 0) {
            stringstream ss;
            ss << "Failed to open tmpPath " << errno;
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }

        return {ERR_OK, tmpFileSN, move(fd)};
    } catch (const BError &e) {
        return {e.GetCode(), -1, UniqueFd(-1)};
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return {BError(-EPERM), -1, UniqueFd(-1)};
    } catch (...) {
        HILOGE("Unexpected exception");
        return {BError(-EPERM), -1, UniqueFd(-1)};
    }
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }
        session_.PublishFile(fileInfo.owner, fileInfo.fileName);

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
        string callerName = VerifyCallerAndGetCallerName();
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_.OnBunleExtManageInfo(callerName, move(fd));
        }

        session_.GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd));

        session_.OnBunleFileReady(callerName, fileName);
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
        string callerName = VerifyCallerAndGetCallerName();
        session_.OnBunleFileReady(callerName);

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

ErrCode Service::LaunchBackupExtension(IServiceReverse::Scenario scenario,
                                       const BundleName &bundleName,
                                       const string &backupExtName)
{
    try {
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else if (scenario == IServiceReverse::Scenario::CLEAR) {
            action = BConstants::ExtensionAction::CLEAR;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }

        AAFwk::Want want;
        want.SetElementName(bundleName, backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));

        const int default_request_code = -1;
        int ret = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, default_request_code,
                                                                           AppExecFwk::Constants::START_USERID);
        HILOGI("Started %{public}s[100]'s %{public}s with %{public}s set to %{public}d", bundleName.c_str(),
               backupExtName.c_str(), BConstants::EXTENSION_ACTION_PARA, action);
        return ret;
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
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);
        session_.QueueGetFileRequest(bundleName, fileName);

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
} // namespace OHOS::FileManagement::Backup
