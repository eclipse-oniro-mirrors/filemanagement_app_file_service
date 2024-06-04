/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H

#include <shared_mutex>
#include <string>
#include <vector>
#include <tuple>

#include <sys/stat.h>

#include "b_json/b_json_entity_extension_config.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_json/b_report_entity.h"
#include "ext_backup_js.h"
#include "ext_extension_stub.h"
#include "i_service.h"
#include "thread_pool.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using CompareFilesResult = tuple<map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>>;
class BackupExtExtension : public ExtExtensionStub {
public:
    UniqueFd GetFileHandle(const std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(const std::string &fileName) override;
    ErrCode HandleBackup() override;
    ErrCode HandleRestore() override;
    ErrCode GetIncrementalFileHandle(const std::string &fileName) override;
    ErrCode PublishIncrementalFile(const std::string &fileName) override;
    ErrCode HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd) override;
    ErrCode IncrementalOnBackup() override;
    std::tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle() override;
    ErrCode GetBackupInfo(std::string &result) override;

    void AsyncTaskRestoreForUpgrade(void);
    void ExtClear(void);
    void AsyncTaskIncrementalRestoreForUpgrade(void);

public:
    explicit BackupExtExtension(const std::shared_ptr<Backup::ExtBackup> &extension) : extension_(extension)
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
    int DoBackup(const BJsonEntityExtensionConfig &usrConfig);

    /**
     * @brief restore
     *
     * @param fileName name of the file that to be untar
     */
    int DoRestore(const string &fileName);

    /**
     * @brief incremental restore
     *
     */
    int DoIncrementalRestore();

    /** @brief clear backup restore data */
    void DoClear();

    /**
     * @brief extension backup restore is done
     *
     * @param errCode
     */
    void AppDone(ErrCode errCode);

    /**
     * @brief extension backup restore is done
     *
     * @param restoreRetInfo app restore reportInfo
     */
    void AppResultReport(const std::string restoreRetInfo, BackupRestoreScenario scenario);

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
     */
    void AsyncTaskRestore(std::set<std::string> fileSet, const std::vector<ExtManageInfo> extManageInfo);

    /**
     * @brief Executing Incremental Restoration Tasks Asynchronously
     *
     */
    void AsyncTaskIncrementalRestore();

    /**
     * @brief Executing Incremental Restoration Tasks Asynchronously for special clone & cloud
     *
     */
    void AsyncTaskIncreRestoreSpecialVersion();

    void AsyncTaskOnBackup();

    int DoIncrementalBackup(const std::map<std::string, struct ReportFileInfo> &allFiles,
                            const std::map<std::string, struct ReportFileInfo> &smallFiles,
                            const std::map<std::string, struct ReportFileInfo> &bigFiles);

    void CompareFiles(UniqueFd incrementalFd,
                      UniqueFd manifestFd,
                      map<string, struct ReportFileInfo> &allFiles,
                      map<string, struct ReportFileInfo> &smallFiles,
                      map<string, struct ReportFileInfo> &bigFiles);

    void AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd);
    void AsyncTaskOnIncrementalBackup();

    /**
     * @brief extension incremental backup restore is done
     *
     * @param errCode
     */
    void AppIncrementalDone(ErrCode errCode);

    /**
     * @brief get callbackEx for execute onRestore
     *
     * @param errCode
     */
    std::function<void(const std::string)> RestoreResultCallbackEx(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute onRestore with string param
     * 
     * @param errCode
     */
    std::function<void(const std::string)> IncrementalRestoreResultCallbackEx(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute onRestore
     *
     * @param errCode
     */
    std::function<void()> IncrementalAppDoneCallbackEx(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute appDone
     */
    std::function<void()> AppDoneCallbackEx(wptr<BackupExtExtension> obj);
private:
    std::shared_mutex lock_;
    std::shared_ptr<ExtBackup> extension_;
    std::string backupInfo_;
    OHOS::ThreadPool threadPool_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H