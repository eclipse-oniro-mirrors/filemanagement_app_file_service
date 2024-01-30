/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_STORAGE_MGR_ADAPTER_H
#define OHOS_FILEMGMT_BACKUP_STORAGE_MGR_ADAPTER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "istorage_manager.h"

namespace OHOS::FileManagement::Backup {
class StorageMgrAdapter {
public:
    /**
     * @brief Get the bundle stats object
     *
     * @param bundleName bundle name
     */
    static StorageManager::BundleStats GetBundleStats(const std::string &bundleName);

    /**
     * @brief Get the user storage stats object
     *
     * @param bundleName bundle name
     * @param userId user id
     */
    static int64_t GetUserStorageStats(const std::string &bundleName, int32_t userId);

    /**
     * @brief update memory para
     *
     * @param size para data
    */
    static int32_t UpdateMemPara(int32_t size);

    /**
     * @brief Get the user storage stats object
     *
     * @param userId user id
     * @param bundleNames
     * @param incrementalBackTimes
     * @param pkgFileSizes bundle backup file size
     */
    static int32_t GetBundleStatsForIncrease(uint32_t userId, const std::vector<std::string> &bundleNames,
        const std::vector<int64_t> &incrementalBackTimes, std::vector<int64_t> &pkgFileSizes);
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_STORAGE_MGR_ADAPTER_H