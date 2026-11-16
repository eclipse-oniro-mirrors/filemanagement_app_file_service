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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_H

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
class BJsonClearDataConfig {
public:
    /**
     * @brief 设置配置文件所属用户，BConstants::INVALID_USER_ID 表示使用当前前台用户
     *
     * @param userId 用户id
     */
    void SetUserId(int32_t userId);

    /**
     * @brief 判断配置文件中是否有记录
     *
     */
    bool HasClearBundleRecord();

    /**
     * @brief 判断配置文件中是否存在bundleName记录
     *
     * @param bundlename
     */
    bool FindClearBundleRecord(const std::string& bundleName);

    /**
     * @brief 配置文件中插入bundleName记录
     *
     * @param bundlename
     */
    bool InsertClearBundleRecord(const std::string& bundleName);

    /**
     * @brief 配置文件中删除bundleName记录
     *
     * @param bundlename
     */
    bool DeleteClearBundleRecord(const std::string& bundleName);

    /**
     * @brief 配置文件中获取bundlename
     *
     */
    std::vector<std::string> GetAllClearBundleRecords();

    /**
     * @brief 删除配置文件
     *
     * @return 文件不存在或删除成功返回 true，删除失败返回 false
     */
    bool DeleteConfigFile();

    /**
     * @brief 删除指定用户的配置文件
     *
     * @param userId 用户id
     * @return 文件不存在或删除成功返回 true，删除失败返回 false
     */
    bool DeleteConfigFile(int32_t userId);

public:
    /**
     * @brief 构造方法
     *
     *
     */
    BJsonClearDataConfig();
private:
    /**
     * @brief 配置文件中删除bundleName记录
     *
     * @param bundlename
     */
    bool WriteClearBundleRecord(const std::string& bundleName, const std::string& filePath);
    void CreateConfigFileIfNotExist(const std::string& filePath);

private:
    std::atomic<int32_t> userId_ {BConstants::INVALID_USER_ID};
    std::mutex fileMutex_;
};

} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_JSON_CLEAR_DATA_CONFIG_H