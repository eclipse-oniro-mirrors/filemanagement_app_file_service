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

#include "b_json/b_json_entity_usr_config.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonEntityUsrConfig::BJsonEntityUsrConfig(Json::Value &obj) : obj_(obj) {}

vector<string> BJsonEntityUsrConfig::GetIncludes() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_.isMember("includes")) {
        HILOGE("'includes' field not found");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_["includes"].isArray()) {
        HILOGE("'includes' field must be an array");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }

    vector<string> dirs;
    for (auto &&item : obj_["includes"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'includes' must be of the type string");
            continue;
        }
        dirs.push_back(item.asString());
    }

    if (dirs.empty()) {
        dirs.emplace_back("");
    }
    return dirs;
}

vector<string> BJsonEntityUsrConfig::GetExcludes() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isMember("excludes")) {
        HILOGE("'excludes' field not found");
        return {};
    }
    if (!obj_["excludes"].isArray()) {
        HILOGE("'excludes' field must be an array");
        return {};
    }

    vector<string> dirs;
    for (auto &&item : obj_["excludes"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'excludes' must be of the type string");
            return {};
        }
        dirs.push_back(item.asString());
    }
    return dirs;
}

bool BJsonEntityUsrConfig::GetAllowToBackupRestore() const
{
    if (!obj_ || !obj_.isMember("allowToBackupRestore") || !obj_["allowToBackupRestore"].isBool()) {
        HILOGE("Failed to init field allowToBackupRestore");
        return false;
    }

    return obj_["allowToBackupRestore"].asBool();
}
} // namespace OHOS::FileManagement::Backup