/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_HANDLE_CONFIG_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_HANDLE_CONFIG_H

#include <cstdint>
#include <string>
#include <tuple>

namespace OHOS::FileManagement::Backup {
class BJsonHandleConfig {
public:
    /**
     * @brief 获取配置参数的值
     *
     * @param key 配置参数的参数名
     * @param len 配置参数值的最大长度
     * @return 成功获取配置参数的值则返回true，失败则返回false；以及表示配置参数值的字符串
     */
    static std::tuple<bool, std::string> GetConfigParameterValue(const std::string &key, uint32_t len);

    /**
     * @brief 读取json文件中的配置项
     *
     * @param jsonCfgFilePath json文件的路径
     * @return 读取到的json文件的配置项内容
     */
    static std::string GetJsonConfig(const std::string &jsonCfgFilePath);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_HANDLE_CONFIG_H