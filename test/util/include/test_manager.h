/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H

#include <string>

namespace OHOS::FileManagement::Backup {
class TestManager {
public:
    explicit TestManager(std::string functionName);
    ~TestManager();

    /**
     * @brief 创建测试文件根目录
     *
     * @return std::string 返回文件路径名
     */
    std::string GetRootDirCurTest() const;

private:
    std::string rootDirCurTest_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_TEST_MANAGER_H
