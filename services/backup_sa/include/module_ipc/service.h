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

namespace OHOS::FileManagement::Backup {
class Service final : public SystemAbility, public ServiceStub, protected NoCopyable {
    DECLARE_SYSTEM_ABILITY(Service);

    // 以下都是IPC接口
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                              UniqueFd fd,
                              const std::vector<BundleName> &bundleNames) override;
    UniqueFd GetLocalCapabilities() override;
    std::tuple<ErrCode, TmpFileSN, UniqueFd> GetFileOnServiceEnd() override;
    ErrCode PublishFile(const BFileInfo &fileInfo) override;

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
    static inline std::atomic<uint32_t> seed {1};

    SvcSessionManager session_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H