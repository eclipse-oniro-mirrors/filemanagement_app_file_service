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
        !SaveStringToFile(root + DIR_TO_TAR + "/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir1/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir1/sub-subdir/file.txt", to_string(__LINE__)) ||
        !ForceCreateDirectory(root + DIR_TO_TAR + "/subdir2/sub-subdir") ||
        !SaveStringToFile(root + DIR_TO_TAR + "/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir2/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir2/sub-subdir/file.txt", to_string(__LINE__)) ||
        !ForceCreateDirectory(root + DIR_TO_TAR + "/subdir3/sub-subdir") ||
        !SaveStringToFile(root + DIR_TO_TAR + "/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir3/file.txt", to_string(__LINE__)) ||
        !SaveStringToFile(root + DIR_TO_TAR + "/subdir3/sub-subdir/file.txt", to_string(__LINE__)) ||
        !ForceCreateDirectory(root + DIR_TO_UNTAR)) {
        throw std::system_error(errno, std::system_category());
    }
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0100
 * @tc.name: tool_simulate_app_0100
 * @tc.desc: 不包含includes和excludes
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
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0100";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0110
 * @tc.name: tool_simulate_app_0110
 * @tc.desc: json文件中includes标签参数为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0110, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0110";
    try {
        TestManager tm("tool_simulate_app_0110");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["", "", ""]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0110";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0120
 * @tc.name: tool_simulate_app_0120
 * @tc.desc: json文件中excludes标签参数为空
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0120, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0120";
    try {
        TestManager tm("tool_simulate_app_0120");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":["", "", ""]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0120";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0130
 * @tc.name: tool_simulate_app_0130
 * @tc.desc: json文件中includes标签无参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0130, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0130";
    try {
        TestManager tm("tool_simulate_app_0130");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includeDir":[]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0130";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0140
 * @tc.name: tool_simulate_app_0140
 * @tc.desc: json文件中excludes标签无参数
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0140, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0140";
    try {
        TestManager tm("tool_simulate_app_0140");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":[]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0140";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0150
 * @tc.name: tool_simulate_app_0150
 * @tc.desc: json文件中无标签
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0150, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0150";
    try {
        TestManager tm("tool_simulate_app_0150");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"":["subdir1", "subdir2", "subdir3"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0150";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0160
 * @tc.name: tool_simulate_app_0160
 * @tc.desc: json文件中includes标签参数为特殊字符
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0160, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0160";
    try {
        TestManager tm("tool_simulate_app_0160");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["123#$%^&*", "4565%#￥%"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0160";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0170
 * @tc.name: tool_simulate_app_0170
 * @tc.desc: json文件中excludes标签参数为特殊字符
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0170, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0170";
    try {
        TestManager tm("tool_simulate_app_0170");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":["123#$%^&*"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0170";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0180
 * @tc.name: tool_simulate_app_0180
 * @tc.desc: json文件中includes标签参数为中文汉字
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0180, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0180";
    try {
        TestManager tm("tool_simulate_app_0180");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["测试代码", "备份恢复测试"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0180";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0190
 * @tc.name: tool_simulate_app_0190
 * @tc.desc: json文件中excludes标签参数为中文汉字
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0190, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0190";
    try {
        TestManager tm("tool_simulate_app_0190");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":["测试代码", "备份恢复测试"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0190";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0200
 * @tc.name: tool_simulate_app_0200
 * @tc.desc: json文件中标签为中文汉字
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0200";
    try {
        TestManager tm("tool_simulate_app_0200");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"测试代码":["subdir1", "subdir2", "subdir3"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0200";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0210
 * @tc.name: tool_simulate_app_0210
 * @tc.desc: json文件标签中有异常括号
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0210, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0210";
    try {
        TestManager tm("tool_simulate_app_0210");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(
            pathConfigFile,
            R"({"includes":["subdir1", "subdir2", "subdir3"], "excludes":["subdir1/file.txt", "subdir2/*.md"], "includes":[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0210";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0220
 * @tc.name: tool_simulate_app_0220
 * @tc.desc: json文件内容为空，路径为多层目录下
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0220, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0220";
    try {
        TestManager tm("tool_simulate_app_0220");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, "{}");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0220";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0230
 * @tc.name: tool_simulate_app_0230
 * @tc.desc: json文件excludes标签中包含多层目录
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0230, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0230";
    try {
        TestManager tm("tool_simulate_app_0230");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(
            pathConfigFile,
            R"({"excludes":["/data/backup/test/test1/test2/test3/test4/test5/test6/test7/test8/test9/test10/test11/test12/test13/test14/test15/test16/test17/test18/test19/test20/test21/test22/test23/test24/test25/test26/test27/test28/test29/test30/test31/test32/test33/test34/test35/test36/test37/test38/test39/test40/test41/test42/test43/test44/test45/test46/test47/test48/test49/test50/test51/test52/test53/test54/test55/test56/test57/test58/test59/test60/test61/test62/test63/test64/test65/test66/test67/test68/test69/test70/test71/test72/test73/test74/test75/test76/test77/test78/test79/test80/test81/test82/test83/test84/test85/test86/test87/test88/test89/test90/test91/test92/test93/test94/test95/test96/test97/test98/test99/test100/"]})");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret =
            BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0230";
}

/**
 * @tc.number: SUB_backup_tool_simulate_app_0240
 * @tc.name: tool_simulate_app_0240
 * @tc.desc: 备份恢复所有文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(ToolsTest, tool_simulate_app_0240, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "ToolsTest-begin tool_simulate_app_0240";
    try {
        TestManager tm("tool_simulate_app_0240");
        PrepareSmallDataSet(tm);

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, "{}");

        string pathTarFile = root + "pkg.tar";
        string pathTaredDir = root + DIR_TO_TAR;
        int ret;
        ret = BProcess::ExecuteCmd(
            {"time", "backup_tool", "simulate", "app", "tar", pathConfigFile, pathTarFile, pathTaredDir});
        EXPECT_EQ(ret, 0);

        string pathUntarDir = root + DIR_TO_UNTAR;
        ret = BProcess::ExecuteCmd({"time", "backup_tool", "simulate", "app", "untar", pathTarFile, pathUntarDir});
        EXPECT_EQ(ret, 0);

        ret = BProcess::ExecuteCmd({"diff", pathTaredDir, pathUntarDir});
        EXPECT_EQ(ret, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(ERROR) << "ToolsTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "ToolsTest-end tool_simulate_app_0240";
}
} // namespace OHOS::FileManagement::Backup