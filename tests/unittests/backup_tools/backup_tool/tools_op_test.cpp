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

#include <iostream>
#include <map>
#include <sstream>

#include <gtest/gtest.h>

#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
class ToolsOpTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_tools_op_0100
 * @tc.name: SUB_backup_tools_op_0100
 * @tc.desc: 测试Execute分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0382
 */
HWTEST_F(ToolsOpTest, SUB_backup_tools_op_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpTest-begin SUB_backup_tools_op_0100";
    try {
        GTEST_LOG_(INFO) << "ToolsOpTest-info";
        bool autoTest = ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
            .opName = {"test"},
            .funcGenHelpMsg = nullptr,
            .funcExec = nullptr,
        }});
        EXPECT_TRUE(autoTest);
        map<string, vector<string>> mapArgToVal;
        vector<string_view> curOp;
        curOp.emplace_back("test");
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            matchedOp->Execute(mapArgToVal);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpTest-end SUB_backup_tools_op_0100";
}

/**
 * @tc.number: SUB_backup_tools_op_0200
 * @tc.name: SUB_backup_tools_op_0200
 * @tc.desc: 测试Register分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0382
 */
HWTEST_F(ToolsOpTest, SUB_backup_tools_op_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ToolsOpTest-begin SUB_backup_tools_op_0200";
    try {
        GTEST_LOG_(INFO) << "ToolsOpTest-info";
        bool autoTest = ToolsOp::Register(ToolsOp {ToolsOp::Descriptor {
            .opName = {"???"},
            .funcGenHelpMsg = nullptr,
            .funcExec = nullptr,
        }});
        EXPECT_FALSE(autoTest);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ToolsOpTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "ToolsOpTest-end SUB_backup_tools_op_0200";
}
} // namespace OHOS::FileManagement::Backup