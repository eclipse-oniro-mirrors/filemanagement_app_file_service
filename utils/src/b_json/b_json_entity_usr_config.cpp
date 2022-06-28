/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_json/b_json_entity_usr_config.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonEntityUsrConfig::BJsonEntityUsrConfig(Json::Value &obj) : obj_(obj) {}

vector<string> BJsonEntityUsrConfig::GetIncludeDirs()
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {""};
    }
    if (!obj_.isMember("includeDirs")) {
        HILOGE("'includeDirs' field not found");
        return {""};
    }
    if (!obj_["includeDirs"].isArray()) {
        HILOGE("'includeDirs' field must be an array");
        return {""};
    }

    vector<string> dirs;
    for (auto &&item : obj_["includeDirs"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'includeDirs' must be of the type string");
            return {""};
        }
        dirs.push_back(item.asString());
    }
    return dirs;
}

vector<string> BJsonEntityUsrConfig::GetExcludeDirs()
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
} // namespace OHOS::FileManagement::Backup