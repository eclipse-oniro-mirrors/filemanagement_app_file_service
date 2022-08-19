/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <mutex>

#include "i_service.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class ServiceProxy : public IRemoteProxy<IService> {
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                              UniqueFd fd,
                              const std::vector<BundleName> &bundleNames) override;
    ErrCode Start() override;
    UniqueFd GetLocalCapabilities() override;
    ErrCode PublishFile(const BFileInfo &fileInfo) override;
    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) override;
    ErrCode AppDone(ErrCode errCode) override;
    ErrCode GetExtFileName(std::string &bundleName, std::string &fileName) override;

public:
    explicit ServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IService>(impl) {}
    ~ServiceProxy() override {}

public:
    static sptr<IService> GetInstance();
    static void FinishStartSA(const sptr<IRemoteObject> &remoteObject);
    static void FinishStartSAFailed();

private:
    static inline std::mutex proxyMutex_;
    static inline sptr<IService> serviceProxy_ = nullptr;
    static inline std::condition_variable proxyConVar_;
    static inline BrokerDelegator<ServiceProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H