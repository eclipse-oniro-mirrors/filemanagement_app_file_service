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

#ifndef OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H
#define OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H

#include <string>
#include <vector>

#include "b_radar/radar_const.h"
#include "b_json/b_json_entity_caps.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "b_incremental_data.h"
#include "istorage_manager.h"

namespace OHOS::FileManagement::Backup {
class InnerReceiverImpl;

struct BundleExtInfo {
    BundleExtInfo(const std::string& bundleName) : bundleName_(bundleName),
        error_(RadarError(BError(BError::Codes::OK))) {}
    AppExecFwk::BundleInfo bundleInfo_;
    const std::string bundleName_;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos_;
    RadarError error_;
    uint32_t getExtSpend_ = 0;
};

class BundleMgrAdapter {
public:

    /**
     * @brief Get the bundle infos object
     *
     * @param bundleNames bundle names
     * @param userId User ID
     * @return std::vector<BJsonEntityCaps::BundleInfo>
     */
    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfos(const std::vector<std::string> &bundleNames,
                                                                   int32_t userId);

    /**
     * @brief Get app gallery bundle name
     */
    static std::string GetAppGalleryBundleName();

    /**
     * @brief Get the bundle infos object for incremental backup
     *
     * @param incrementalDataList bundle Name and time list
     * @param userId User ID
     * @return std::vector<BJsonEntityCaps::BundleInfo>
     */
    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForIncremental(
        const std::vector<BIncrementalData> &incrementalDataList, int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForIncremental(int32_t userId,
        const std::vector<BIncrementalData> &extraIncreData = {});

    static std::vector<BJsonEntityCaps::BundleInfo> GetFullBundleInfos(int32_t userId);

    static std::string GetExtName(string bundleName, int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForSA();

    static void GetBundleInfoForSA(std::string bundleName, std::vector<BJsonEntityCaps::BundleInfo> &bundleInfos);

    static bool IsUser0BundleName(std::string bundleName, int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForAppend(
        const std::vector<BIncrementalData> &incrementalDataList, int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForLocalCapabilities(int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForIndex(
        const vector<string> &bundleNames, int32_t userId);

    static int64_t GetBundleDataSize(const std::string &bundleName, int32_t userId);

    static void CreatBackupEnv(const std::vector<BIncrementalData> &bundleNameList, int32_t userId);

    static std::vector<BJsonEntityCaps::BundleInfo> GetBundleInfosForAppendBundles(
        const std::vector<BIncrementalData> &incrementalDataList, int32_t userId);
private:
    static bool GetCurBundleExtenionInfo(BundleExtInfo &bundleExtInfo, sptr<AppExecFwk::IBundleMgr> bms,
        int32_t userId);
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_BUNDLE_MGR_ADAPTER_H
