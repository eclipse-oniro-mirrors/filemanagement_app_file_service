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

#ifndef OHOS_FILEMGMT_BACKUP_NOTIFY_WORK_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_NOTIFY_WORK_SERVICE_H

#include <singleton.h>
#include <nocopyable.h>

#include "b_jsonutil/b_jsonutil.h"

namespace OHOS::FileManagement::Backup {
class NotifyWorkService final : public NoCopyable {
    DECLARE_DELAYED_SINGLETON(NotifyWorkService);
public:
    bool NotifyBundleDetail(BJsonUtil::BundleDetailInfo bundleDetailInfo);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_NOTIFY_WORK_SERVICE_H