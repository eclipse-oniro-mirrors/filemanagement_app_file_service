/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/svc_session_manager.h"

#include <sstream>

#include "ability_util.h"
#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static void VerifyBunldeNamesWithBundleMgr(const vector<BundleName> &bundleNames)
{
    if (bundleNames.empty()) {
        throw BError(BError::Codes::SA_INVAL_ARG, "No app was selected");
    }
    auto bms = AAFwk::AbilityUtil::GetBundleManager();
    if (!bms) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Bms is invalid");
    }

    vector<AppExecFwk::BundleInfo> bundleInfos;
    if (!bms->GetBundleInfos(AppExecFwk::GET_BUNDLE_DEFAULT, bundleInfos, AppExecFwk::Constants::START_USERID)) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Failed to get bundle infos");
    }

    for (auto &&bundleToVerify : bundleNames) {
        bool bVerify =
            none_of(bundleInfos.begin(), bundleInfos.end(),
                    [bundleToVerify](const AppExecFwk::BundleInfo &bInfo) { return bInfo.name == bundleToVerify; });
        if (bVerify) {
            stringstream ss;
            ss << "Could not find the " << bundleToVerify << " from current session";
            throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
        }
    }
}

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

void SvcSessionManager::Active(const Impl &newImpl)
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

    VerifyBunldeNamesWithBundleMgr(newImpl.bundlesToProcess);

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

void SvcSessionManager::VerifyBundleName(const string &bundleName)
{
    shared_lock lock(lock_);
    bool bVerify = none_of(impl_.bundlesToProcess.begin(), impl_.bundlesToProcess.end(),
                           [bundleName](const BundleName &name) { return bundleName == name; });
    if (bVerify) {
        stringstream ss;
        ss << "Could not find the " << bundleName << " from current session";
        throw BError(BError::Codes::SA_REFUSED_ACT, ss.str());
    }
    HILOGE("Succeed to verify the bundleName");
}
} // namespace OHOS::FileManagement::Backup