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

#include <cstddef>
#include <cstdio>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "b_error/b_error.h"
#include "i_service_mock.h"
#include "i_service_reverse.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace testing;
using namespace std;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";

constexpr int32_t CLIENT_TOKEN_ID = 100;
map<string, BackupExtInfo> g_backupExtNameMap;
} // namespace

class SvcSessionManagerMockEx : public SvcSessionManager {
public:
    SvcSessionManagerMockEx(wptr<IServiceMock> reversePtr) : SvcSessionManager(reversePtr) {};
    virtual ~SvcSessionManagerMockEx() = default;
    MOCK_METHOD1(GetBundleExtNames, void(std::map<std::string, BackupExtInfo> &));
    MOCK_METHOD1(InitClient, void(Impl &));
};

class SvcSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override;

    void Init(IServiceReverse::Scenario scenario);

    sptr<SvcSessionManagerMockEx> sessionManagerPtr_;
    sptr<IServiceMock> serviceMock_;
};

void SvcSessionManagerTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void SvcSessionManagerTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
}

void SvcSessionManagerTest::SetUp(void)
{
    g_backupExtNameMap.clear();
    vector<string> bundleNames;
    bundleNames.emplace_back(BUNDLE_NAME);
    auto setBackupExtNameMap = [](const string &bundleName) {
        BackupExtInfo info {};
        info.backupExtName = BUNDLE_NAME;
        info.receExtManageJson = true;
        info.receExtAppDone = true;
        return make_pair(bundleName, info);
    };
    transform(bundleNames.begin(), bundleNames.end(), inserter(g_backupExtNameMap, g_backupExtNameMap.end()),
              setBackupExtNameMap);

    serviceMock_ = sptr<IServiceMock>(new IServiceMock());
    sessionManagerPtr_ = sptr<SvcSessionManagerMockEx>(new SvcSessionManagerMockEx(wptr(serviceMock_)));
    EXPECT_CALL(*sessionManagerPtr_, GetBundleExtNames(_)).WillRepeatedly(Return());
    EXPECT_CALL(*sessionManagerPtr_, InitClient(_)).WillRepeatedly(Return());
}

void SvcSessionManagerTest::TearDown()
{
    sessionManagerPtr_ = nullptr;
    serviceMock_ = nullptr;
}

void SvcSessionManagerTest::Init(IServiceReverse::Scenario scenario)
{
    sessionManagerPtr_->Active({
        .clientToken = CLIENT_TOKEN_ID,
        .scenario = scenario,
        .backupExtNameMap = move(g_backupExtNameMap),
    });
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyCallerAndScenario_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyCallerAndScenario_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyCallerAndScenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyCallerAndScenario_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_getscenario_0100
 * @tc.name: SUB_backup_sa_session_getscenario_0100
 * @tc.desc: 测试 GetScenario 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getscenario_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getscenario_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        IServiceReverse::Scenario scenario = sessionManagerPtr_->GetScenario();
        EXPECT_EQ(scenario, IServiceReverse::Scenario::RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by getscenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_getscenario_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0100
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0100
 * @tc.desc: 测试 OnBunleFileReady 接口 restroe流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0200
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0200
 * @tc.desc: 测试 OnBunleFileReady 接口 backup流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0200";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0200";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0300
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0300
 * @tc.desc: 测试 OnBunleFileReady 接口 backup流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0300";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0300";
}

/**
 * @tc.number: SUB_backup_sa_session_removeextinfo_0100
 * @tc.name: SUB_backup_sa_session_removeextinfo_0100
 * @tc.desc: 测试 RemoveExtInfo 移除bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_removeextinfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_removeextinfo_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by RemoveExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_removeextinfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.name: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.desc: 测试 VerifyBundleName 检验调用者给定的bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyBundleName_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        string bundleName = BUNDLE_NAME;
        sessionManagerPtr_->VerifyBundleName(bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyBundleName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        sessionManagerPtr_->SetExtFileNameRequest(BUNDLE_NAME, FILE_NAME);
        auto fileNameVec = sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
        for (auto &fileName : fileNameVec) {
            EXPECT_EQ(fileName, FILE_NAME);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.name: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.desc: 测试 GetSchedBundleName 调度器获取所需要的调度信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSchedBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetSchedBundleName_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        string bundleName;
        bool condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_EQ(bundleName, BUNDLE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetSchedBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetSchedBundleName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0100
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0100
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        auto action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::WAIT);

        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::START);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetServiceSchedAction_0100
 * @tc.name: SUB_backup_sa_session_SetServiceSchedAction_0100
 * @tc.desc: 测试 SetServiceSchedAction 设置状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetServiceSchedAction_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetServiceSchedAction_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        string bundleName;
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::RUNNING);
        bool condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_FALSE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetServiceSchedAction_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        string extName = sessionManagerPtr_->GetBackupExtName(BUNDLE_NAME);
        EXPECT_EQ(extName, BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.name: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.desc: 测试 GetExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtConnection_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtConnection_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        auto backupCon = sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
        EXPECT_NE(backupCon, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtConnection_0100";
}
} // namespace OHOS::FileManagement::Backup