/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_json/b_json_entity_ext_manage.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BJsonEntityExtManage::BJsonEntityExtManage(Json::Value &obj) : obj_(obj) {}

void BJsonEntityExtManage::SetExtManage(const set<string> &info) const
{
    obj_["extmanage"].clear();

    for (auto &&fileName : info) {
        obj_["extmanage"].append(fileName);
    }
}

set<string> BJsonEntityExtManage::GetExtManage() const
{
    if (!obj_) {
        HILOGE("Uninitialized JSon Object reference");
        return {};
    }
    if (!obj_.isMember("extmanage")) {
        HILOGE("'extmanage' field not found");
        return {};
    }
    if (!obj_["extmanage"].isArray()) {
        HILOGE("'extmanage' field must be an array");
        return {};
    }

    set<string> info;
    for (auto &&item : obj_["extmanage"]) {
        if (!item.isString()) {
            HILOGE("Each item of array 'extmanage' must be of the type string");
            return {};
        }
        info.insert(item.asString());
    }
    return info;
}
} // namespace OHOS::FileManagement::Backup