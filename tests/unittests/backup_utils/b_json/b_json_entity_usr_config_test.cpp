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
#include <sys/stat.h>
#include <sys/types.h>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_json/b_json_entity_usr_config.h"
#include "directory_ex.h"
#include "file_ex.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BJsonEntityUsrConfigTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

static vector<string> DEFAULT_INCLUDE_DIR = {
    "data/storage/el2/database/",
    "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",
    "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/base/files/",
    "data/storage/el2/base/haps/*/base/preferences/",
};
static vector<string> DEFAULT_EXCLUDE_DIR = {};

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0100
 * @tc.name: b_json_entity_usr_config_0100
 * @tc.desc: 不包含includes和excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0100";
    try {
        TestManager tm("b_json_entity_usr_config_0100");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, "");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0100";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0200
 * @tc.name: b_json_entity_usr_config_0200
 * @tc.desc: json文件中只包含includes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0200";
    try {
        TestManager tm("b_json_entity_usr_config_0200");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {"", "", ""};
        EXPECT_EQ(includes, includesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0200";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0300
 * @tc.name: b_json_entity_usr_config_0300
 * @tc.desc: json文件中只包含excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0300, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0300";
    try {
        TestManager tm("b_json_entity_usr_config_0300");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"excludes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {"", "", ""};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0300";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0400
 * @tc.name: b_json_entity_usr_config_0400
 * @tc.desc: 同时包含includes和excludes
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0400, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0400";
    try {
        TestManager tm("b_json_entity_usr_config_0400");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"includes":["", "", ""], "excludes":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {"", "", ""};
        EXPECT_EQ(includes, includesExpect);
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {"", "", ""};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0400";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0500
 * @tc.name: b_json_entity_usr_config_0500
 * @tc.desc: json文件中标签为特殊字符
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0500, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0500";
    try {
        TestManager tm("b_json_entity_usr_config_0500");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"%#$%445":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        vector<string> includesExpect = {DEFAULT_INCLUDE_DIR};
        EXPECT_EQ(includes, includesExpect);
        vector<string> excludes = cache.GetExcludes();
        vector<string> excludesExpect = {DEFAULT_EXCLUDE_DIR};
        EXPECT_EQ(excludes, excludesExpect);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0500";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0600
 * @tc.name: b_json_entity_usr_config_0600
 * @tc.desc: json文件中标签为中文汉字
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0600, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0600";
    try {
        TestManager tm("b_json_entity_usr_config_0600");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"测试代码":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0600";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0700
 * @tc.name: b_json_entity_usr_config_0700
 * @tc.desc: json文件中无标签
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0700, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0700";
    try {
        TestManager tm("b_json_entity_usr_config_0700");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(pathConfigFile, R"({"":["", "", ""]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0700";
}

/**
 * @tc.number: SUB_backup_b_json_entity_usr_config_0800
 * @tc.name: b_json_entity_usr_config_0800
 * @tc.desc: json文件标签中有异常引号
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 2
 * @tc.require: SR000H037V
 */
HWTEST_F(BJsonEntityUsrConfigTest, b_json_entity_usr_config_0800, testing::ext::TestSize.Level2)
{
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-begin b_json_entity_usr_config_0800";
    try {
        TestManager tm("b_json_entity_usr_config_0800");

        string root = tm.GetRootDirCurTest();
        string pathConfigFile = root + "config.json";
        SaveStringToFile(
            pathConfigFile,
            R"({"includes":["", "", ""], "excludes":["", "", ""], """""""""""""""""""""""""""""""""""""""""""""""""":[]})");

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(pathConfigFile.data(), O_RDONLY, 0)));
        auto cache = cachedEntity.Structuralize();
        vector<string> includes = cache.GetIncludes();
        EXPECT_EQ(includes, DEFAULT_INCLUDE_DIR);
        vector<string> excludes = cache.GetExcludes();
        EXPECT_EQ(excludes, DEFAULT_EXCLUDE_DIR);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonEntityUsrConfigTest-end b_json_entity_usr_config_0800";
}
} // namespace OHOS::FileManagement::Backup