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
#include "svc_death_recipient.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BSessionRestore {
public:
    struct Callbacks {
        std::function<void(const BFileInfo &, UniqueFd)> onFileReady;    // 当备份服务有文件待发送时执行的回调
        std::function<void(ErrCode, const BundleName)> onBundleStarted;  // 当启动某个应用的恢复流程结束时执行的回调函数
        std::function<void(ErrCode, const BundleName)> onBundleFinished; // 当某个应用的恢复流程结束或意外中止时执行的回调函数
        std::function<void(ErrCode)> onAllBundlesFinished;               // 当整个恢复流程结束或意外中止时执行的回调函数
        std::function<void()> onBackupServiceDied;                       // 当备份服务意外死亡时执行的回调函数
    };

public:
    /**
     * @brief 获取一个用于控制恢复流程的会话
     *
     * @param bundlesToRestore 待恢复应用清单
     * @param callbacks 注册的回调函数
     * @return std::unique_ptr<BRestoreSession> 指向BRestoreSession的智能指针。失败时为空指针
     */
    static std::unique_ptr<BSessionRestore> Init(std::vector<BundleName> bundlesToRestore, Callbacks callbacks);

    /**
     * @brief 获取用于描述本端能力的Json文件
     *
     * @return UniqueFd 文件描述符
     */
    UniqueFd GetLocalCapabilities();

    /**
     * @brief 通知备份服务文件内容已就绪
     *
     * @param fileInfo 文件描述信息
     * @return ErrCode 规范错误码
     * @see GetExtFileName
     */
    ErrCode PublishFile(BFileInfo fileInfo);

    /**
     * @brief 请求恢复流程所需的真实文件
     *
     * @param bundleName 应用名称
     * @param fileName   文件名称
     */
    ErrCode GetExtFileName(std::string &bundleName, std::string &fileName);

    /**
     * @brief 用于启动恢复流程
     *
     * @return ErrCode 规范错误码
     */
    ErrCode Start();

    /**
     * @brief 注册备份服务意外死亡时执行的回调函数
     *
     * @param functor 回调函数
     */
    void RegisterBackupServiceDied(std::function<void()> functor);

public:
    ~BSessionRestore();

private:
    sptr<SvcDeathRecipient> deathRecipient_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_SESSION_RESTORE_H