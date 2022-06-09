/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <iostream>

#include "directory_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

TestManager::TestManager(std::string functionName)
{
    rootDirCurTest_ = "/data/test/backup/" + functionName + "/";
    if (bool created = ForceCreateDirectory(rootDirCurTest_); !created) {
        throw std::system_error(errno, std::system_category());
    }
}

TestManager::~TestManager()
{
    ForceRemoveDirectory(rootDirCurTest_);
}

string TestManager::GetRootDirCurTest() const
{
    return rootDirCurTest_;
}
} // namespace OHOS::FileManagement::Backup