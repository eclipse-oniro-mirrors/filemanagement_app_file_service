/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H

#include "i_service.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class ServiceProxy : public IRemoteProxy<IService> {
public:
    int32_t EchoServer(const std::string &echoStr) override;
    void DumpObj(const ComplexObject &obj) override;
    int32_t InitRestoreSession(std::vector<AppId> apps) override;
    int32_t GetLocalCapabilities() override;

public:
    explicit ServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IService>(impl) {}
    ~ServiceProxy() override {}

public:
    static sptr<IService> GetInstance();

private:
    static inline BrokerDelegator<ServiceProxy> delegator_;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_PROXY_H