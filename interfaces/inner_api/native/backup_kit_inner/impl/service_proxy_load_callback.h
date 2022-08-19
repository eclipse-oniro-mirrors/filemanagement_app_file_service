/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_LOAD_CALLBACK_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_LOAD_CALLBACK_H

#include <cstdint>
#include <string>

#include "refbase.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceProxyLoadCallback : public SystemAbilityLoadCallbackStub {
public:
    explicit ServiceProxyLoadCallback();
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
    void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_LOAD_CALLBACK_H