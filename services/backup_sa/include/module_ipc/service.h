/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_H

#include <mutex>

#include "i_service_reverse.h"
#include "iremote_stub.h"
#include "service_stub.h"
#include "svc_session_manager.h"
#include "system_ability.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class Service final : public SystemAbility, public ServiceStub, protected NoCopyable {
    DECLARE_SYSTEM_ABILITY(Service);

    // 以下都是IPC接口
public:
    int32_t EchoServer(const std::string &echoStr) override;
    void DumpObj(const ComplexObject &obj) override;
    int32_t InitRestoreSession(sptr<IServiceReverse> remote, std::vector<AppId> apps) override;
    int32_t InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, std::vector<AppId> apps) override;
    int32_t GetLocalCapabilities() override;

    // 以下都是非IPC接口
public:
    void OnStart() override;
    void OnStop() override;
    void StopAll(const wptr<IRemoteObject> &obj, bool force = false);
    sptr<IServiceReverse> GetRemoteProxy();

public:
    explicit Service(int32_t saID, bool runOnCreate = false)
        : SystemAbility(saID, runOnCreate), session_(wptr(this)) {};
    ~Service() override = default;

private:
    static sptr<Service> instance_;
    static std::mutex instanceLock_;

    SvcSessionManager session_;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H