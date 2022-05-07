/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_H

#include <memory>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BFile {
public:
    /**
     * @brief 一次性读取文件全部内容
     *
     * @param fd 文件描述符
     * @return std::unique_ptr<char[]> 文件全部内容，保证最后一个字节为'\0'
     * @throw std::system_error IO异常
     */
    static std::unique_ptr<char[]> ReadFile(const UniqueFd &fd);

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_H