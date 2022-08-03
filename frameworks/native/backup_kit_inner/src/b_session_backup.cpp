/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_session_backup.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BSessionBackup::~BSessionBackup()
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
        remoteObject->RemoveDeathRecipient(deathRecipient_);
    }
    deathRecipient_ = nullptr;
}

unique_ptr<BSessionBackup> BSessionBackup::Init(UniqueFd remoteCap,
                                                vector<BundleName> bundlesToBackup,
                                                Callbacks callbacks)
{
    try {
        auto backup = make_unique<BSessionBackup>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            return nullptr;
        }

        int32_t res = proxy->InitBackupSession(sptr(new ServiceReverse(callbacks)), move(remoteCap), bundlesToBackup);
        if (res != 0) {
            HILOGE("Failed to Backup because of %{public}d", res);
            return nullptr;
        }

        backup->RegisterBackupServiceDied(callbacks.onBackupServiceDied);
        return backup;
    } catch (const exception e) {
        HILOGE("Failed to Backup because of %{public}s", e.what());
    }
    return nullptr;
}

void BSessionBackup::RegisterBackupServiceDied(std::function<void()> functor)
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

ErrCode BSessionBackup::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return ErrCode(BError::Codes::SDK_BROKEN_IPC);
    }

    return proxy->Start();
}
} // namespace OHOS::FileManagement::Backup