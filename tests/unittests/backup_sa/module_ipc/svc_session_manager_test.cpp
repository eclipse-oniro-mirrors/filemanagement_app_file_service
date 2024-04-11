/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "ext_extension_mock.h"
#include "file_ex.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"
#include "service_reverse_mock.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace testing;
using namespace std;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t CLIENT_TOKEN_ID = 100;
} // namespace

class SvcSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override {};
    void TearDown() override {};
    void Init(IServiceReverse::Scenario scenario);

    static inline sptr<SvcSessionManager> sessionManagerPtr_ = nullptr;
    static inline sptr<ServiceReverseMock> remote_ = nullptr;
    static inline sptr<Service> servicePtr_ = nullptr;
};

void SvcSessionManagerTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    remote_ = sptr(new ServiceReverseMock());
    servicePtr_ = sptr(new Service(SERVICE_ID));
    sessionManagerPtr_ = sptr<SvcSessionManager>(new SvcSessionManager(wptr(servicePtr_)));
}

void SvcSessionManagerTest::TearDownTestCase(void)
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    sessionManagerPtr_ = nullptr;
    servicePtr_ = nullptr;
    remote_ = nullptr;
}

void SvcSessionManagerTest::Init(IServiceReverse::Scenario scenario)
{
    vector<string> bundleNames;
    map<string, BackupExtInfo> backupExtNameMap;
    bundleNames.emplace_back(BUNDLE_NAME);
    sessionManagerPtr_->Active(
        {.clientToken = CLIENT_TOKEN_ID, .scenario = scenario, .backupExtNameMap = {}, .clientProxy = remote_});
    sessionManagerPtr_->IsOnAllBundlesFinished();
    sessionManagerPtr_->AppendBundles(bundleNames);
    sessionManagerPtr_->Finish();
    sessionManagerPtr_->IsOnAllBundlesFinished();
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0100
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
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
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0101
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0101
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyCallerAndScenario_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyCallerAndScenario_0101";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::BACKUP);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyCallerAndScenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyCallerAndScenario_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0102
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0102
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyCallerAndScenario_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyCallerAndScenario_0102";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::BACKUP);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyCallerAndScenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyCallerAndScenario_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyCallerAndScenario_0103
 * @tc.name: SUB_backup_sa_session_VerifyCallerAndScenario_0103
 * @tc.desc: 测试 VerifyCallerAndScenario 是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyCallerAndScenario_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyCallerAndScenario_0103";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->VerifyCallerAndScenario(CLIENT_TOKEN_ID, IServiceReverse::Scenario::BACKUP);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyCallerAndScenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyCallerAndScenario_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.name: SUB_backup_sa_session_VerifyBundleName_0100
 * @tc.desc: 测试 VerifyBundleName 检验调用者给定的bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyBundleName_0100";
    try {
        string bundleName = BUNDLE_NAME;
        sessionManagerPtr_->VerifyBundleName(bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyBundleName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyBundleName_0101
 * @tc.name: SUB_backup_sa_session_VerifyBundleName_0101
 * @tc.desc: 测试 VerifyBundleName 检验调用者给定的bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyBundleName_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyBundleName_0101";
    try {
        string bundleName = BUNDLE_NAME;
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->VerifyBundleName(bundleName);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyBundleName_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_VerifyBundleName_0102
 * @tc.name: SUB_backup_sa_session_VerifyBundleName_0102
 * @tc.desc: 测试 VerifyBundleName 检验调用者给定的bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_VerifyBundleName_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_VerifyBundleName_0102";
    try {
        string bundleName = "";
        sessionManagerPtr_->VerifyBundleName(bundleName);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by VerifyBundleName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_VerifyBundleName_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceReverseProxy_0100
 * @tc.name: SUB_backup_sa_session_GetServiceReverseProxy_0100
 * @tc.desc: 测试 GetServiceReverseProxy
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceReverseProxy_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceReverseProxy_0100";
    try {
        auto proxy = sessionManagerPtr_->GetServiceReverseProxy();
        EXPECT_NE(proxy, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceReverseProxy.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceReverseProxy_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceReverseProxy_0101
 * @tc.name: SUB_backup_sa_session_GetServiceReverseProxy_0101
 * @tc.desc: 测试 GetServiceReverseProxy
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceReverseProxy_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceReverseProxy_0101";
    try {
        sptr<IServiceReverse> clientProxy = nullptr;
        auto proxy = sessionManagerPtr_->GetServiceReverseProxy();
        EXPECT_NE(proxy, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceReverseProxy.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceReverseProxy_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_getscenario_0100
 * @tc.name: SUB_backup_sa_session_getscenario_0100
 * @tc.desc: 测试 GetScenario 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getscenario_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getscenario_0100";
    try {
        IServiceReverse::Scenario scenario = sessionManagerPtr_->GetScenario();
        EXPECT_EQ(scenario, IServiceReverse::Scenario::RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by getscenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_getscenario_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_getscenario_0101
 * @tc.name: SUB_backup_sa_session_getscenario_0101
 * @tc.desc: 测试 GetScenario 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getscenario_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getscenario_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        IServiceReverse::Scenario scenario = sessionManagerPtr_->GetScenario();
        EXPECT_EQ(scenario, IServiceReverse::Scenario::RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by getscenario.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_getscenario_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0100
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0100
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0100";
    try {
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0101
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0101
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0102
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0102
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0102";
    try {
        string bundleName = "";
        bool condition = sessionManagerPtr_->OnBunleFileReady(bundleName);
        EXPECT_FALSE(condition);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0103
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0103
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0103";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        bool condition = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0104
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0104
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0104";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        string fileName = "backup_config.json";
        BackupExtInfo ss;
        ss.fileNameInfo = {};
        bool condition = sessionManagerPtr_->OnBunleFileReady(fileName);
        EXPECT_FALSE(condition);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0104";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0105
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0105
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0105";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        string fileName = "";
        bool condition = sessionManagerPtr_->OnBunleFileReady(fileName);
        EXPECT_FALSE(condition);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0105";
}

/**
 * @tc.number: SUB_backup_sa_session_onbunlefileready_0106
 * @tc.name: SUB_backup_sa_session_onbunlefileready_0106
 * @tc.desc: 测试 OnBunleFileReady 接口 restore流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0106, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0106";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        BackupExtInfo ss;
        ss.receExtManageJson = true;
        ss.receExtAppDone = true;
        ss.fileNameInfo = {};
        bool condition = sessionManagerPtr_->OnBunleFileReady(FILE_NAME);
        EXPECT_TRUE(condition);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by onbunlefileready.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_onbunlefileready_0106";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0100
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0100";
    try {
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
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0101
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0101
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0101";
    try {
        sessionManagerPtr_->SetExtFileNameRequest(BUNDLE_NAME, FILE_NAME);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        auto fileNameVec = sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
        for (auto &fileName : fileNameVec) {
            EXPECT_EQ(fileName, FILE_NAME);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtFileNameRequest_0102
 * @tc.name: SUB_backup_sa_session_GetExtFileNameRequest_0102
 * @tc.desc: 测试 GetExtFileNameRequest 获取暂存真实文件请求
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtFileNameRequest_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtFileNameRequest_0102";
    try {
        sessionManagerPtr_->SetExtFileNameRequest(BUNDLE_NAME, FILE_NAME);
        Init(IServiceReverse::Scenario::BACKUP);
        auto fileNameVec = sessionManagerPtr_->GetExtFileNameRequest(BUNDLE_NAME);
        for (auto &fileName : fileNameVec) {
            EXPECT_NE(fileName, FILE_NAME);
        }
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtFileNameRequest.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtFileNameRequest_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.name: SUB_backup_sa_session_GetExtConnection_0100
 * @tc.desc: 测试 GetExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtConnection_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtConnection_0100";
    try {
        auto backupCon = sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
        EXPECT_NE(backupCon, nullptr);
        sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
        backupCon->OnAbilityConnectDone({}, mock->AsObject(), 0);
        backupCon->OnAbilityDisconnectDone({}, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtConnection_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtConnection_0101
 * @tc.name: SUB_backup_sa_session_GetExtConnection_0101
 * @tc.desc: 测试 GetExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtConnection_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtConnection_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        auto backupCon = sessionManagerPtr_->GetExtConnection(BUNDLE_NAME);
        EXPECT_NE(backupCon, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtConnection_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetExtConnection_0102
 * @tc.name: SUB_backup_sa_session_GetExtConnection_0102
 * @tc.desc: 测试 GetExtConnection 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetExtConnection_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetExtConnection_0102";
    try {
        string bundleName = "";
        auto backupCon = sessionManagerPtr_->GetExtConnection(bundleName);
        EXPECT_EQ(backupCon, nullptr);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetExtConnection.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetExtConnection_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_DumpInfo_0100
 * @tc.name: SUB_backup_sa_session_DumpInfo_0100
 * @tc.desc: 测试 DumpInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_DumpInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_DumpInfo_0100";
    try {
        TestManager tm("SvcSessionManagerTest_GetFd_0100");
        string fileUri = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(fileUri.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        sessionManagerPtr_->DumpInfo(fd, {});
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by DumpInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_DumpInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.name: SUB_backup_sa_session_GetSchedBundleName_0100
 * @tc.desc: 测试 GetSchedBundleName 调度器获取所需要的调度信息
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSchedBundleName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetSchedBundleName_0100";
    try {
        string bundleName;
        bool condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_EQ(bundleName, BUNDLE_NAME);
        EXPECT_TRUE(condition);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-GetSchedBundleName Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_FALSE(condition);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        condition = sessionManagerPtr_->GetSchedBundleName(bundleName);
        EXPECT_FALSE(condition);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::WAIT);
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
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0100";
    try {
        auto action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::WAIT);

        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0102
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0102
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0102";
    try {
        auto action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_NE(action, BConstants::ServiceSchedAction::WAIT);

        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(BUNDLE_NAME);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_GetServiceSchedAction_0103
 * @tc.name: SUB_backup_sa_session_GetServiceSchedAction_0103
 * @tc.desc: 测试 GetServiceSchedAction 获取状态
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetServiceSchedAction_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetServiceSchedAction_0103";
    try {
        string bundleName = "";
        auto action = sessionManagerPtr_->GetServiceSchedAction(bundleName);
        EXPECT_EQ(action, BConstants::ServiceSchedAction::WAIT);

        sessionManagerPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::START);
        action = sessionManagerPtr_->GetServiceSchedAction(bundleName);
        EXPECT_NE(action, BConstants::ServiceSchedAction::START);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-SetServiceSchedAction Branches";
        sessionManagerPtr_->SetServiceSchedAction(bundleName, BConstants::ServiceSchedAction::FINISH);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetServiceSchedAction.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetServiceSchedAction_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0100
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0100";
    try {
        sessionManagerPtr_->SetBackupExtName(BUNDLE_NAME, BUNDLE_NAME);
        string extName = sessionManagerPtr_->GetBackupExtName(BUNDLE_NAME);
        EXPECT_EQ(extName, BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0101
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0101
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0101";
    try {
        sessionManagerPtr_->SetBackupExtName(BUNDLE_NAME, BUNDLE_NAME);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        string extName = sessionManagerPtr_->GetBackupExtName(BUNDLE_NAME);
        EXPECT_EQ(extName, BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBackupExtName_0102
 * @tc.name: SUB_backup_sa_session_GetBackupExtName_0102
 * @tc.desc: 测试 GetBackupExtName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBackupExtName_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBackupExtName_0102";
    try {
        string bundleName = "";
        sessionManagerPtr_->SetBackupExtName(bundleName, bundleName);
        string extName = sessionManagerPtr_->GetBackupExtName(bundleName);
        EXPECT_NE(extName, bundleName);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBackupExtName_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_NeedToUnloadService_0100
 * @tc.name: SUB_backup_sa_session_NeedToUnloadService_0100
 * @tc.desc: 测试 NeedToUnloadService 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_NeedToUnloadService_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_NeedToUnloadService_0100";
    try {
        auto ret = sessionManagerPtr_->NeedToUnloadService();
        EXPECT_EQ(ret, false);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by NeedToUnloadService.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_NeedToUnloadService_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_NeedToUnloadService_0101
 * @tc.name: SUB_backup_sa_session_NeedToUnloadService_0101
 * @tc.desc: 测试 NeedToUnloadService 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_NeedToUnloadService_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_NeedToUnloadService_0101";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        auto ret = sessionManagerPtr_->NeedToUnloadService();
        EXPECT_EQ(ret, true);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by NeedToUnloadService.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_NeedToUnloadService_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_removeextinfo_0100
 * @tc.name: SUB_backup_sa_session_removeextinfo_0100
 * @tc.desc: 测试 RemoveExtInfo 移除bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_removeextinfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_removeextinfo_0100";
    try {
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-RemoveExtInfo Branches";
        sessionManagerPtr_->RemoveExtInfo(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by RemoveExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_removeextinfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_removeextinfo_0101
 * @tc.name: SUB_backup_sa_session_removeextinfo_0101
 * @tc.desc: 测试 RemoveExtInfo 移除bundleName是否是有效的
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_removeextinfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_removeextinfo_0101";
    try {
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-RemoveExtInfo Branches";
        string bundleName = "";
        sessionManagerPtr_->RemoveExtInfo(bundleName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by RemoveExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_removeextinfo_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0100
 * @tc.name: SUB_backup_sa_session_Deactive_0100
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0100";
    try {
        sessionManagerPtr_->Deactive(nullptr, true);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-Deactive Branches One";
        sessionManagerPtr_->Deactive(nullptr, true);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-Deactive Branches Two";
        Init(IServiceReverse::Scenario::BACKUP);
        sessionManagerPtr_->Deactive(remote_, false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-Deactive Branches Three";
        Init(IServiceReverse::Scenario::BACKUP);
        sessionManagerPtr_->Deactive(remote_, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0101
 * @tc.name: SUB_backup_sa_session_Deactive_0101
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->Deactive(nullptr, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0102
 * @tc.name: SUB_backup_sa_session_Deactive_0102
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0102";
    try {
        sptr<IServiceReverse> clientProxy = nullptr;
        sessionManagerPtr_->Deactive(nullptr, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0103
 * @tc.name: SUB_backup_sa_session_Deactive_0103
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0103, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0103";
    try {
        bool force = false;
        sptr<IServiceReverse> clientProxy = nullptr;
        sessionManagerPtr_->Deactive(nullptr, force);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0103";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0104
 * @tc.name: SUB_backup_sa_session_Deactive_0104
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0104, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0104";
    try {
        bool force = false;
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->Deactive(nullptr, force);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0104";
}

/**
 * @tc.number: SUB_backup_sa_session_Deactive_0105
 * @tc.name: SUB_backup_sa_session_Deactive_0105
 * @tc.desc: 测试 Deactive
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Deactive_0105, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Deactive_0105";
    try {
        bool force = false;
        Init(IServiceReverse::Scenario::BACKUP);
        sessionManagerPtr_->Deactive(nullptr, force);
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Deactive.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Deactive_0105";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBunleExtManageInfo_0100
 * @tc.name: SUB_backup_sa_session_OnBunleExtManageInfo_0100
 * @tc.desc: 测试 OnBunleExtManageInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBunleExtManageInfo_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBunleExtManageInfo_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        auto ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
        TestManager tm("SvcSessionManagerTest_GetFd_0100");
        string filePath = tm.GetRootDirCurTest().append(MANAGE_JSON);
        SaveStringToFile(filePath, R"({"fileName" : "1.tar"})");
        UniqueFd fd(open(filePath.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
        sessionManagerPtr_->OnBunleExtManageInfo(BUNDLE_NAME, move(fd));
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_TRUE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBunleExtManageInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBunleExtManageInfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBunleExtManageInfo_0101
 * @tc.name: SUB_backup_sa_session_OnBunleExtManageInfo_0101
 * @tc.desc: 测试 OnBunleExtManageInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBunleExtManageInfo_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBunleExtManageInfo_0101";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        auto ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
        TestManager tm("SvcSessionManagerTest_GetFd_0101");
        string filePath = tm.GetRootDirCurTest().append(MANAGE_JSON);
        SaveStringToFile(filePath, R"({"fileName" : "1.tar"})");
        UniqueFd fd(open(filePath.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
        sessionManagerPtr_->OnBunleExtManageInfo(BUNDLE_NAME, move(fd));
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
            ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBunleExtManageInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBunleExtManageInfo_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBunleExtManageInfo_0102
 * @tc.name: SUB_backup_sa_session_OnBunleExtManageInfo_0102
 * @tc.desc: 测试 OnBunleExtManageInfo
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBunleExtManageInfo_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBunleExtManageInfo_0102";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        auto ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
        TestManager tm("SvcSessionManagerTest_GetFd_0102");
        string filePath = tm.GetRootDirCurTest().append(MANAGE_JSON);
        SaveStringToFile(filePath, R"({"fileName" : "1.tar"})");
        UniqueFd fd(open(filePath.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR));
        sessionManagerPtr_->OnBunleExtManageInfo(BUNDLE_NAME, move(fd));
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches";
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-OnBunleFileReady Branches End";
    } catch (...) {
        EXPECT_FALSE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBunleExtManageInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBunleExtManageInfo_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_OnBunleFileReady_0200
 * @tc.name: SUB_backup_sa_session_OnBunleFileReady_0200
 * @tc.desc: 测试 OnBunleFileReady
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_OnBunleFileReady_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_OnBunleFileReady_0200";
    try {
        sessionManagerPtr_->Deactive(nullptr, true);
        Init(IServiceReverse::Scenario::BACKUP);
        auto ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
        ret = sessionManagerPtr_->OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
        EXPECT_FALSE(ret);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnBunleFileReady.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_OnBunleFileReady_0200";
}

/**
 * @tc.number: SUB_backup_sa_session_SetSessionUserId_0100
 * @tc.name: SUB_backup_sa_session_SetSessionUserId_0100
 * @tc.desc: 测试 SetSessionUserId
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetSessionUserId_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_backup_sa_session_SetSessionUserId_0100";
    try {
        int32_t userId = 1;
        sessionManagerPtr_->SetSessionUserId(userId);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by SetSessionUserId.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_backup_sa_session_SetSessionUserId_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetSessionUserId_0100
 * @tc.name: SUB_backup_sa_session_GetSessionUserId_0100
 * @tc.desc: 测试 GetSessionUserId
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I8ZIMJ
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetSessionUserId_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_backup_sa_session_GetSessionUserId_0100";
    try {
        sessionManagerPtr_->GetSessionUserId();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetSessionUserId.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_backup_sa_session_GetSessionUserId_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleRestoreType_0100
 * @tc.name: SUB_backup_sa_session_GetBundleRestoreType_0100
 * @tc.desc: 测试 GetBundleRestoreType 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleRestoreType_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleRestoreType_0100";
    try {
        sessionManagerPtr_->SetBundleRestoreType(BUNDLE_NAME, RESTORE_DATA_READDY);
        auto ret = sessionManagerPtr_->GetBundleRestoreType(BUNDLE_NAME);
        EXPECT_EQ(ret, RESTORE_DATA_READDY);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleRestoreType.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleRestoreType_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleRestoreType_0101
 * @tc.name: SUB_backup_sa_session_GetBundleRestoreType_0101
 * @tc.desc: 测试 GetBundleRestoreType 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleRestoreType_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleRestoreType_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetBundleRestoreType(BUNDLE_NAME, RESTORE_DATA_READDY);
        auto ret = sessionManagerPtr_->GetBundleRestoreType(BUNDLE_NAME);
        EXPECT_EQ(ret, RESTORE_DATA_READDY);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleRestoreType.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleRestoreType_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionCode_0100
 * @tc.name: SUB_backup_sa_session_GetBundleVersionCode_0100
 * @tc.desc: 测试 GetBundleVersionCode 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionCode_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionCode_0100";
    try {
        sessionManagerPtr_->SetBundleVersionCode(BUNDLE_NAME, 1000000);
        int ret = static_cast<int>(sessionManagerPtr_->GetBundleVersionCode(BUNDLE_NAME));
        EXPECT_EQ(ret, 1000000);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionCode.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionCode_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionCode_0101
 * @tc.name: SUB_backup_sa_session_GetBundleVersionCode_0101
 * @tc.desc: 测试 GetBundleVersionCode 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionCode_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionCode_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetBundleVersionCode(BUNDLE_NAME, 1000000);
        int ret = static_cast<int>(sessionManagerPtr_->GetBundleVersionCode(BUNDLE_NAME));
        EXPECT_EQ(ret, 1000000);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionCode.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionCode_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionName_0100
 * @tc.name: SUB_backup_sa_session_GetBundleVersionName_0100
 * @tc.desc: 测试 GetBundleVersionName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionName_0100";
    try {
        sessionManagerPtr_->SetBundleVersionName(BUNDLE_NAME, "1.0.0");
        auto ret = sessionManagerPtr_->GetBundleVersionName(BUNDLE_NAME);
        EXPECT_EQ(ret, "1.0.0");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionName_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_GetBundleVersionName_0101
 * @tc.name: SUB_backup_sa_session_GetBundleVersionName_0101
 * @tc.desc: 测试 GetBundleVersionName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6VA38
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_GetBundleVersionName_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_GetBundleVersionName_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetBundleVersionName(BUNDLE_NAME, "1.0.0");
        auto ret = sessionManagerPtr_->GetBundleVersionName(BUNDLE_NAME);
        EXPECT_EQ(ret, "1.0.0");
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBundleVersionName.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_GetBundleVersionName_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_Start_0100
 * @tc.name: SUB_backup_sa_session_Start_0100
 * @tc.desc: 测试 Start
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Start_0100";
    try {
        sessionManagerPtr_->Start();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Start_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_Start_0101
 * @tc.name: SUB_backup_sa_session_Start_0101
 * @tc.desc: 测试 Start
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_Start_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_Start_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->Start();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_Start_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_IsOnOnStartSched_0100
 * @tc.name: SUB_backup_sa_session_IsOnOnStartSched_0100
 * @tc.desc: 测试 IsOnOnStartSched
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_IsOnOnStartSched_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_IsOnOnStartSched_0100";
    try {
        SvcSessionManager::Impl impl_;
        impl_.isBackupStart = true;
        auto res = impl_.backupExtNameMap.size();
        EXPECT_FALSE(res);
        bool ret = sessionManagerPtr_->IsOnOnStartSched();
        EXPECT_EQ(ret, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by IsOnOnStartSched.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_IsOnOnStartSched_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_IsOnOnStartSched_0101
 * @tc.name: SUB_backup_sa_session_IsOnOnStartSched_0101
 * @tc.desc: 测试 IsOnOnStartSched
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_IsOnOnStartSched_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_IsOnOnStartSched_0101";
    try {
        bool ret = sessionManagerPtr_->IsOnOnStartSched();
        EXPECT_EQ(ret, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by IsOnOnStartSched.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_IsOnOnStartSched_0101";
}

/**
 * @tc.number: SUB_backup_sa_session_IsOnOnStartSched_0102
 * @tc.name: SUB_backup_sa_session_IsOnOnStartSched_0102
 * @tc.desc: 测试 IsOnOnStartSched
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_IsOnOnStartSched_0102, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_IsOnOnStartSched_0102";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        bool ret = sessionManagerPtr_->IsOnOnStartSched();
        EXPECT_EQ(ret, true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by IsOnOnStartSched.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_IsOnOnStartSched_0102";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleDataSize_0100
 * @tc.name: SUB_backup_sa_session_SetBundleDataSize_0100
 * @tc.desc: 测试 SetBundleDataSize
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleDataSize_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleDataSize_0100";
    try {
        sessionManagerPtr_->SetBundleDataSize(BUNDLE_NAME, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleDataSize.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleDataSize_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_SetBundleDataSize_0101
 * @tc.name: SUB_backup_sa_session_SetBundleDataSize_0101
 * @tc.desc: 测试 SetBundleDataSize
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_SetBundleDataSize_0101, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_SetBundleDataSize_0101";
    try {
        SvcSessionManager::Impl impl_;
        impl_.clientToken = 0;
        sessionManagerPtr_->SetBundleDataSize(BUNDLE_NAME, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by SetBundleDataSize.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_SetBundleDataSize_0101";
}
} // namespace OHOS::FileManagement::Backup