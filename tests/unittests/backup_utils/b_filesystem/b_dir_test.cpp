/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <gtest/gtest.h>

#include "b_filesystem/b_dir.h"
#include "file_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BDirTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_dir_GetDirFiles_0100
 * @tc.name: b_dir_GetDirFiles_0100
 * @tc.desc: Test function of GetDirFiles interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H09N0
 */
HWTEST_F(BDirTest, b_dir_GetDirFiles_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BDirTest-begin b_dir_GetDirFiles_0100";
    try {
        TestManager tm("b_dir_GetDirFiles_0100");

        string preparedDir = tm.GetRootDirCurTest();
        string cmdMkdir = string("mkdir ") + preparedDir;
        system(cmdMkdir.c_str());

        string touchFilePrefix = string("touch ") + preparedDir;
        system(touchFilePrefix.append("a.txt").c_str());
        system(touchFilePrefix.append("b.txt").c_str());
        system(touchFilePrefix.append("c.txt").c_str());

        bool bSucc;
        vector<string> out;
        tie(bSucc, out) = BDir::GetDirFiles(preparedDir);

        vector<string> expectedRes = {preparedDir.append("a.txt"), preparedDir.append("b.txt"),
                                      preparedDir.append("c.txt")};
        EXPECT_EQ(out, expectedRes);

        tie(bSucc, out) = BDir::GetDirFiles("dev");
        EXPECT_EQ(bSucc, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BDirTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BDirTest-end b_dir_GetDirFiles_0100";
}
} // namespace OHOS::FileManagement::Backup