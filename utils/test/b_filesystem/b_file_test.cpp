/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <tuple>

#include "b_filesystem/b_file.h"
#include "b_error/b_error.h"
#include "directory_ex.h"
#include "file_ex.h"

namespace OHOS::FileManagement::Backup {
class BFileTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown()
    {
        ForceRemoveDirectory("/data/test/temp/");
    };
};

/**
 * @brief 创建测试文件
 *
 * @return std::tuple<bool, string, string> 创建结果、文件路径、文件内容
 */
std::tuple<std::string, std::string> GetTestFile()
{
    std::string path = "/data/test/temp/";
    std::string filePath = "/data/test/temp/temp.txt";
    std::string content = "backup test";
    bool pathCreate = ForceCreateDirectory(path);
    bool fileCreate = SaveStringToFile(filePath, content, true);
    if (!pathCreate || !fileCreate) {
        throw std::system_error(errno, std::system_category());
    }
    return {filePath, content};
}

/**
 * @tc.number: SUB_backup_b_file_ReadFile_0100
 * @tc.name: b_file_ReadFile_0100
 * @tc.desc: Test function of ReadFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BFileTest, b_file_ReadFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BFileTest-begin b_file_ReadFile_0100";
    try {
        const auto [filePath, content] = GetTestFile();
        BFile bf;
        std::unique_ptr<char[]> result = bf.ReadFile(UniqueFd(open("/data/test/temp/temp.txt", O_RDWR)));
        std::string readContent(result.get());
        EXPECT_EQ(readContent.compare(content), 0);
    } catch (const std::exception &e) {
        GTEST_LOG_(INFO) << "BFileTest-an exception occurred by ReadFile.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileTest-end b_file_ReadFile_0100";
}
} // namespace OHOS::FileManagement::Backup