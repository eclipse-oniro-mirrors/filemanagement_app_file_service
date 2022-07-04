/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_json/b_json_entity_usr_config.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonEntityUsrConfig::BJsonEntityUsrConfig(Json::Value &obj) : obj_(obj) {}

vector<string> BJsonEntityUsrConfig::GetIncludeDirs() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_.isMember("includeDirs")) {
        HILOGE("'includeDirs' field not found");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }
    if (!obj_["includeDirs"].isArray()) {
        HILOGE("'includeDirs' field must be an array");
        return {BConstants::PATHES_TO_BACKUP.begin(), BConstants::PATHES_TO_BACKUP.end()};
    }

    vector<string> dirs;
    for (auto &&item : obj_["includeDirs"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'includeDirs' must be of the type string");
            continue;
        }
        dirs.push_back(item.asString());
    }

    if (dirs.empty()) {
        dirs.emplace_back("");
    }
    return dirs;
}

vector<string> BJsonEntityUsrConfig::GetExcludeDirs() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isMember("excludeDirs")) {
        HILOGE("'excludeDirs' field not found");
        return {};
    }
    if (!obj_["excludeDirs"].isArray()) {
        HILOGE("'excludeDirs' field must be an array");
        return {};
    }

    vector<string> dirs;
    for (auto &&item : obj_["excludeDirs"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'excludeDirs' must be of the type string");
            return {};
        }
        dirs.push_back(item.asString());
    }
    return dirs;
}

bool BJsonEntityUsrConfig::GetAllowToBackup() const
{
    if (!obj_ || !obj_.isMember("allowToBackup") || !obj_["allowToBackup"].isBool()) {
        HILOGE("Failed to init field allowToBackup");
        return false;
    }

    return obj_["allowToBackup"].asBool();
}
} // namespace OHOS::FileManagement::Backup