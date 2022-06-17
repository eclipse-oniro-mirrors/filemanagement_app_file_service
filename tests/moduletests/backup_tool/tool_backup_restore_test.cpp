/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_process/b_process.h"
#include "test_manager.h"
#include "gtest/gtest.h"

namespace OHOS::FileManagement::Backup {
class ToolsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tool_backup_restore_0100
 * @tc.name: tool_backup_restore_0100
 * @tc.desc: 测试backup/restore命令常规用法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_backup_restore_0100, testing::ext::TestSize.Level0)
{
    TestManager tm(__func__);
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_backup_restore_0100";
    try {
        std::string path = tm.GetRootDirCurTest();
        std::string filePath = path + "tmp";

        int ret = BProcess::ExecuteCmd({
            "backup_tool",
            "restore",
            filePath.data(),
            "com.ohos.settings",
        });
        EXPECT_EQ(ret, 0);
        ret = BProcess::ExecuteCmd({
            "backup_tool",
            "backup",
            filePath.data(),
            "com.ohos.settings",
        });
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_backup_restore_0100";
}
} // namespace OHOS::FileManagement::Backup