/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>
#include <tuple>

#include <sys/stat.h>

#include "b_json/b_json_entity_extension_config.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_json/b_report_entity.h"
#include "b_radar/b_radar.h"
#include "b_radar/radar_app_statistic.h"
#include "ext_backup_js.h"
#include "extension_stub.h"
#include "service_common.h"
#include "iservice.h"
#include "tar_file.h"
#include "thread_pool.h"
#include "timer.h"
#include "unique_fd.h"
#include "untar_file.h"

namespace OHOS::FileManagement::Backup {
using CompareFilesResult = tuple<map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>,
                                 map<string, struct ReportFileInfo>>;
class BackupExtExtension : public ExtensionStub {
public:
    ErrCode GetFileHandleWithUniqueFd(const std::string &fileName, int32_t &errCode, int& fd) override;
    ErrCode HandleClear() override;
    ErrCode PublishFile(const std::string &fileName) override;
    ErrCode HandleBackup(bool isClearData) override;
    ErrCode HandleRestore(bool isClearData) override;
    ErrCode GetIncrementalFileHandle(const std::string &fileName, int &fd, int &reportFd, int32_t &fdErrCode) override;
    ErrCode PublishIncrementalFile(const std::string &fileName) override;
    ErrCode HandleIncrementalBackup(int incrementalFd, int manifestFd) override;
    ErrCode IncrementalOnBackup(bool isClearData) override;
    ErrCode GetIncrementalBackupFileHandle(int &fd, int &reportFd) override;
    ErrCode GetBackupInfo(std::string &result) override;
    ErrCode UpdateFdSendRate(const std::string &bundleName, int32_t sendRate) override;
    void AsyncTaskRestoreForUpgrade(void);
    void ExtClear(void);
    void AsyncTaskIncrementalRestoreForUpgrade(void);
    ErrCode User0OnBackup() override;
    ErrCode UpdateDfxInfo(int64_t uniqId, uint32_t extConnectSpend, const std::string &bundleName) override;
    ErrCode CleanBundleTempDir() override;
    ErrCode HandleOnRelease(int32_t scenario) override;
    ErrCode HandleGetCompatibilityInfo(const string &extInfo, int32_t scenario, string &compatibilityInfo) override;

public:
    explicit BackupExtExtension(const std::shared_ptr<Backup::ExtBackup> &extension,
        const std::string &bundleName) : extension_(extension)
    {
        if (extension_ != nullptr) {
            extension_->SetBackupExtExtension(this);
        }
        bundleName_ = bundleName;
        threadPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        onProcessTaskPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        reportOnProcessRetPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        doBackupPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        onReleaseTaskPool_.Start(BConstants::EXTENSION_THREAD_POOL_COUNT);
        SetStagingPathProperties();
        appStatistic_ = std::make_shared<RadarAppStatistic>();
    }
    ~BackupExtExtension()
    {
        onProcessTimeoutTimer_.Shutdown();
        threadPool_.Stop();
        onProcessTaskPool_.Stop();
        reportOnProcessRetPool_.Stop();
        doBackupPool_.Stop();
        onReleaseTaskPool_.Stop();
        if (callJsOnProcessThread_.joinable()) {
            callJsOnProcessThread_.join();
        }
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
     * @param bigFileInfo bigfiles to be backup
     * @param bigFileInfoBackuped bigfiles have been backuped
     * @param smallFiles smallfiles to be backup
     * @param includesNum sizeof includes
     * @param excludesNum sizeof excludes
     */
    int DoBackup(TarMap &bigFileInfo, TarMap &bigFileInfoBackuped, map<string, size_t> &smallFiles,
                 uint32_t includesNum, uint32_t excludesNum);

    /**
     * @brief backup
     *
     * @param bigFileInfo bigfiles to be backup
     * @param backupedFileSize backuped file size
     */
    int DoBackupBigFiles(TarMap &bigFileInfo, uint32_t backupedFileSize);

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

    /** @brief inner of doing clear backup restore data */
    void DoClearInner();

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

    bool IfAllowToBackupRestore();

    void AsyncTaskUser0Backup();

    void DoUser0Backup(const BJsonEntityExtensionConfig &usrConfig);

    int User0DoBackup(const BJsonEntityExtensionConfig &usrConfig);

    int DoIncrementalBackup(const std::vector<struct ReportFileInfo> &allFiles,
                            const std::vector<struct ReportFileInfo> &smallFiles,
                            const std::vector<struct ReportFileInfo> &bigFiles);

    void FillFileInfos(UniqueFd incrementalFd,
                       UniqueFd manifestFd,
                       vector<struct ReportFileInfo> &allFiles,
                       vector<struct ReportFileInfo> &smallFiles,
                       vector<struct ReportFileInfo> &bigFiles);
    void FillFileInfosWithoutCmp(vector<struct ReportFileInfo> &allFiles,
                                 vector<struct ReportFileInfo> &smallFiles,
                                 vector<struct ReportFileInfo> &bigFiles,
                                 UniqueFd incrementalFd);
    void FillFileInfosWithCmp(vector<struct ReportFileInfo> &allFiles,
                              vector<struct ReportFileInfo> &smallFiles,
                              vector<struct ReportFileInfo> &bigFiles,
                              const unordered_map<string, struct ReportFileInfo> &cloudFiles,
                              UniqueFd incrementalFd);
    void CompareFiles(vector<struct ReportFileInfo> &allFiles,
                      vector<struct ReportFileInfo> &smallFiles,
                      vector<struct ReportFileInfo> &bigFiles,
                      const unordered_map<string, struct ReportFileInfo> &cloudFiles,
                      unordered_map<string, struct ReportFileInfo> &localFilesInfo);

    void AsyncTaskDoIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd);
    void AsyncTaskOnIncrementalBackup();
    int DoIncrementalBackupTask(UniqueFd incrementalFd, UniqueFd manifestFd);
    ErrCode IncrementalBigFileReady(TarMap &pkgInfo, const vector<struct ReportFileInfo> &bigInfos,
        sptr<IService> proxy);
    ErrCode BigFileReady(TarMap &bigFileInfo, sptr<IService> proxy, int backupedFileSize);
    void WaitToSendFd(std::chrono::system_clock::time_point &startTime, int &fdSendNum);
    void RefreshTimeInfo(std::chrono::system_clock::time_point &startTime, int &fdSendNum);
    void IncrementalPacket(const vector<struct ReportFileInfo> &infos, TarMap &tar, sptr<IService> proxy);
    void DoPacket(const map<string, size_t> &srcFiles, TarMap &tar, sptr<IService> proxy);
    void CheckTmpDirFileInfos(bool isSpecialVersion = false);
    std::map<std::string, off_t> GetIdxFileInfos(bool isSpecialVersion = false);
    tuple<bool, vector<string>> CheckRestoreFileInfos();
    void CheckAppIncrementalFileReadyResult(int32_t ret, std::string packageName, std::string file);
    /**
     * @brief extension incremental backup restore is done
     *
     * @param errCode
     */
    void AppIncrementalDone(ErrCode errCode);

