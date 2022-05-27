/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <gtest/gtest.h>

#include "b_process/b_process.h"

namespace OHOS::FileManagement::Backup {
class ToolsTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_help_0100
 * @tc.name: tools_help_0100
 * @tc.desc: Test function of help interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tools_help_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tools_help_0100";
    try {
        BProcess::ExcuteCmd({
            "/system/bin/backup_tool",
            "help",
        });
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tools_help_0100";
}

/**
 * @tc.number: SUB_backup_tools_backup_0100
 * @tc.name: tools_backup_0100
 * @tc.desc: Test function of backup interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tools_backup_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tools_backup_0100";
    try {
        BProcess::ExcuteCmd({
            "rm",
            "/data/test/backup",
            "-rf",
        });
        BProcess::ExcuteCmd({
            "mkdir",
            "/data/test/backup",
        });
        BProcess::ExcuteCmd({
            "backup_tool",
            "restore",
            "/data/test/backup/tmp",
            "1",
        });
        BProcess::ExcuteCmd({
            "backup_tool",
            "backup",
            "/data/test/backup/tmp",
            "1",
        });
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tools_backup_0100";
}
} // namespace OHOS::FileManagement::Backup