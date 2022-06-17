/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <functional>
#include <string>

#include "b_process/b_process.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "test_manager.h"
#include "gtest/gtest.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class ToolsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

string DIR_TO_TAR = "dir2tar";
string DIR_TO_UNTAR = "dir2untar";

void PrepareSmallDataSet(const TestManager &tm)
{
    string root = tm.GetRootDirCurTest();
    if (!ForceCreateDirectory(root + DIR_TO_TAR + "/subdir1/sub-subdir") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir1/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir1/sub-subdir/file.txt") ||
        !ForceCreateDirectory(root + DIR_TO_TAR + "/subdir2/sub-subdir") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir2/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir2/sub-subdir/file.txt") ||
        !ForceCreateDirectory(root + DIR_TO_TAR + "/subdir3/sub-subdir") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir3/file.txt") ||
        !SaveStringToFile(to_string(__LINE__), root + DIR_TO_TAR + "/subdir3/sub-subdir/file.txt") ||
        !ForceCreateDirectory(root + DIR_TO_UNTAR)) {
        throw std::system_error(errno, std::system_category());
    }
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0100
 * @tc.name: tool_simulate_app_0100
 * @tc.desc: 测试simulate_app正常功能
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0100";
    try {
        TestManager tm("tool_simulate_app_0100");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, "{}");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret = BProcess::ExecuteCmd(
            {"backup_tool", "simulate", "app", "tar", pathConfigFile.data(), pathTarFile.data(), pathTaredDir.data()});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile.data(), pathUntarDir.data()});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir.data(), pathUntarDir.data()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0100";
}
} // namespace OHOS::FileManagement::Backup