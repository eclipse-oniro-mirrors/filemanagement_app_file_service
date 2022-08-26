/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdlib>
#include <gtest/gtest.h>
#include <string>

#include "b_process/b_process.h"
#include "b_tarball/b_tarball_posix/b_tarball_posix_tarball.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTarballPosixTarballTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_EmptyFile_0100
 * @tc.name: b_tarball_posix_tarball_EmptyFile_0100
 * @tc.desc: 测试BTarballPosixTarball类打包空普通文件是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_EmptyFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_EmptyFile_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_EmptyFile_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 创建空文件
        string fileName = "empty.file";
        string filePath = backupRootDirPath + fileName;
        auto [bFatalError, ret] = BProcess::ExecuteCmd({"touch", filePath.c_str()});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对空文件进行打包
        string tarballName = "empty.file.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(filePath);
        tarball.Publish();
        // 对包文件进行解包
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 比较源文件和解包文件的差异
        string unpackFilePath = unpackDirPath + filePath;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"diff", filePath.c_str(), unpackFilePath.c_str()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_EmptyFile_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_RegularFile_0100
 * @tc.name: b_tarball_posix_tarball_RegularFile_0100
 * @tc.desc: 测试BTarballPosixTarball类打包含数据的普通文件是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_RegularFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_RegularFile_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_RegularFile_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 创建普通文件（dd命令中bs设为1000是为了构造文件字节数不是512的整数倍的场景）
        string fileName = "regular.file";
        string filePath = backupRootDirPath + fileName;
        auto [bFatalError, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath).c_str(), "bs=1000", "count=1"});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对普通文件进行打包
        string tarballName = "regular.file.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(filePath);
        tarball.Publish();
        // 对包文件进行解包
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 比较源文件和解包文件的差异
        string unpackFilePath = unpackDirPath + filePath;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"diff", filePath.c_str(), unpackFilePath.c_str()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_RegularFile_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_EmptyDir_0100
 * @tc.name: b_tarball_posix_tarball_EmptyDir_0100
 * @tc.desc: 测试BTarballPosixTarball类打包空目录是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_EmptyDir_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_EmptyDir_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_EmptyDir_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 创建空目录
        string fileName = "empty.dir";
        string filePath = backupRootDirPath + fileName;
        auto [bFatalError, ret] = BProcess::ExecuteCmd({"mkdir", filePath.c_str()});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对空目录进行打包
        string tarballName = "empty.dir.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(filePath);
        tarball.Publish();
        // 对包文件进行解包
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 比较源文件和解包文件的差异
        string unpackFilePath = unpackDirPath + filePath;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"diff", filePath.c_str(), unpackFilePath.c_str()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_EmptyDir_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_SymbolicLink_0100
 * @tc.name: b_tarball_posix_tarball_SymbolicLink_0100
 * @tc.desc: 测试BTarballPosixTarball类打包软链接是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_SymbolicLink_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_SymbolicLink_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_SymbolicLink_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 创建普通文件（dd命令中bs设为1000是为了构造文件字节数不是512的整数倍的场景）
        string fileName = "regular.file";
        string filePath = backupRootDirPath + fileName;
        auto [bFatalError, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath).c_str(), "bs=1000", "count=1"});
        EXPECT_EQ(ret, 0);
        // 创建软链接
        string linkName = "symbolic.link";
        string linkPath = backupRootDirPath + linkName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"ln", "-s", filePath.c_str(), linkPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对普通文件进行打包
        string tarballName = "symbolic.link.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(linkPath);
        tarball.Publish();
        // 对包文件进行解包
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 比较源文件和解包文件的差异
        string unpackFilePath = unpackDirPath + linkPath;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"diff", linkPath.c_str(), unpackFilePath.c_str()});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_SymbolicLink_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_SuperLongPathFile_0100
 * @tc.name: b_tarball_posix_tarball_SuperLongPathFile_0100
 * @tc.desc:
 * 测试BTarballPosixTarball类打包超长路径名文件，由于tar工具采用GNU格式，而自研打包工具采用POSIX格式，故解包必定失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_SuperLongPathFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_SuperLongPathFile_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_SuperLongPathFile_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 文件路径长度超过99字节就属于长文件名文件
        string fileName =
            "superLongPathSuperLongPathSuperLongPathSuperLongPathSuperLongPathSuperLongPathSuperLongPathSuperLongPath."
            "file";
        string filePath = backupRootDirPath + fileName;
        // 创建超长路径名文件（dd命令中bs设为1000是为了构造文件字节数不是512的整数倍的场景）
        auto [bFatalError, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath).c_str(), "bs=1000", "count=1"});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对超长文件名文件进行打包
        string tarballName = "superLongPath.file.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(filePath);
        tarball.Publish();
        // 对包文件进行解包（由于tar工具不支持typeFlag='x'的场景，故解包会失败）
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_NE(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_SuperLongPathFile_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_SuperLongSymbolicLink_0100
 * @tc.name: b_tarball_posix_tarball_SuperLongSymbolicLink_0100
 * @tc.desc:
 * 测试BTarballPosixTarball类打包超长链接路径名软链接，由于tar工具采用GNU格式，而自研打包工具采用POSIX格式，故解包必定失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_SuperLongSymbolicLink_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_SuperLongSymbolicLink_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_SuperLongSymbolicLink_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 链接路径长度超过99字节就属于超长链接路径
        string fileName =
            "superLongLinkSuperLongLinkSuperLongLinkSuperLongLinkSuperLongLinkSuperLongLinkSuperLongLinkSuperLongLink."
            "file";
        string filePath = backupRootDirPath + fileName;
        // 创建超长路径名文件（dd命令中bs设为1000是为了构造文件字节数不是512的整数倍的场景）
        auto [bFatalError, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath).c_str(), "bs=1000", "count=1"});
        EXPECT_EQ(ret, 0);
        // 创建超长链接路径软链接
        string linkName = "superLongSymbolic.link";
        string linkPath = backupRootDirPath + linkName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"ln", "-s", filePath.c_str(), linkPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对超长链接路径软链接进行打包
        string tarballName = "superLongSymbolic.link.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(linkPath);
        tarball.Publish();
        // 对包文件进行解包（由于tar工具不支持typeFlag='x'的场景，故解包会失败）
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_NE(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_SuperLongSymbolicLink_0100";
}

