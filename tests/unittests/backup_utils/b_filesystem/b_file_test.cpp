/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <tuple>

#include "file_ex.h"
#include "test_manager.h"
#include "b_filesystem/b_file.h"

namespace OHOS::FileManagement::Backup {
class BFileTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @brief 创建测试文件
 *
 * @return std::tuple<bool, string, string> 创建结果、文件路径、文件内容
 */
std::tuple<std::string, std::string> GetTestFile(const TestManager &tm)
{
    std::string path = tm.GetRootDirCurTest();
    std::string filePath = path + "temp.txt";
    std::string content = "backup test";
    if (bool contentCreate = SaveStringToFile(filePath, content, true); !contentCreate) {
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
 * @tc.require: SR000H037V
 */
HWTEST_F(BFileTest, b_file_ReadFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BFileTest-begin b_file_ReadFile_0100";
    try {
        TestManager tm(__func__);
        const auto [filePath, content] = GetTestFile(tm);
        BFile bf;
        std::unique_ptr<char[]> result = bf.ReadFile(UniqueFd(open(filePath.data(), O_RDWR)));
        std::string readContent(result.get());
        EXPECT_EQ(readContent.compare(content), 0);
    } catch (const std::exception &e) {
        GTEST_LOG_(INFO) << "BFileTest-an exception occurred by ReadFile.";
        e.what();
    }
    GTEST_LOG_(INFO) << "BFileTest-end b_file_ReadFile_0100";
}
} // namespace OHOS::FileManagement::Backup