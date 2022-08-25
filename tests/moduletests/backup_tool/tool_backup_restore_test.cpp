/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdlib>

#include "b_process/b_process.h"
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

string bundleName = "com.example.app2backup";
string toyBox = "/data/toybox-aarch64";
string signedHapPath = "/data/signed.hap";

/**
 * @tc.number: SUB_backup_tool_backup_restore_0100
 * @tc.name: tool_backup_restore_0100
 * @tc.desc: 测试backup/restore命令常规用法
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0382
 */
HWTEST_F(ToolsTest, tool_backup_restore_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_backup_restore_0100";
    try {
        TestManager testManager("tool_backup_restore_0100");
        string backupRootDir = testManager.GetRootDirCurTest();
        string outputDir = "/data/service/el2/100/backup/bundles/" + bundleName + "/";
        string tmpFilePath = backupRootDir + "tmp";
        string testFileDir = "/data/app/el2/100/base/" + bundleName + "/files/";
        string testFile1Path = testFileDir + "file1";
        string testFile2Path = testFileDir + "file2";
        // 创建测试文件file1和file2，并执行backup命令
        int ret = BProcess::ExecuteCmd({"touch", testFile1Path});
        EXPECT_EQ(ret, 0);
        ret = BProcess::ExecuteCmd({"touch", testFile2Path});
        EXPECT_EQ(ret, 0);
        ret = BProcess::ExecuteCmd({"backup_tool", "backup", "--isLocal=true", "--bundle", bundleName.c_str(),
                                    "--pathCapFile", tmpFilePath.c_str()});
        EXPECT_EQ(ret, 0);

        string allFilesInTestFileDir = testFileDir + "*";
        string beforeBackupDir = backupRootDir + "beforebackup";
        ret = BProcess::ExecuteCmd({"mkdir", "-p", beforeBackupDir.c_str()});
        EXPECT_EQ(ret, 0);
        // 利用toybox执行cp命令
        ret = BProcess::ExecuteCmd({"chmod", "+x", toyBox.c_str()});
        EXPECT_EQ(ret, 0);
        string toyBoxCmdBeforeBackup = toyBox + " cp -r --parents " + allFilesInTestFileDir + " " + beforeBackupDir;
        system(toyBoxCmdBeforeBackup.c_str());

        // 删除beforebackup目录下的file1
        string file1PathInBeforeBackupDir = beforeBackupDir + testFile1Path;
        ret = BProcess::ExecuteCmd({"rm", "-fr", file1PathInBeforeBackupDir.c_str()});
        EXPECT_EQ(ret, 0);

        // 删除应用，重装应用
        ret = BProcess::ExecuteCmd({"bm", "uninstall", "-n", bundleName.c_str()});
        EXPECT_EQ(ret, 0);
        ret = BProcess::ExecuteCmd({"bm", "install", "-p", signedHapPath.c_str()});
        EXPECT_EQ(ret, 0);

        // 执行restore命令
        ret = BProcess::ExecuteCmd(
            {"backup_tool", "restore", "--bundle", bundleName.c_str(), "--pathCapFile", tmpFilePath.c_str()});
        EXPECT_EQ(ret, 0);

        string afterRestoreDir = backupRootDir + "afterrestore";
        ret = BProcess::ExecuteCmd({"mkdir", "-p", afterRestoreDir.c_str()});
        EXPECT_EQ(ret, 0);
        // 利用toybox执行cp命令
        string toyBoxCmdAfterRestore = toyBox + " cp -r --parents " + allFilesInTestFileDir + " " + afterRestoreDir;
        system(toyBoxCmdAfterRestore.c_str());

        // 删除afterrestore目录下的file1
        string file1PathInAfterRestoreDir = afterRestoreDir + testFile1Path;
        ret = BProcess::ExecuteCmd({"rm", "-fr", file1PathInAfterRestoreDir.c_str()});
        EXPECT_EQ(ret, 0);

        // 比较beforebackup目录和afterrestore目录
        ret = BProcess::ExecuteCmd({"diff", "-r", beforeBackupDir.c_str(), afterRestoreDir.c_str()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_backup_restore_0100";
}
} // namespace OHOS::FileManagement::Backup