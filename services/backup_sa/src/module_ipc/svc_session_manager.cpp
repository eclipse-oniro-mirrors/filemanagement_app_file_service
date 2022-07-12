/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/svc_session_manager.h"

#include <cstdint>
#include <sstream>

#include "ability_util.h"
#include "b_error/b_error.h"
#include "b_json/b_json_entity_usr_config.h"
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
    HILOGI("Succeed to deactive a session. Client token = %{public}u, client proxy = 0x%{private}p", impl_.clientToken,
           impl_.clientProxy.GetRefPtr());
    impl_ = {};
}

void SvcSessionManager::VerifyBundleName(string bundleName)
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
    }
}

const map<BundleName, BackupExtInfo> SvcSessionManager::GetBackupExtNameMap()
{
    shared_lock lock(lock_);
    if (!impl_.clientToken) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No caller token was specified");
    }

    return impl_.backupExtNameMap;
}

void SvcSessionManager::OnBunleFileReady(const string &bundleName, bool bundleDone, int32_t bundleTotalFiles)
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
    if (bundleDone) {
        it->second.numFilesTotal = bundleTotalFiles;
    } else
        it->second.numFilesSent++;
}

void SvcSessionManager::OnNewBundleConnected(const string &bundleName, sptr<SvcBackupConnection> &backUpConnection)
{
    shared_lock lock(lock_);
    if (!backUpConnection) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Svc backup connection is nullptr");
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
    it->second.backUpConnection = backUpConnection;
}
} // namespace OHOS::FileManagement::Backup