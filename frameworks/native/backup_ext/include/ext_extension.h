/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H

#include "ext_backup_js.h"
#include "ext_extension_stub.h"

namespace OHOS::FileManagement::Backup {
class BackupExtExtension : public ExtExtensionStub {
public:
    UniqueFd GetFileHandle(std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(std::string &fileName) override;
    ErrCode HandleBackup() override;

private:
    /**
     * @brief verify caller uid
     *
     */
    void VerifyCaller();

public:
    explicit BackupExtExtension(const std::shared_ptr<Backup::ExtBackupJs> &extension) : extension_(extension) {}
    ~BackupExtExtension() = default;

private:
    std::shared_ptr<ExtBackupJs> extension_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H