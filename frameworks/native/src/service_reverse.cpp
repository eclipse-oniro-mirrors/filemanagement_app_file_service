/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_reverse.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverse::BackupOnFileReady(std::string appId, std::string fileName, int fd)
{
    HILOGI("appid = %{public}s, filename = %{public}s, fd = %{private}d", appId.c_str(), fileName.c_str(), fd);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onFileReady) {
        return;
    }
    BFileInfo bFileInfo(appId, fileName, 0);
    callbacksBackup_.onFileReady(bFileInfo, UniqueFd(fd));
}

void ServiceReverse::BackupOnSubTaskStarted(int32_t errCode, std::string appId)
{
    HILOGI("errCode = %{public}d, appId = %{public}s", errCode, appId.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onSubTaskStarted) {
        return;
    }
    callbacksBackup_.onSubTaskStarted(errCode, appId);
}

void ServiceReverse::BackupOnSubTaskFinished(int32_t errCode, std::string appId)
{
    HILOGI("errCode = %{public}d, appId = %{public}s", errCode, appId.c_str());
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onSubTaskFinished) {
        return;
    }
    callbacksBackup_.onSubTaskFinished(errCode, appId);
}

void ServiceReverse::BackupOnTaskFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::BACKUP || !callbacksBackup_.onTaskFinished) {
        return;
    }
    callbacksBackup_.onTaskFinished(errCode);
}

void ServiceReverse::RestoreOnSubTaskStarted(int32_t errCode, std::string appId)
{
    HILOGI("errCode = %{public}d, appId = %{public}s", errCode, appId.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onSubTaskStarted) {
        return;
    }
    callbacksRestore_.onSubTaskStarted(errCode, appId);
}

void ServiceReverse::RestoreOnSubTaskFinished(int32_t errCode, std::string appId)
{
    HILOGI("errCode = %{public}d, appId = %{public}s", errCode, appId.c_str());
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onSubTaskFinished) {
        return;
    }
    callbacksRestore_.onSubTaskFinished(errCode, appId);
}

void ServiceReverse::RestoreOnTaskFinished(int32_t errCode)
{
    HILOGI("errCode = %{public}d", errCode);
    if (scenario_ != Scenario::RESTORE || !callbacksRestore_.onTaskFinished) {
        return;
    }
    callbacksRestore_.onTaskFinished(errCode);
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