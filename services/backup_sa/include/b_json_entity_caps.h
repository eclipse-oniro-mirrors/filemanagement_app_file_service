/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H

#include "b_json/b_json_cached_entity.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class BJsonEntityCaps {
public:
    BJsonEntityCaps(Json::Value &obj) : obj_(obj)
    {
        SetFreeDiskSpace(GetFreeDiskSpace());
    }

    uint64_t GetFreeDiskSpace()
    {
        if (!obj_ || obj_.isMember("freeDiskSpace")) {
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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_CAPS_H