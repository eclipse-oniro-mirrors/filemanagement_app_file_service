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

#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <sstream>

#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpBackupTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_backup_0100
 * @tc.name: SUB_backup_tools_op_backup_0100
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0382
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-info";
        map<string, vector<string>> mapArgToVal;
        vector<string> path = {"/data/backup/tmp"};
        mapArgToVal.insert(make_pair("pathCapFile", path));
        vector<string> bundles = {"com.example.app2backup"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        vector<string> local = {"true"};
        mapArgToVal.insert(make_pair("isLocal", local));

        // 创建测试路径
        string strPath = "/data/backup";
        mkdir(strPath.data(), S_IRWXU);

        // 尝试匹配当前命令，成功后执行
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-backup";
        vector<string_view> curOp;
        curOp.emplace_back("backup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            matchedOp->Execute(mapArgToVal);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_backup_0200
 * @tc.name: SUB_backup_tools_op_backup_0200
 * @tc.desc: 测试
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0382
 */
HWTEST_F(ToolsOpBackupTest, SUB_backup_tools_op_backup_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-begin SUB_backup_tools_op_backup_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The pathCapFile field is not contained.";
        map<string, vector<string>> mapArgToVal;
        vector<string> bundles = {"com.example.app2backup"};
        vector<string> path = {"/data/backup/tmp"};
        vector<string> local = {"true"};
        mapArgToVal.insert(make_pair("bundles", bundles));
        mapArgToVal.insert(make_pair("isLocal", local));

        vector<string_view> curOp;
        curOp.emplace_back("backup");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        int ret = 0;
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The bundles field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("isLocal", local));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }

        GTEST_LOG_(INFO) << "ToolsOpBackupTest-The isLocal field is noGt contained.";
        mapArgToVal.clear();
        mapArgToVal.insert(make_pair("pathCapFile", path));
        mapArgToVal.insert(make_pair("bundles", bundles));
        if (matchedOp != opeartions.end()) {
            ret = matchedOp->Execute(mapArgToVal);
            EXPECT_NE(ret, 0);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpBackupTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpBackupTest-end SUB_backup_tools_op_backup_0200";
}
} // namespace OHOS::FileManagement::Backup