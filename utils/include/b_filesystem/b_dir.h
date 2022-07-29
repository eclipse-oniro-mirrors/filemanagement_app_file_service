/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_DIR_H
#define OHOS_FILEMGMT_BACKUP_B_DIR_H

#include <linux/stat.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "errors.h"

namespace OHOS::FileManagement::Backup {
class BDir {
public:
    /**
     * @brief 读取指定目录下所有文件(非递归)
     *
     * @param 目录
     * @return 错误码、文件名数组
     */
    static std::tuple<ErrCode, std::vector<std::string>> GetDirFiles(const std::string &path);

    /**
     * @brief 从给定的includes和excludes目录及文件中获取所有有用大文件和其链接文件的集合
     *
     * @param includes 需要包含的文件及目录集合
     * @param excludes 需要排除的文件及目录集合
     * @return 错误码、大文件名集合
     */
    static std::pair<ErrCode, std::map<std::string, struct stat>> GetBigFiles(const std::vector<std::string> &includes,
                                                                              const std::vector<std::string> &excludes);

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_DIR_H