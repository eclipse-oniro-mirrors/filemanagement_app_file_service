/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_DIR_H
#define OHOS_FILEMGMT_BACKUP_B_DIR_H

#include <string>
#include <tuple>
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

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_DIR_H