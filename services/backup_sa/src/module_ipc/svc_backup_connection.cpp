/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/svc_backup_connection.h"

#include "ability_manager_client.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/svc_extension_proxy.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
constexpr int WAIT_TIME = 3;
using namespace std;

void SvcBackupConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                               const sptr<IRemoteObject> &remoteObject,
                                               int resultCode)
{
    HILOGI("called begin");
    if (remoteObject == nullptr) {
        HILOGE("Failed to ability connect done, remote is nullptr");
        return;
    }
    backupProxy_ = iface_cast<SvcExtensionProxy>(remoteObject);
    if (backupProxy_ == nullptr) {
        HILOGE("Failed to ability connect done, backupProxy_ is nullptr");
        return;
    }
    isConnected_.store(true);
    string bundleName = element.GetBundleName();
    callStart_(move(bundleName));
    HILOGI("called end");
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOGI("called begin");
    if (isConnectedDone_ == false) {
        string bundleName = element.GetBundleName();
        HILOGE("It's error that the backup extension dies before the backup sa. name : %{public}s", bundleName.data());
        callDied_(move(bundleName));
    }
    backupProxy_ = nullptr;
    isConnected_.store(false);
    condition_.notify_all();
    HILOGI("called end");
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want)
{
    HILOGI("called begin");
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret =
        AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, AppExecFwk::Constants::START_USERID);
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    HILOGI("called begin");
    isConnectedDone_.store(true);
    std::unique_lock<std::mutex> lock(mutex_);
    ErrCode ret = AppExecFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
    if (condition_.wait_for(lock, std::chrono::seconds(WAIT_TIME), [this] { return backupProxy_ == nullptr; })) {
        HILOGI("Wait until the connection ends");
    }
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

bool SvcBackupConnection::IsExtAbilityConnected()
{
    return isConnected_.load();
}

sptr<IExtension> SvcBackupConnection::GetBackupExtProxy()
{
    return backupProxy_;
}
} // namespace OHOS::FileManagement::Backup