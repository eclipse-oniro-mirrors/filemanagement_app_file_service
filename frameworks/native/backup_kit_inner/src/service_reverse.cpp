/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_reverse.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverse::BackupOnFileReady(string bundleName, string fileName, int fd)
{
    HILOGI("bundlename = %{public}s, filename = %{private}s, fd = %{private}d", bundleName.c_str(), fileName.c_str(),
           fd);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(fd));
}

void ServiceReverse::BackupOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleStarted) {
        return;
    }
    callbacksBackup_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::BackupOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onBundleFinished) {
        return;
    }
    callbacksBackup_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::BackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onAllBundlesFinished) {
        return;
    }
    callbacksBackup_.onAllBundlesFinished(errCode);
}

void ServiceReverse::RestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleStarted) {
        return;
    }
    callbacksRestore_.onBundleStarted(errCode, bundleName);
}

void ServiceReverse::RestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("errCode = %{public}d, bundleName = %{public}s", errCode, bundleName.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onBundleFinished) {
        return;
    }
    callbacksRestore_.onBundleFinished(errCode, bundleName);
}

void ServiceReverse::RestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onAllBundlesFinished) {
        return;
    }
    callbacksRestore_.onAllBundlesFinished(errCode);
}

void ServiceReverse::RestoreOnFileReady(string bundleName, string fileName, int fd)
{
    HILOGI("bundlename = %{public}s, filename = %{private}s, fd = %{private}d", bundleName.c_str(), fileName.c_str(),
           fd);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onFileReady) {
        return;
    }
    BFileInfo bFileInfo(bundleName, fileName, 0);
    callbacksRestore_.onFileReady(bFileInfo, UniqueFd(fd));
}

ServiceReverse::ServiceReverse(BSessionBackup::Callbacks callbacks)
    : scenario_(Scenario::BACKUP), callbacksBackup_(callbacks)
{
}

ServiceReverse::ServiceReverse(BSessionRestore::Callbacks callbacks)
    : scenario_(Scenario::RESTORE), callbacksRestore_(callbacks)
{
}
} // namespace OHOS::FileManagement::Backup