/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <iomanip>
#include <sstream>
#include <unistd.h>

#include "b_process/b_multiuser.h"
#include "b_radar/b_radar.h"
#include "b_resources/b_constants.h"
#include "b_utils/b_time.h"
#include "hisysevent.h"

namespace OHOS::FileManagement::Backup {
int32_t AppRadar::GetUserId()
{
    auto multiuser = BMultiuser::ParseUid(getuid());
    if ((multiuser.userId == BConstants::SYSTEM_UID) || (multiuser.userId == BConstants::XTS_UID)) {
        return BConstants::DEFAULT_USER_ID;
    }
    return multiuser.userId;
}

void AppRadar::RecordDefaultFuncRes(Info &info, const std::string &func, int32_t userId,
                                    enum BizStageBackup bizStage, int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        BConstants::FILE_BACKUP_RESTORE_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", info.bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", TimeUtils::GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BConstants::ExtensionAction::INVALID),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "EXEC_STATUS", info.status,
        "RESULT_CODE", resultCode,
        "RESULT_INFO", info.resInfo);
}

void AppRadar::RecordBackupFuncRes(Info &info, const std::string &func, int32_t userId,
                                   enum BizStageBackup bizStage, int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        BConstants::FILE_BACKUP_RESTORE_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", info.bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", TimeUtils::GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BConstants::ExtensionAction::BACKUP),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "EXEC_STATUS", info.status,
        "RESULT_CODE", resultCode,
        "RESULT_INFO", info.resInfo);
}

void AppRadar::RecordRestoreFuncRes(Info &info, const std::string &func, int32_t userId,
                                    enum BizStageRestore bizStage, int32_t resultCode)
{
    HiSysEventWrite(
        OHOS::HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT,
        BConstants::FILE_BACKUP_RESTORE_EVENTS,
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "BUNDLE_NAME", info.bundleName,
        "USER_ID", userId,
        "PID", getpid(),
        "FUNC", func,
        "TIME", TimeUtils::GetCurrentTime(),
        "BIZ_SCENE", static_cast<int32_t>(BConstants::ExtensionAction::RESTORE),
        "BIZ_STAGE", static_cast<int32_t>(bizStage),
        "EXEC_STATUS", info.status,
        "RESULT_CODE", resultCode,
        "RESULT_INFO", info.resInfo);
}
} // namespace OHOS::FileManagement::Backup