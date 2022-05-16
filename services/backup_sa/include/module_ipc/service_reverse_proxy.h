/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H

#include "i_service_reverse.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseProxy final : public IRemoteProxy<IServiceReverse>, protected NoCopyable {
public:
    void BackupOnFileReady(std::string appId, std::string fileName, int fd) override;
    void BackupOnSubTaskStarted(int32_t errCode, std::string appId) override;
    void BackupOnSubTaskFinished(int32_t errCode, std::string appId) override;
    void BackupOnTaskFinished(int32_t errCode) override;

    void RestoreOnSubTaskStarted(int32_t errCode, std::string appId) override;
    void RestoreOnSubTaskFinished(int32_t errCode, std::string appId) override;
    void RestoreOnTaskFinished(int32_t errCode) override;

public:
    explicit ServiceReverseProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IServiceReverse>(impl) {}
    ~ServiceReverseProxy() override = default;

private:
    static inline BrokerDelegator<ServiceReverseProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H