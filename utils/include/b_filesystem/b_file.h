/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_H

#include <memory>
#include <string>

#include "unique_fd.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
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

    /**
     * @brief linux sendfile 二次封装
     * @param out_fd 参数是待写入内容的文件描述符
     * @param in_fd 参数是待读出内容的文件描述符
     * @throw std::system_error IO异常
     */
    static void SendFile(int out_fd, int in_fd);

    /**
     * @brief linux write 二次封装
     * @param fd 参数是待写入内容的文件描述符
     * @param str 待写入文件的字符串
     * @throw std::system_error IO异常
     */
    static void Write(const UniqueFd &fd, const string &str);

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_H