    /**
     * @brief start extension timer by ipc
     *
     * @param result
     */
    void StartExtTimer(bool &isExtStart);

    /**
     * @brief start fwk timer by ipc
     *
     * @param errCode
     */
    void StartFwkTimer(bool &isFwkStart);

    /**
     * @brief stop ext timer by ipc
     */
    bool StopExtTimer();

    /**
     * @brief refresh datasize
     *
     * @param totalSize backup totalSize
     */
    bool RefreshDataSize(int64_t totalSize);

    /**
     * @brief scanning files and calculate datasize
     *
     * @param usrConfig usrConfig
     * @param totalSize totalSize
     * @param bigFileInfo bigFileInfo
     * @param smallFiles smallFiles info
     */
    tuple<ErrCode, uint32_t, uint32_t> CalculateDataSize(const BJsonEntityExtensionConfig &usrConfig,
        int64_t &totalSize, TarMap &bigFileInfo, map<string, size_t> &smallFiles);

    /**
     * @brief get increCallbackEx for execute onRestore with string param
     *
     * @param obj
     */
    std::function<void(ErrCode, const std::string)> IncreOnRestoreExCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get increCallback for execute onRestore with string param
     *
     * @param obj
     */
    std::function<void(ErrCode, const std::string)> IncreOnRestoreCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get callback for execute onRestore with string param
     *
     * @param obj
     */
    std::function<void(ErrCode, std::string)> OnRestoreCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute onRestore with string param
     *
     * @param obj
     */
    std::function<void(ErrCode, std::string)> OnRestoreExCallback(wptr<BackupExtExtension> obj);

