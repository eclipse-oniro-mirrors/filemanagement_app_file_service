/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_PROCESS_H
#define OHOS_FILEMGMT_BACKUP_B_PROCESS_H

#include <vector>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BProcess final : protected NoCopyable {
public:
    /**
     * @brief 执行一个命令并同步等待执行结果
     *
     * @param argv 命令参数表
     * 向量第一项是绝对路径表示的命令名
     * 向量后续参数表示相应命令参数
     * 向量最后无需追加nullptr
     *
     * @return 命令执行结果
     *
     * @throw BError(UTILS_INVAL_PROCESS_ARG) 系统调用异常(子进程启动失败、waitpid调用失败)
     *
     * @throw BError(UTILS_INTERRUPTED_PROCESS) 系统调用异常(pipe调用失败、dup2调用失败、子进程被信号终止)
     */
    static int ExecuteCmd(std::vector<const char *> argv);

private:
    BProcess() = delete;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_PROCESS_H