/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/svc_session_manager.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>

#include "ability_util.h"
#include "b_error/b_error.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_json/b_json_entity_usr_config.h"
#include "b_resources/b_constants.h"
#include "bundle_mgr_client.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SvcSessionManager::VerifyCaller(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    shared_lock lock(lock_);
    if (impl_.scenario != scenario) {
        throw BError(BError::Codes::SDK_MIXED_SCENARIO);
    }
    if (impl_.clientToken != clientToken) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Caller mismatched");
    }
    HILOGE("Succeed to verify the caller");
}

void SvcSessionManager::Active(Impl newImpl)
{
    unique_lock lock(lock_);

    Impl &oldImpl = impl_;
    if (oldImpl.clientToken) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Already have an active session");
    }

    if (!newImpl.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (newImpl.scenario == IServiceReverse::Scenario::UNDEFINED) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No scenario was specified");
    }

    GetBundleExtNames(newImpl.backupExtNameMap);

    if (!newImpl.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }
    auto remoteObj = newImpl.clientProxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
    }

    impl_ = newImpl;
    auto callback = [revPtr {reversePtr_}](const wptr<IRemoteObject> &obj) {
        HILOGI("Client died. Died remote obj = %{private}p", obj.GetRefPtr());

        auto revPtrStrong = revPtr.promote();
        if (!revPtrStrong) {
            // 服务先于客户端死亡是一种异常场景，但该场景对本流程来说也没什么影响，所以只是简单记录一下
            HILOGW("It's curious that the backup sa dies before the backup client");
            return;
        }
        (void)revPtrStrong->StopAll(obj);
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
    sched_ = make_unique<SchedScheduler>(*this);
    HILOGI(
        "Succeed to active a session."
        "Client token = %{public}u, client proxy = 0x%{private}p, remote obj = 0x%{private}p",
        impl_.clientToken, impl_.clientProxy.GetRefPtr(), remoteObj.GetRefPtr());
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    unique_lock lock(lock_);
    if (!force && (!impl_.clientToken || !impl_.clientProxy)) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Try to deactive an empty session");
    }
    auto remoteHeldByProxy = impl_.clientProxy->AsObject();
    if (!force && (remoteInAction != remoteHeldByProxy)) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Only the client actived the session can deactive it");
    }

    if (remoteHeldByProxy) {
        remoteHeldByProxy->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
    sched_ = nullptr;
    HILOGI("Succeed to deactive a session. Client token = %{public}u, client proxy = 0x%{private}p", impl_.clientToken,
           impl_.clientProxy.GetRefPtr());
    impl_ = {};
}

void SvcSessionManager::VerifyBundleName(string &bundleName)
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    auto asVerify = [&bundleName](const auto &it) { return it.first == bundleName; };
    if (none_of(impl_.backupExtNameMap.begin(), impl_.backupExtNameMap.end(), asVerify)) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }
    HILOGE("Succeed to verify the bundleName");
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    unique_lock lock(lock_);
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_REFUSED_ACT, "Try to deactive an empty session");
    }
    return impl_.clientProxy;
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    return impl_.scenario;
}

void SvcSessionManager::GetBundleExtNames(map<BundleName, BackupExtInfo> &backupExtNameMap)
{
    if (backupExtNameMap.empty()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No app was selected");
    }

    auto bms = AAFwk::AbilityUtil::GetBundleManager();
    if (!bms) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Bms is invalid");
    }

    for (auto &&it : backupExtNameMap) {
        AppExecFwk::BundleInfo installedBundle;
        if (!bms->GetBundleInfo(it.first, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, installedBundle,
                                AppExecFwk::Constants::START_USERID)) {
            string pendingMsg = string("Failed to get the info of bundle ").append(it.first);
            throw BError(BError::Codes::SA_BROKEN_IPC, pendingMsg);
        }
        for (auto &&ext : installedBundle.extensionInfos) {
            if (ext.type == AppExecFwk::ExtensionAbilityType::BACKUP) {
                vector<string> out;
                AppExecFwk::BundleMgrClient client;
                if (!client.GetResConfigFile(ext, "ohos.extension.backup", out)) {
                    string pendingMsg = string("Failed to get resconfigfile of bundle ").append(it.first);
                    throw BError(BError::Codes::SA_INVAL_ARG, pendingMsg);
                }
                if (!out.size()) {
                    string pendingMsg = string("ResConfigFile size is empty of bundle ").append(it.first);
                    throw BError(BError::Codes::SA_INVAL_ARG, pendingMsg);
                }
                BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(out[0]);
                auto cache = cachedEntity.Structuralize();
                if (cache.GetAllowToBackupRestore()) {
                    it.second.backupExtName = ext.name;
                } else {
                    string pendingMsg =
                        string("Permission denied to get allowToBackupRestore of bundle ").append(it.first);
                    throw BError(BError::Codes::SA_INVAL_ARG, pendingMsg);
                }
            }
        }

        auto callback = [session {this}](const string &bundleName) {
            HILOGI("extension died. Died bundleName = %{public}s", bundleName.data());
            session->OnBackupExtensionDied(bundleName);
        };
        auto backUpConnection = sptr<SvcBackupConnection>(new SvcBackupConnection(callback));
        it.second.backUpConnection = backUpConnection;
    }
}

