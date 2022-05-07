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

namespace OHOS::FileManagement::Backup {
class BSessionRestore {
public:
    struct Callbacks {
        std::function<void(ErrCode, const AppId)> onSubTaskStarted;  // 当启动某个应用的恢复流程结束时执行的回调函数
        std::function<void(ErrCode, const AppId)> onSubTaskFinished; // 当某个应用的恢复流程结束或意外中止时执行的回调函数
        std::function<void(ErrCode)> onTaskFinished;                 // 当整个恢复流程结束或意外中止时执行的回调函数
        std::function<void()> onBackupServiceDied;                   // 当备份服务意外死亡时执行的回调函数
    };

public:
    /**
     * @brief 获取一个用于控制恢复流程的会话
     *
     * @param appsToRestore 待恢复应用清单
     * @param callbacks 注册的回调函数
     * @return std::unique_ptr<BRestoreSession> 指向BRestoreSession的智能指针。失败时为空指针
     */
    static std::unique_ptr<BSessionRestore> Init(std::vector<AppId> appsToRestore, Callbacks callbacks);

    /**
     * @brief 获取用于描述本端能力的Json文件
     *
     * @return UniqueFd 文件描述符
     */
    UniqueFd GetLocalCapabilities();

    /**
     * @brief 获取在备份服务中打开的临时文件，用于接收对端设备发送的文件
     *
     * @return UniqueFd 文件描述符
     * @see PublishFile
     */
    UniqueFd GetFileOnServiceEnd();

    /**
     * @brief 通知备份服务文件内容已就绪
     *
     * @param fileInfo 文件描述信息
     * @param fileHandle 仅具有移动语义的文件描述符
     * @return ErrCode 规范错误码
     * @see GetFileOnServiceEnd
     */
    ErrCode PublishFile(const BFileInfo &fileInfo, UniqueFd fileHandle);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_SESSION_RESTORE_H