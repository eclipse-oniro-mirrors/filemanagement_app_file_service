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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "ext_extension_mock.h"
#include "module_ipc/service.h"
#include "module_sched/sched_scheduler.h"
#include "service_proxy.h"
#include "service_reverse_mock.h"
#include "svc_session_manager_mock.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
constexpr int32_t SERVICE_ID = 5203;
} // namespace

class ServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    ErrCode Init(IServiceReverse::Scenario scenario);
    void InitExtConnection();
    void InitSched();

    sptr<Service> servicePtr_;
    sptr<ServiceReverseMock> remote_;
    sptr<SvcSessionManagerMock> sessionManagerPtr_;
};

void ServiceTest::SetUp()
{
    servicePtr_ = sptr<Service>(new Service(SERVICE_ID));
    sessionManagerPtr_ = sptr<SvcSessionManagerMock>(new SvcSessionManagerMock(wptr(servicePtr_)));
    remote_ = sptr(new ServiceReverseMock());
    servicePtr_->session_ = sessionManagerPtr_;
}

void ServiceTest::TearDown()
{
    remote_ = nullptr;
    sessionManagerPtr_ = nullptr;
    servicePtr_->session_ = nullptr;
    servicePtr_->sched_ = nullptr;
    servicePtr_ = nullptr;
}

ErrCode ServiceTest::Init(IServiceReverse::Scenario scenario)
{
    EXPECT_CALL(*sessionManagerPtr_, GetBundleExtNames(_)).Times(1).WillOnce(Return());
    EXPECT_CALL(*sessionManagerPtr_, InitClient(_)).Times(1).WillOnce(Return());
    EXPECT_CALL(*sessionManagerPtr_, InitExtConn(_))
        .Times(1)
        .WillOnce(Invoke(sessionManagerPtr_.GetRefPtr(), &SvcSessionManagerMock::InvokeInitExtConn));
    vector<string> bundleNames;
    bundleNames.emplace_back(BUNDLE_NAME);
    ErrCode ret = 0;
    if (scenario == IServiceReverse::Scenario::RESTORE) {
        ret = servicePtr_->InitRestoreSession(remote_, bundleNames);
    } else if (scenario == IServiceReverse::Scenario::BACKUP) {
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
        ret = servicePtr_->InitBackupSession(remote_, move(fd), bundleNames);
    }
    return ret;
}

void ServiceTest::InitExtConnection()
{
    AppExecFwk::ElementName element;
    element.SetBundleName(BUNDLE_NAME);
    int resultCode = 1;
    sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
    auto backupCon = servicePtr_->session_->GetExtConnection(BUNDLE_NAME);
    backupCon->OnAbilityConnectDone(element, mock->AsObject(), resultCode);
    bool ret = backupCon->IsExtAbilityConnected();
    EXPECT_TRUE(ret);
}

void ServiceTest::InitSched()
{
    servicePtr_->sched_ = sptr(new SchedScheduler(wptr(servicePtr_), wptr(sessionManagerPtr_)));
    servicePtr_->session_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
}

