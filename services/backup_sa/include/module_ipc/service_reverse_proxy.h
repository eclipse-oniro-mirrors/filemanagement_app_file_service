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
    void BackupOnFileReady(std::string bundleName, std::string fileName, int fd) override;
    void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void BackupOnAllBundlesFinished(int32_t errCode) override;
    void BackupOnBackupServiceDied() override;

    void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override;
    void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override;
    void RestoreOnAllBundlesFinished(int32_t errCode) override;
    void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd) override;
    void RestoreOnBackupServiceDied() override;

public:
    explicit ServiceReverseProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IServiceReverse>(impl) {}
    ~ServiceReverseProxy() override = default;

private:
    static inline BrokerDelegator<ServiceReverseProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_PROXY_H