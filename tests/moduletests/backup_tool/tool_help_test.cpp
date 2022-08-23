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
 * @tc.number: SUB_backup_tool_help_0100
 * @tc.name: tool_help_0100
 * @tc.desc: 测试help命令常规用法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0377
 */
HWTEST_F(ToolsTest, tool_help_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_help_0100";
    try {
        auto [bFatalError, ret] = BProcess::ExecuteCmd({
            "/system/bin/backup_tool",
            "help",
        });
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_help_0100";
}
} // namespace OHOS::FileManagement::Backup