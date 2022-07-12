/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/svc_backup_connection.h"

#include "ability_manager_client.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/svc_extension_proxy.h"

namespace OHOS::FileManagement::Backup {
constexpr int WAIT_TIME = 1;
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
    std::unique_lock<std::mutex> lock(condition_.mutex);
    condition_.condition.notify_all();
    if (backupProxy_ == nullptr) {
        HILOGE("Failed to ability connect done, backupProxy_ is nullptr");
        return;
    }
    isConnected_.store(true);
    HILOGI("called end");
}

void SvcBackupConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    HILOGI("called begin");
    backupProxy_ = nullptr;
    isConnected_.store(false);
    HILOGI("called end");
}

ErrCode SvcBackupConnection::ConnectBackupExtAbility(AAFwk::Want &want)
{
    HILOGI("called begin");
    std::unique_lock<std::mutex> lock(condition_.mutex);
    ErrCode ret =
        AAFwk::AbilityManagerClient::GetInstance()->ConnectAbility(want, this, AppExecFwk::Constants::START_USERID);
    if (condition_.condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
                                      [this] { return backupProxy_ != nullptr; })) {
        HILOGI("Wait connect timeout.");
    }
    HILOGI("called end, ret=%{public}d", ret);
    return ret;
}

ErrCode SvcBackupConnection::DisconnectBackupExtAbility()
{
    HILOGI("called begin");
    std::unique_lock<std::mutex> lock(condition_.mutex);
    backupProxy_ = nullptr;
    isConnected_.store(false);
    ErrCode ret = AppExecFwk::AbilityManagerClient::GetInstance()->DisconnectAbility(this);
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