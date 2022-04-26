/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_SESSION_RESTORE_H
#define OHOS_FILEMGMT_BACKUP_B_SESSION_RESTORE_H

#include <functional>
#include <memory>
#include <vector>

#include "b_file_info.h"
#include "errors.h"
#include "unique_fd.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class BSessionRestore {
public:
    /**
     * @brief 获取一个用于控制恢复流程的会话
     *
     * @param appsToRestore 待恢复应用清单
     * @param callbacks 注册的回调函数
     * @return std::unique_ptr<BRestoreSession> 指向BRestoreSession的智能指针。失败时为空指针
     */
    static std::unique_ptr<BSessionRestore> Init(std::vector<AppId> appsToRestore);

    /**
     * @brief 获取用于描述本端能力的Json文件
     *
     * @return UniqueFd 文件描述符
     */
    UniqueFd GetLocalCapabilities();
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_SESSION_RESTORE_H