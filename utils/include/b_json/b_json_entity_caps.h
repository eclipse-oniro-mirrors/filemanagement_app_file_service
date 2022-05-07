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
    };

    void SetFreeDiskSpace(uint64_t FreeDiskSpace)
    {
        obj_["freeDiskSpace"] = FreeDiskSpace;
    };

private:
    Json::Value &obj_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H