void SvcSessionManager::OnBunleFileReady(const string &bundleName, const string &fileName)
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    if (!it->second.backUpConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
    }
    auto proxy = it->second.backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }

    // 判断是否结束 通知EXTENTION清理资源  TOOL应用完成备份
    if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        if (!fileName.empty() && fileName != BConstants::EXT_BACKUP_MANAGE) {
            auto ret = it->second.fileNameInfo.insert(fileName);
            if (!ret.second) {
                it->second.fileNameInfo.erase(fileName);
            }
        } else if (fileName.empty()) {
            it->second.receExtAppDone = true;
        }
        if (it->second.receExtManageJson && it->second.fileNameInfo.empty() && it->second.receExtAppDone) {
            proxy->HandleClear();
            impl_.clientProxy->BackupOnBundleFinished(BError(BError::Codes::OK), bundleName);
            it->second.backUpConnection->DisconnectBackupExtAbility();
            impl_.backupExtNameMap.erase(it);
        }
    } else if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        proxy->HandleClear();
        impl_.clientProxy->RestoreOnBundleFinished(BError(BError::Codes::OK), bundleName);
        it->second.backUpConnection->DisconnectBackupExtAbility();
        impl_.backupExtNameMap.erase(it);
    }
}

UniqueFd SvcSessionManager::OnBunleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (impl_.scenario != IServiceReverse::Scenario::BACKUP) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
    }

    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(move(fd));
    auto cache = cachedEntity.Structuralize();
    auto info = cache.GetExtManage();

    for (auto &fileName : info) {
        HILOGE("fileName %{public}s", fileName.data());
        OnBunleFileReady(bundleName, fileName);
    }

    shared_lock lock(lock_);
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    it->second.receExtManageJson = true;
    return move(cachedEntity.GetFd());
}

void SvcSessionManager::PublishFile(const string &bundleName, const string &fileName)
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    if (!it->second.backUpConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
    }

    auto proxy = it->second.backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }
    string pushName = fileName;
    ErrCode res = proxy->PublishFile(pushName);
    if (res) {
        HILOGE("Failed to publish file for backup extension");
    }
}

void SvcSessionManager::GetFileHandle(const string &bundleName, const string &fileName)
{
    shared_lock lock(lock_);
    HILOGE("BEGIN fileName %{public}s", fileName.data());
    if (impl_.scenario != IServiceReverse::Scenario::RESTORE) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
    }
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }

    if (!it->second.backUpConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
    }
    auto proxy = it->second.backUpConnection->GetBackupExtProxy();
    if (!proxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
    }
    string name = fileName;
    UniqueFd fd = proxy->GetFileHandle(name);
    if (fd < 0) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Failed to get file handle");
    }
    impl_.clientProxy->RestoreOnFileReady(bundleName, fileName, move(fd));
}

void SvcSessionManager::Start()
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }

    BConstants::ExtensionAction action;
    if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        action = BConstants::ExtensionAction::BACKUP;
    } else if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        action = BConstants::ExtensionAction::RESTORE;
    } else {
        throw BError(BError::Codes::SA_INVAL_ARG, "Failed to scenario");
    }

    for (auto it : impl_.backupExtNameMap) {
        AAFwk::Want want;
        want.SetElementName(it.first, it.second.backupExtName);
        want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));

        if (!it.second.backUpConnection) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
        }
        if (it.second.backUpConnection->IsExtAbilityConnected()) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Backup extension ability is connected");
        }

        ErrCode ret = it.second.backUpConnection->ConnectBackupExtAbility(want);
        HILOGI("Started %{public}s[100]'s %{public}s with %{public}s set to %{public}d", it.first.c_str(),
               it.second.backupExtName.c_str(), BConstants::EXTENSION_ACTION_PARA, action);

        auto proxy = it.second.backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Extension backup Proxy is empty");
        }

        if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
            ret = proxy->HandleBackup();
            impl_.clientProxy->BackupOnBundleStarted(ret, it.first);
        } else if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
            impl_.clientProxy->RestoreOnBundleStarted(ret, it.first);
            if (!it.second.backUpConnection->IsExtAbilityConnected()) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Failed to extension ability connected");
            }
            sched_->Sched(it.first);
        }
    }
}

bool SvcSessionManager::TryExtConnect(const std::string &bundleName)
{
    try {
        shared_lock lock(lock_);
        if (impl_.scenario != IServiceReverse::Scenario::RESTORE) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
        }
        if (!impl_.clientToken) {
            throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
        }

        auto it = impl_.backupExtNameMap.find(bundleName);
        if (it == impl_.backupExtNameMap.end()) {
            stringstream ss;
            ss << "Could not find the " << bundleName << " from current session";
            throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
        }
        if (!it->second.backUpConnection) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is empty");
        }

        if (!it->second.backUpConnection->IsExtAbilityConnected()) {
            return false;
        }
        auto proxy = it->second.backUpConnection->GetBackupExtProxy();
        if (!proxy) {
            return false;
        }
        return true;
    } catch (const BError &e) {
        return false;
    }
    return false;
}

void SvcSessionManager::QueueGetFileRequest(const string &bundleName, string &fileName)
{
    HILOGE("BEGIN fileName %{public}s", fileName.data());
    if (impl_.scenario != IServiceReverse::Scenario::RESTORE) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid Scenario");
    }
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }

    if (!sched_) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid sched scheduler");
    }
    sched_->QueueGetFileRequest(bundleName, fileName);
    sched_->Sched(bundleName);
}

void SvcSessionManager::OnBackupExtensionDied(const string &bundleName)
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }
    if (!impl_.clientProxy) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Invalid client");
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        BError(BError::Codes::SA_REFUSED_ACT, ss.str());
        return;
    }

    if (impl_.scenario == IServiceReverse::Scenario::BACKUP) {
        impl_.clientProxy->BackupOnBundleFinished(-ESRCH, bundleName);
    } else if (impl_.scenario == IServiceReverse::Scenario::RESTORE) {
        impl_.clientProxy->RestoreOnBundleFinished(-ESRCH, bundleName);
    }
    impl_.backupExtNameMap.erase(it);
}
} // namespace OHOS::FileManagement::Backup