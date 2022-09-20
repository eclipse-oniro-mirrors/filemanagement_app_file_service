/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <gtest/gtest.h>
#include <string_view>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "b_error.h"
#include "b_tarball_cmdline.h"
#include "file_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTarballCmdlineTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_b_tarball_cmdline_0100
 * @tc.name: b_tarball_cmdline_0100
 * @tc.desc: 测试BTarballCmdline类构造函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0379
 */
HWTEST_F(BTarballCmdlineTest, b_tarball_cmdline_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-begin b_tarball_cmdline_0100";
    try {
        TestManager tm("b_tarball_cmdline_0100");
        string root = tm.GetRootDirCurTest();
        string_view tarballDir = root;
        string_view tarballName = "test.tar";

        BTarballCmdline tarballCmdline(tarballDir, tarballName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballCmdlineTest-an exception occurred by BTarballCmdline.";
    }
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-end b_tarball_cmdline_0100";
}

/**
 * @tc.number: SUB_b_tarball_cmdline_0100
 * @tc.name: b_tarball_cmdline_0100
 * @tc.desc: 测试BTarballCmdline类Tar函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0379
 */
HWTEST_F(BTarballCmdlineTest, b_tarball_cmdline_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-begin b_tarball_cmdline_0200";
    try {
        // 预置文件和目录
        TestManager tm("b_tarball_cmdline_0200");
        string root = tm.GetRootDirCurTest();
        string_view tarballDir = root;
        string_view tarballName = "test.tar";
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "/a.txt";
        string bFile = testDir + "/b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");
        vector<string_view> includes {testDir};
        vector<string_view> excludes {bFile};

        // 调用tar打包
        BTarballCmdline tarballCmdline(tarballDir, tarballName);
        tarballCmdline.Tar(root, includes, excludes);

        // 判断是否生成了打包文件
        if (access((root + "/" + string(tarballName)).data(), F_OK) != 0) {
            EXPECT_TRUE(false);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballCmdlineTest-an exception occurred by BTarballCmdline.";
    }
    GTEST_LOG_(INFO) << "BTarballCmdlineTest-end b_tarball_cmdline_0200";
}
} // namespace OHOS::FileManagement::Backup