    /**
     * @brief get callbackEx for execute appDone
     */
    std::function<void(ErrCode, std::string)> AppDoneCallbackEx(wptr<BackupExtExtension> obj);

    std::function<void(ErrCode, const std::string)> IncOnBackupExCallback(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> IncOnBackupCallback(wptr<BackupExtExtension> obj);

    std::function<void(ErrCode, const std::string)> OnBackupExCallback(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> OnBackupCallback(wptr<BackupExtExtension> obj);

    void HandleSpecialVersionRestore();
    void DeleteBackupIncrementalIdxFile();
    void DeleteBackupIdxFile();
    void DeleteBackupIncrementalTars(const string &tarName);
    void SetClearDataFlag(bool isClearData);
    std::string GetBundlePath();
    std::vector<ExtManageInfo> GetExtManageInfo(bool isSpecialVersion = false);
    ErrCode RestoreFilesForSpecialCloneCloud();
    void RestoreBigFilesForSpecialCloneCloud(const ExtManageInfo &item);
    ErrCode RestoreTarForSpecialCloneCloud(const ExtManageInfo &item);
    void RestoreBigFiles(bool appendTargetPath);
    void FillEndFileInfos(const std::string &path, const unordered_map<string, struct ReportFileInfo> &result);
    void RestoreBigFileAfter(const string &filePath, const struct stat &sta);
    int DealIncreUnPacketResult(const off_t tarFileSize, const std::string &tarFileName,
        const std::tuple<int, EndFileInfo, ErrFileInfo> &result);

    ErrCode StartOnProcessTaskThread(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario);
    void FinishOnProcessTask();
    void ExecCallOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario);
    void AsyncCallJsOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario);
    void SyncCallJsOnProcessTask(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario);
    void StartOnProcessTimeOutTimer(wptr<BackupExtExtension> obj, BackupRestoreScenario scenario);
    void CloseOnProcessTimeOutTimer();
    void UpdateOnStartTime();
    int32_t GetOnStartTimeCost();
    bool SetStagingPathProperties();

    std::function<void(std::string, int)> ReportErrFileByProc(wptr<BackupExtExtension> obj,
        BackupRestoreScenario scenario);
    std::tuple<ErrCode, UniqueFd, UniqueFd> GetIncreFileHandleForNormalVersion(const std::string &fileName);
    void RestoreOneBigFile(const std::string &path, const ExtManageInfo &item, const bool appendTargetPath);
    int DealIncreRestoreBigAndTarFile();
    ErrCode IncrementalTarFileReady(const TarMap &bigFileInfo, const vector<struct ReportFileInfo> &srcFiles,
        sptr<IService> proxy);
    ErrCode IncrementalAllFileReady(const TarMap &pkgInfo, const vector<struct ReportFileInfo> &srcFiles,
        sptr<IService> proxy);
    void ClearNoPermissionFiles(TarMap &pkgInfo, vector<std::string> &noPermissionFiles);
    std::function<void(ErrCode, std::string)> ReportOnProcessResultCallback(wptr<BackupExtExtension> obj,
        BackupRestoreScenario scenario);
    UniqueFd GetFileHandle(const std::string &fileName, int32_t &errCode);
    std::tuple<ErrCode, UniqueFd, UniqueFd> GetIncrementalFileHandle(const std::string &fileName);
    ErrCode HandleIncrementalBackup(UniqueFd incrementalFd, UniqueFd manifestFd);
    std::tuple<UniqueFd, UniqueFd> GetIncrementalBackupFileHandle();
    bool IfCloudSpecialRestore(std::string tarName);
    ErrCode CloudSpecialRestore(std::string tarName, std::string untarPath, off_t tarFileSize);
    void GetTarIncludes(const string &tarName, unordered_map<string, struct ReportFileInfo> &infos);
    void DeleteIndexAndRpFile();
    ErrCode RestoreTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList);
    bool CheckIsSplitTarList(const std::vector<const ExtManageInfo> &tarList);
    ErrCode RestoreUnSplitTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList);
    ErrCode RestoreSplitTarListForSpecialCloneCloud(const std::vector<const ExtManageInfo> &tarList);

    tuple<ErrCode, UniqueFd, UniqueFd> GetIncreFileHandleForSpecialVersion(const string &fileName);
    void RmBigFileReportForSpecialCloneCloud(const std::string &srcFile);
    string GetReportFileName(const string &fileName);
    void OnBackupFinish();
    void OnBackupExFinish();
    void OnRestoreFinish();
    void OnRestoreExFinish();
    void DoBackupStart();
    void DoBackupEnd();
    void CalculateDataSizeTask(const string &config);
    void DoBackUpTask(const string &config);
    TarMap convertFileToBigFiles(std::map<std::string, struct stat> files);
    void PreDealExcludes(std::vector<std::string> &excludes);
    template <typename T>
    map<string, T> MatchFiles(map<string, T> files, vector<string> endExcludes);
    void UpdateTarStat(uint64_t tarFileSize);

    void HandleExtDisconnect(bool isAppResultReport, ErrCode errCode);
    bool HandleGetExtOnRelease();
    void HandleExtOnRelease(bool isAppResultReport, ErrCode errCode);
    std::function<void(ErrCode, const std::string)> OnReleaseCallback(wptr<BackupExtExtension> obj);
    std::function<void(ErrCode, const std::string)> GetComInfoCallback(wptr<BackupExtExtension> obj);
