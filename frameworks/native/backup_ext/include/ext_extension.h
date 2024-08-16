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

#include <chrono>
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
#include "tar_file.h"
#include "thread_pool.h"
#include "timer.h"
#include "unique_fd.h"
#include "untar_file.h"

namespace OHOS::FileManagement::Backup {
using CompareFilesResult = tuple<map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>>;
class BackupExtExtension : public ExtExtensionStub {
public:
    UniqueFd GetFileHandle(const std::string &fileName, int32_t &errCode) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(const std::string &fileName) override;
    ErrCode HandleBackup(bool isClearData) override;
    ErrCode HandleRestore(bool isClearData) override;
    ErrCode GetIncrementalFileHandle(const std::string &fileName) override;
    ErrCode PublishIncrementalFile(const std::string &fileName) override;
    ErrCode HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd) override;
    ErrCode IncrementalOnBackup(bool isClearData) override;
    std::tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle() override;
    ErrCode GetBackupInfo(std::string &result) override;
    ErrCode UpdateFdSendRate(std::string &bundleName, int32_t sendRate) override;
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
    int DoRestore(const string &fileName, const off_t fileSize);

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
    void AppResultReport(const std::string restoreRetInfo, BackupRestoreScenario scenario,
        ErrCode errCode = 0);

    /**
     * @brief extension process Info
     *
     * @param restoreRetInfo app processInfo
     * @param scenario backup or restore
     * @param errCode errCode
     */
    void ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario scenario);

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

    int DoIncrementalBackup(const std::vector<struct ReportFileInfo> &allFiles,
                            const std::vector<struct ReportFileInfo> &smallFiles,
                            const std::vector<struct ReportFileInfo> &bigFiles);

    void CompareFiles(UniqueFd incrementalFd,
                      UniqueFd manifestFd,
                      vector<struct ReportFileInfo> &allFiles,
                      vector<struct ReportFileInfo> &smallFiles,
                      vector<struct ReportFileInfo> &bigFiles);

    void AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd);
    void AsyncTaskOnIncrementalBackup();
    ErrCode IncrementalBigFileReady(const TarMap &pkgInfo, const vector<struct ReportFileInfo> &bigInfos,
        sptr<IService> proxy);
    ErrCode BigFileReady(const TarMap &bigFileInfo, sptr<IService> proxy);
    void WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum);
    void RefreshTimeInfo(std::chrono::system_clock::time_point &startTime, int &fdSendNum);
    void IncrementalPacket(const vector<struct ReportFileInfo> &infos, TarMap &tar, sptr<IService> proxy);
    void DoPacket(const map<string, size_t> &srcFiles, TarMap &tar, sptr<IService> proxy);
    void CheckTmpDirFileInfos(bool isSpecialVersion = false);
    std::map<std::string, off_t> GetIdxFileInfos(bool isSpecialVersion = false);
    tuple<bool, vector<string>> CheckRestoreFileInfos();
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
    std::function<void(ErrCode, const std::string)> FullRestoreCallbackEx(wptr<BackupExtExtension> obj);

     /**
     * @brief get callback for execute onRestore
     *
     * @param errCode
     */
    std::function<void(ErrCode, const std::string)> FullRestoreCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute onRestore with string param
     *
     * @param errCode
     */
    std::function<void(ErrCode, const std::string)> IncRestoreResultCallbackEx(wptr<BackupExtExtension> obj);

    /**
     * @brief get callback for execute onRestore with string param
     *
     * @param errCode
     */
    std::function<void(ErrCode, const std::string)> IncRestoreResultCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute onRestore
     *
     * @param errCode
     */
    std::function<void(ErrCode, std::string)> IncAppDoneCallbackEx(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute appDone
     */
    std::function<void(ErrCode, std::string)> AppDoneCallbackEx(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> HandleBackupEx(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> HandleFullBackupCallbackEx(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> HandleFullBackupCallback(wptr<BackupExtExtension> obj);

    void HandleSpecialVersionRestore();
    void DeleteBackupIncrementalTars();
    void DeleteBackupTars();
    void SetClearDataFlag(bool isClearData);
    std::vector<ExtManageInfo> GetExtManageInfo();
    ErrCode RestoreFilesForSpecialCloneCloud();
    void RestoreBigFilesForSpecialCloneCloud(const ExtManageInfo &item);
    ErrCode RestoreTarForSpecialCloneCloud(const ExtManageInfo &item);
    void RestoreBigFiles(bool appendTargetPath);
    void FillEndFileInfos(const std::string &path, const unordered_map<string, struct ReportFileInfo> &result);
    void RestoreBigFileAfter(const string &filePath, const struct stat &sta);
    void DealIncreUnPacketResult(const off_t tarFileSize, const std::string &tarFileName,
        const std::tuple<int, EndFileInfo, ErrFileInfo> &result);









    std::function<void(ErrCode, const std::string)> ExecOnProcessCallback(wptr<BackupExtExtension> obj,
        BackupRestoreScenario scenario);
    // std::function<void(ErrCode, const std::string)> ExecOnProcessTimeoutCallback(wptr<BackupExtExtension> obj);
    bool CreateExecOnProcessTask(BackupRestoreScenario scenario);
    bool ShutDownExecOnProcessTask();
    bool ExecOnProcessTimeoutTask(wptr<BackupExtExtension> obj);
    bool ShutDownOnProcessTimeoutTask();


private:
    std::shared_mutex lock_;
    std::shared_ptr<ExtBackup> extension_;
    std::string backupInfo_;
    std::string bundleProcessInfo_;
    OHOS::ThreadPool threadPool_;
    std::mutex updateSendRateLock_;
    std::condition_variable startSendFdRateCon_;
    std::condition_variable waitSendFdCon_;
    std::mutex startSendMutex_;
    std::mutex waitTimeLock_;
    std::string bundleName_;
    int32_t sendRate_ = BConstants::DEFAULT_FD_SEND_RATE;
    bool isClearData_ {true};
    bool isDebug_ {false};
    std::map<std::string, off_t> endFileInfos_;
    std::map<std::string, std::vector<ErrCode>> errFileInfos_;
    bool isRpValid_ {false};






















    Utils::Timer execOnProcessTaskTimer_ {"execOnProcessTaskTimer_"};
    uint32_t execOnProcessTaskTimerId_;
    Utils::Timer execOnProcessTimeoutTimer_ {"execOnProcessTimeoutTimer_"};
    uint32_t execOnProcessTimeoutTimerId_;
    int execOnProcessTimeoutTimes_;
    bool isSameClock_ {false};
    bool appExecFinished_ {false};

};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H