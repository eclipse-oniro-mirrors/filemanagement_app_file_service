/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy_load_callback.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"

namespace OHOS::FileManagement::Backup {
ServiceProxyLoadCallback::ServiceProxyLoadCallback() {}
void ServiceProxyLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
                                                          const sptr<IRemoteObject> &remoteObject)
{
    HILOGI("load backup SA success, systemAbilityId:%d, remoteObject result:%s", systemAbilityId,
           (remoteObject != nullptr) ? "true" : "false");
    if (remoteObject == nullptr) {
        HILOGE("remoteObject is nullptr");
        return;
    }

    ServiceProxy::FinishStartSA(remoteObject);
}
void ServiceProxyLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    HILOGE("load backup SA failed, systemAbilityId:%d", systemAbilityId);
    ServiceProxy::FinishStartSAFailed();
}
} // namespace OHOS::FileManagement::Backup
