/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H

#include <string>
#include <vector>

#include "b_json/b_json_entity_usr_config.h"
#include "b_resources/b_constants.h"
#include "ext_backup_js.h"
#include "ext_extension_stub.h"
#include "thread_pool.h"

namespace OHOS::FileManagement::Backup {
class BackupExtExtension : public ExtExtensionStub {
public:
    UniqueFd GetFileHandle(std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(std::string &fileName) override;
    ErrCode HandleBackup() override;

public:
    explicit BackupExtExtension(const std::shared_ptr<Backup::ExtBackupJs> &extension) : extension_(extension)
    {
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
    }
    ~BackupExtExtension()
    {
        threadPool_.Stop();
    }

private:
    /**
     * @brief verify caller uid
     *
     */
    void VerifyCaller();

    /**
     * @brief backup
     *
     * @param usrConfig user configure
     */
    int HandleBackup(const BJsonEntityUsrConfig &usrConfig);

    /**
     * @brief restore
     *
     * @param fileName name of the file that to be untar
     */
    int HandleRestore(const string &fileName);

    /**
     * @brief Executing Backup Tasks Asynchronously
     *
     * @param extAction action
     *
     * @param config user configure
     */
    void AsyncTaskBackup(const std::string config);

    /**
     * @brief Executing Restoration Tasks Asynchronously
     *
     * @param fileName ready file to untar
     */
    void AsyncTaskRestore(const std::string fileName);

private:
    std::shared_ptr<ExtBackupJs> extension_;
    std::vector<std::string> tars_;
    OHOS::ThreadPool threadPool_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H