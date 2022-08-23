/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_session_restore.h"
#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionRestore::~BSessionRestore()
{
    if (!deathRecipient_) {
        return;
    }
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return;
    }
    auto remoteObject = proxy->AsObject();
    if (remoteObject != nullptr) {
        HILOGI("remoteObject died. Died remote obj = %{public}p", remoteObject.GetRefPtr());
        remoteObject->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
}

unique_ptr<BSessionRestore> BSessionRestore::Init(std::vector<BundleName> bundlesToRestore, Callbacks callbacks)
{
    try {
        auto restore = make_unique<BSessionRestore>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacks), bundlesToRestore);
        if (res != 0) {
            HILOGE("Failed to Restore because of %{public}d", res);
            return nullptr;
        }

        restore->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return restore;
    } catch (const exception e) {
        HILOGE("Failed to Restore because of %{public}s", e.what());
    }
    return nullptr;
}

UniqueFd BSessionRestore::GetLocalCapabilities()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return UniqueFd(-1);
    }
    return UniqueFd(proxy->GetLocalCapabilities());
}

ErrCode BSessionRestore::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return ErrCode(BError::Codes::SDK_BROKEN_IPC);
    }
    return proxy->PublishFile(fileInfo);
}

ErrCode BSessionRestore::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC);
    }

    return proxy->Start();
}

ErrCode BSessionRestore::GetExtFileName(string &bundleName, string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to proxy because of is empty").GetCode();
    }

    return proxy->GetExtFileName(bundleName, fileName);
}

void BSessionRestore::RegisterBackupServiceDied(std::function<void()> functor)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return;
    }
    auto remoteObj = proxy->AsObject();
    if (!remoteObj) {
        throw BError(BError::Codes::SA_BROKEN_IPC, "Proxy's remote object can't be nullptr");
    }

    auto callback = [functor](const wptr<IRemoteObject> &obj) {
        HILOGI("service died. Died remote obj = %{public}p", obj.GetRefPtr());
        functor();
    };
    deathRecipient_ = sptr(new SvcDeathRecipient(callback));
    remoteObj->AddDeathRecipient(deathRecipient_);
}
} // namespace OHOS::FileManagement::Backup