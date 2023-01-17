/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：This document, in principle, captures all exceptions.
 *               Prevent exceptions from spreading to insecure modules.
 */
#include "module_ipc/service.h"

#include <algorithm>
#include <cerrno>
#include <cstddef>
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
    sched_ = sptr(new SchedScheduler(wptr(this), wptr(session_)));
    sched_->StartTimer();
    HILOGI("End, res = %{public}d", res);
}

void Service::OnStop()
{
    HILOGI("Called");
    sched_ = nullptr;
    session_ = nullptr;
}

UniqueFd Service::GetLocalCapabilities()
{
    try {
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
    session_->Deactive(obj, force);
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
            session_->VerifyBundleName(hapTokenInfo.bundleName);
            return hapTokenInfo.bundleName;
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
        auto setbackupExtNameMap = [](const BundleName &bundleName) {
            BackupExtInfo info {};
            return make_pair(bundleName, info);
        };
        transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
                  setbackupExtNameMap);
        session_->Active({
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
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, const vector<BundleName> &bundleNames)
{
    try {
        map<BundleName, BackupExtInfo> backupExtNameMap;
        auto setbackupExtNameMap = [](const BundleName &bundleName) {
            BackupExtInfo info {};
            return make_pair(bundleName, info);
        };
        transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
                  setbackupExtNameMap);
        session_->Active({
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
        HILOGE("begin");
        for (int num = 0; num < BConstants::EXT_CONNECT_MAX_COUNT; num++) {
            sched_->Sched();
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    try {
        HILOGE("begin");
        session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }

        auto backUpConnection = session_->GetExtConnection(fileInfo.owner);

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
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd)
{
    try {
        HILOGE("begin %{public}s", fileName.data());
        string callerName = VerifyCallerAndGetCallerName();
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }
        if (fileName == BConstants::EXT_BACKUP_MANAGE) {
            fd = session_->OnBunleExtManageInfo(callerName, move(fd));
        }

        session_->GetServiceReverseProxy()->BackupOnFileReady(callerName, fileName, move(fd));

        if (session_->OnBunleFileReady(callerName, fileName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            // 通知extension清空缓存
            proxy->HandleClear();
            // 通知TOOL 备份完成
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(BError(BError::Codes::OK), callerName);
            // 断开extension
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::AppDone(ErrCode errCode)
{
    try {
        HILOGE("begin");
        string callerName = VerifyCallerAndGetCallerName();
        if (session_->OnBunleFileReady(callerName)) {
            auto backUpConnection = session_->GetExtConnection(callerName);
            auto proxy = backUpConnection->GetBackupExtProxy();
            if (!proxy) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
            }
            proxy->HandleClear();
            IServiceReverse::Scenario scenario = session_->GetScenario();
            if (scenario == IServiceReverse::Scenario::BACKUP) {
                session_->GetServiceReverseProxy()->BackupOnBundleFinished(errCode, callerName);
            } else if (scenario == IServiceReverse::Scenario::RESTORE) {
                session_->GetServiceReverseProxy()->RestoreOnBundleFinished(errCode, callerName);
            }
            backUpConnection->DisconnectBackupExtAbility();
            ClearSessionAndSchedInfo(callerName);
        }
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode(); // 任意异常产生，终止监听该任务
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        BConstants::ExtensionAction action;
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            action = BConstants::ExtensionAction::BACKUP;
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            action = BConstants::ExtensionAction::RESTORE;
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }

        AAFwk::Want want;
        string backupExtName = session_->GetBackupExtName(bundleName);
        want.SetElementName(bundleName, backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));

        auto backUpConnection = session_->GetExtConnection(bundleName);
        ErrCode ret = backUpConnection->ConnectBackupExtAbility(want);
        return ret;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

ErrCode Service::GetExtFileName(string &bundleName, string &fileName)
{
    try {
        HILOGE("begin");
        session_->VerifyCallerAndScenario(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);
        session_->SetExtFileNameRequest(bundleName, fileName);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("Unexpected exception");
        return EPERM;
    }
}

void Service::OnBackupExtensionDied(const string &&bundleName, ErrCode ret)
{
    try {
        HILOGI("extension died. Died bundleName = %{public}s", bundleName.data());
        string callName = move(bundleName);
        session_->VerifyBundleName(callName);
        auto scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleFinished(ret, callName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleFinished(ret, callName);
        }
        auto backUpConnection = session_->GetExtConnection(callName);
        if (backUpConnection->IsExtAbilityConnected()) {
            backUpConnection->DisconnectBackupExtAbility();
        }
        ClearSessionAndSchedInfo(bundleName);
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

void Service::ExtStart(const string &bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        auto backUpConnection = session_->GetExtConnection(bundleName);
        auto proxy = backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            auto ret = proxy->HandleBackup();
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
            if (ret) {
                ClearSessionAndSchedInfo(bundleName);
            }
            return;
        }
        if (scenario != IServiceReverse::Scenario::RESTORE) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
        }
        session_->GetServiceReverseProxy()->RestoreOnBundleStarted(BError(BError::Codes::OK), bundleName);
        auto fileNameVec = session_->GetExtFileNameRequest(bundleName);
        for (auto &fileName : fileNameVec) {
            UniqueFd fd = proxy->GetFileHandle(fileName);
            if (fd < 0) {
                HILOGE("Failed to extension file handle");
                OnBackupExtensionDied(move(bundleName), fd);
                return;
            }
            session_->GetServiceReverseProxy()->RestoreOnFileReady(bundleName, fileName, move(fd));
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

int Service::Dump(int fd, const vector<u16string> &args)
{
    if (fd < 0) {
        HILOGI("HiDumper handle invalid");
        return -1;
    }

    session_->DumpInfo(fd, args);
    return 0;
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        IServiceReverse::Scenario scenario = session_->GetScenario();
        if (scenario == IServiceReverse::Scenario::BACKUP) {
            session_->GetServiceReverseProxy()->BackupOnBundleStarted(ret, bundleName);
        } else if (scenario == IServiceReverse::Scenario::RESTORE) {
            session_->GetServiceReverseProxy()->RestoreOnBundleStarted(ret, bundleName);
        }
        ClearSessionAndSchedInfo(bundleName);
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

void Service::ExtConnectDone(string bundleName)
{
    try {
        HILOGE("begin %{public}s", bundleName.data());
        session_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::RUNNING);
        sched_->Sched(bundleName);
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

void Service::ClearSessionAndSchedInfo(const string &bundleName)
{
    try {
        session_->RemoveExtInfo(bundleName);
        sched_->RemoveExtConn(bundleName);
        sched_->Sched();
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
} // namespace OHOS::FileManagement::Backup
