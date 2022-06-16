/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H
#define OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
class BMultiuser {
public:
    struct UidReadOut {
        int userId;
        int appId;
    };

public:
    static UidReadOut ParseUid(int uid)
    {
        return UidReadOut {
            .userId = uid / BConstants::SPAN_USERID_UID,
            .appId = uid % BConstants::SPAN_USERID_UID,
        };
    }
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_MULTIUSER_H