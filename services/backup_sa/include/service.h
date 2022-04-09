/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_H

#include <mutex>

#include "iremote_stub.h"
#include "service_stub.h"
#include "system_ability.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class Service : public SystemAbility, public ServiceStub, protected NoCopyable {
    DECLARE_SYSTEM_ABILITY(Service);

public:
    explicit Service(int32_t saID, bool runOnCreate = true) : SystemAbility(saID, runOnCreate){};
    ~Service() = default;

    void OnStart() override;
    void OnStop() override;

    int32_t EchoServer(const std::string &echoStr) override;
    void DumpObj(const ComplexObject &obj) override;
    int32_t GetFd() override;

private:
    Service();
    static sptr<Service> instance_;
    static std::mutex instanceLock_;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H