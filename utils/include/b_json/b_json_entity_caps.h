/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H

#include "b_json/b_json_cached_entity.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
class BJsonEntityCaps {
public:
    BJsonEntityCaps(Json::Value &obj) : obj_(obj)
    {
        SetFreeDiskSpace(GetFreeDiskSpace());
    }

    uint64_t GetFreeDiskSpace()
    {
        if (!obj_ || !obj_.isMember("freeDiskSpace") || !obj_["freeDiskSpace"].isUInt64()) {
            HILOGE("Failed to init field FreeDiskSpace");
            return 0;
        }

        return obj_["freeDiskSpace"].asUInt64();
    }

    void SetFreeDiskSpace(uint64_t FreeDiskSpace)
    {
        obj_["freeDiskSpace"] = FreeDiskSpace;
    }

    void SetOSFullName(std::string osFullName)
    {
        obj_["OSFullName"] = osFullName;
    }

    void SetDeviceType(std::string deviceType)
    {
        obj_["deviceType"] = deviceType;
    }

    std::string GetOSFullName()
    {
        if (!obj_ || !obj_.isMember("OSFullName") || !obj_["OSFullName"].isString()) {
            HILOGE("Failed to get field OSFullName");
            return "";
        }

        return obj_["OSFullName"].asString();
    }

    std::string GetDeviceType()
    {
        if (!obj_ || !obj_.isMember("deviceType") || !obj_["deviceType"].isString()) {
            HILOGE("Failed to get field deviceType");
            return "";
        }

        return obj_["deviceType"].asString();
    }

private:
    Json::Value &obj_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H