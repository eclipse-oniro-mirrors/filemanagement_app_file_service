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
     */
    static void ExcuteCmd(std::vector<const char *> argv);

private:
    BProcess() = delete;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_PROCESS_H