private:
    pair<TarMap, map<string, size_t>> GetFileInfos(const vector<string> &includes, const vector<string> &excludes);
    TarMap GetIncrmentBigInfos(const vector<struct ReportFileInfo> &files);
    void UpdateFileStat(std::string filePath, uint64_t fileSize);
    void ReportAppStatistic(const std::string &func, ErrCode errCode);
    ErrCode IndexFileReady(const TarMap &pkgInfo, sptr<IService> proxy);

    std::shared_mutex lock_;
    std::shared_ptr<ExtBackup> extension_;
    std::string backupInfo_;
    OHOS::ThreadPool threadPool_;
    std::mutex updateSendRateLock_;
    std::condition_variable startSendFdRateCon_;
    std::condition_variable waitSendFdCon_;
    std::mutex startSendMutex_;
    std::mutex waitTimeLock_;
    std::string bundleName_;
    int32_t sendRate_ = BConstants::DEFAULT_FD_SEND_RATE;
    bool isClearData_ {true};
    bool isDebug_ {true};
    std::map<std::string, off_t> endFileInfos_;
    std::map<std::string, std::vector<ErrCode>> errFileInfos_;
    bool isRpValid_ {false};

    std::thread callJsOnProcessThread_;
    Utils::Timer onProcessTimeoutTimer_ {"onProcessTimeoutTimer_"};
    uint32_t onProcessTimeoutTimerId_ { 0 };
    std::atomic<int> onProcessTimeoutCnt_ { 0 };
    std::atomic<bool> stopCallJsOnProcess_ {false};
    std::condition_variable execOnProcessCon_;
    std::mutex onProcessLock_;
    std::atomic<bool> onProcessTimeout_ {false};
    std::chrono::time_point<std::chrono::system_clock> g_onStart;
    std::mutex onStartTimeLock_;
    AppRadar::DoRestoreInfo radarRestoreInfo_ { 0 };
    OHOS::ThreadPool onProcessTaskPool_;
    std::atomic<bool> isFirstCallOnProcess_ {false};
    std::atomic<bool> isExecAppDone_ {false};
    OHOS::ThreadPool reportOnProcessRetPool_;
    OHOS::ThreadPool doBackupPool_;

    std::mutex reportHashLock_;
    std::map<std::string, std::string> reportHashSrcPathMap_;

    std::shared_ptr<RadarAppStatistic> appStatistic_ = nullptr;
    BackupRestoreScenario curScenario_ { BackupRestoreScenario::FULL_BACKUP };

    OHOS::ThreadPool onReleaseTaskPool_;
    std::atomic<bool> stopWaitOnRelease_ {false};
    std::mutex onReleaseLock_;
    std::condition_variable execOnReleaseCon_;
    std::atomic<bool> needAppResultReport_ {false};
    std::string appResultReportInfo_;
    ErrCode appResultReportErrCode_ { 0 };
    std::mutex execOnReleaseLock_ {};
    std::atomic<bool> isOnReleased_ {false};

    std::mutex getCompatibilityInfoLock_ {};
    std::condition_variable getCompatibilityInfoCon_ {};
    std::atomic<bool> stopGetComInfo_ {false};
    std::string compatibilityInfo_ {};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_BACKUP_EXT_EXTENSION_H