/**
 * @tc.number: SUB_Service_GetLocalCapabilities_0100
 * @tc.name: SUB_Service_GetLocalCapabilities_0100
 * @tc.desc: 测试 GetLocalCapabilities 获取本地能力文件
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_GetLocalCapabilities_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetLocalCapabilities_0100";
    try {
        UniqueFd fd = servicePtr_->GetLocalCapabilities();
        EXPECT_GT(fd, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetLocalCapabilities_0100";
}

/**
 * @tc.number: SUB_Service_InitRestoreSession_0100
 * @tc.name: SUB_Service_InitRestoreSession_0100
 * @tc.desc: 测试 InitRestoreSession restore初始化流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_InitRestoreSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_InitRestoreSession_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by InitRestoreSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_InitRestoreSession_0100";
}

/**
 * @tc.number: SUB_Service_InitBackupSession_0100
 * @tc.name: SUB_Service_InitBackupSession_0100
 * @tc.desc: 测试 InitBackupSession backup初始化流程
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_InitBackupSession_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_InitBackupSession_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by InitBackupSession.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_InitBackupSession_0100";
}

/**
 * @tc.number: SUB_Service_Start_0100
 * @tc.name: SUB_Service_Start_0100
 * @tc.desc: 测试 Start 备份恢复启动
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_Start_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_Start_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitSched();
        ret = servicePtr_->Start();
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_Start_0100";
}

/**
 * @tc.number: SUB_Service_PublishFile_0100
 * @tc.name: SUB_Service_PublishFile_0100
 * @tc.desc: 测试 PublishFile 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_PublishFile_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_PublishFile_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitExtConnection();
        BFileInfo fileInfo {BUNDLE_NAME, FILE_NAME, 0};
        ret = servicePtr_->PublishFile(fileInfo);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_PublishFile_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupExtension_0100
 * @tc.name: SUB_Service_LaunchBackupExtension_0100
 * @tc.desc: 测试 LaunchBackupExtension 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitExtConnection();
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0100";
}

/**
 * @tc.number: SUB_Service_LaunchBackupExtension_0200
 * @tc.name: SUB_Service_LaunchBackupExtension_0200
 * @tc.desc: 测试 LaunchBackupExtension 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_LaunchBackupExtension_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_LaunchBackupExtension_0200";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitExtConnection();
        ret = servicePtr_->LaunchBackupExtension(BUNDLE_NAME);
        EXPECT_NE(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by LaunchBackupExtension.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_LaunchBackupExtension_0200";
}

/**
 * @tc.number: SUB_Service_GetExtFileName_0100
 * @tc.name: SUB_Service_GetExtFileName_0100
 * @tc.desc: 测试 GetExtFileName 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_GetExtFileName_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_GetExtFileName_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        string bundleName = BUNDLE_NAME;
        string fileName = FILE_NAME;
        ret = servicePtr_->GetExtFileName(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by GetExtFileName.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_GetExtFileName_0100";
}

/**
 * @tc.number: SUB_Service_ExtStart_0100
 * @tc.name: SUB_Service_ExtStart_0100
 * @tc.desc: 测试 ExtStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitSched();
        InitExtConnection();

        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0100";
}

/**
 * @tc.number: SUB_Service_ExtStart_0200
 * @tc.name: SUB_Service_ExtStart_0200
 * @tc.desc: 测试 ExtStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0200";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitSched();
        InitExtConnection();
        string bundleName = BUNDLE_NAME;
        string fileName = FILE_NAME;
        ret = servicePtr_->GetExtFileName(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0200";
}

/**
 * @tc.number: SUB_Service_ExtStart_0300
 * @tc.name: SUB_Service_ExtStart_0300
 * @tc.desc: 测试 ExtStart 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_ExtStart_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtStart_0300";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitExtConnection();
        InitSched();
        string bundleName = BUNDLE_NAME;
        string fileName = "";
        ret = servicePtr_->GetExtFileName(bundleName, fileName);
        EXPECT_EQ(ret, BError(BError::Codes::OK));

        servicePtr_->ExtStart(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtStart.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtStart_0300";
}

/**
 * @tc.number: SUB_Service_ExtConnectFailed_0100
 * @tc.name: SUB_Service_ExtConnectFailed_0100
 * @tc.desc: 测试 ExtConnectFailed 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectFailed_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectFailed_0100";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitSched();
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectFailed.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectFailed_0100";
}

/**
 * @tc.number: SUB_Service_ExtConnectFailed_0200
 * @tc.name: SUB_Service_ExtConnectFailed_0200
 * @tc.desc: 测试 ExtConnectFailed 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceTest, SUB_Service_ExtConnectFailed_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceTest-begin SUB_Service_ExtConnectFailed_0200";
    try {
        ErrCode ret = Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_EQ(ret, BError(BError::Codes::OK));
        InitSched();
        servicePtr_->ExtConnectFailed(BUNDLE_NAME, BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceTest-an exception occurred by ExtConnectFailed.";
    }
    GTEST_LOG_(INFO) << "ServiceTest-end SUB_Service_ExtConnectFailed_0200";
}
} // namespace OHOS::FileManagement::Backup