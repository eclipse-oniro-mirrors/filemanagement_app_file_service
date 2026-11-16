/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "b_process/b_userid.h"

#include "filemgmt_libhilog.h"
#include "os_account_manager.h"
#include "b_error/b_error.h"
#include "b_resources/b_constants.h"

#include <vector>

namespace OHOS::FileManagement::Backup {
int BUserId::GetUserId()
{
    int32_t osAccountId = BConstants::DEFAULT_USER_ID;
    ErrCode result = AccountSA::OsAccountManager::GetForegroundOsAccountLocalId(osAccountId);
    if (result != ERR_OK) {
        HILOGE("GetUserId error, err is %{public}d", result);
        return BConstants::DEFAULT_USER_ID;
    }
    HILOGD("Current foreground account userId=%{public}d", osAccountId);
    return osAccountId;
}
} // namespace OHOS::FileManagement::Backup
