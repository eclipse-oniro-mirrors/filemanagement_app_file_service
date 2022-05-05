/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_SESSION_BACKUP_H
#define OHOS_FILEMGMT_BACKUP_B_SESSION_BACKUP_H

#include <functional>
#include <memory>
#include <vector>

#include "b_file_info.h"
#include "errors.h"
#include "unique_fd.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class BSessionBackup {
public:
    /**
     * @brief 获取一个用于控制备份流程的会话
     *
     * @param remoteCap 已打开的保存远端设备能力的Json文件。可使用BRestoreSession的GetLocalCaablities方法获取
     * @param appsToBackup 待备份应用清单
     * @return std::unique_ptr<BSessionBackup> 指向会话的智能指针。失败时为空指针
     */
    static std::unique_ptr<BSessionBackup> Init(UniqueFd remoteCap, std::vector<AppId> appsToBackup);
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_SESSION_BACKUP_H