/**
 * @tc.number: SUB_backup_b_tarball_posix_tarball_SuperLargeFile_0100
 * @tc.name: b_tarball_posix_tarball_SuperLargeFile_0100
 * @tc.desc: 测试BTarballPosixTarball类打包超大文件，由于tar工具采用GNU格式，而自研打包工具采用POSIX格式，故解包必定失败
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(BTarballPosixTarballTest, b_tarball_posix_tarball_SuperLargeFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-begin b_tarball_posix_tarball_SuperLargeFile_0100";
    try {
        TestManager testManager("b_tarball_posix_tarball_SuperLargeFile_0100");
        string backupRootDirPath = testManager.GetRootDirCurTest();
        // 创建超大文件（dd命令中count设为8200是为了构造文件字节数大于8589934591字节的场景，文件字节数超过该值属于超大文件）
        string fileName = "superLarge.file";
        string filePath = backupRootDirPath + fileName;
        auto [bFatalError, ret] =
            BProcess::ExecuteCmd({"dd", "if=/dev/urandom", ("of=" + filePath).c_str(), "bs=1M", "count=8200"});
        EXPECT_EQ(ret, 0);
        // 创建用于存放解包文件的目录
        string unpackDirName = "unpack";
        string unpackDirPath = backupRootDirPath + unpackDirName;
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"mkdir", unpackDirPath.c_str()});
        EXPECT_EQ(ret, 0);
        // 对超大文件进行打包
        string tarballName = "superLarge.file.tar";
        string tarballPath = backupRootDirPath + tarballName;
        BTarballPosixTarball tarball(tarballPath);
        tarball.Emplace(filePath);
        tarball.Publish();
        // 对包文件进行解包（由于tar工具不支持typeFlag='x'的场景，故解包会失败）
        tie(bFatalError, ret) = BProcess::ExecuteCmd({"tar", "-xvf", tarballPath.c_str(), "-C", unpackDirPath.c_str()});
        EXPECT_NE(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballPosixTarballTest-an exception occurred by BTarballPosixTarball.";
    }
    GTEST_LOG_(INFO) << "BTarballPosixTarballTest-end b_tarball_posix_tarball_SuperLargeFile_0100";
}
} // namespace OHOS::FileManagement::Backup