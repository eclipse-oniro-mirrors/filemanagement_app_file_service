/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H

#include "b_session_backup.h"
#include "b_session_restore.h"
#include "service_reverse_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverse final : public ServiceReverseStub {
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
    ServiceReverse() = delete;
    ServiceReverse(BSessionRestore::Callbacks callbacks);
    ServiceReverse(BSessionBackup::Callbacks callbacks);
    ~ServiceReverse() override = default;

private:
    Scenario scenario_ {Scenario::UNDEFINED};
    BSessionBackup::Callbacks callbacksBackup_;
    BSessionRestore::Callbacks callbacksRestore_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_H