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

#include "b_error/b_error.h"
#include "module_ipc/service.h"
#include "module_sched/sched_scheduler.h"
#include "svc_session_manager_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";
constexpr int32_t CLIENT_TOKEN_ID = 100;
constexpr int32_t SERVICE_ID = 5203;
} // namespace

class ServiceMock : public Service {
public:
    explicit ServiceMock(int32_t saID) : Service(saID) {};
    ~ServiceMock() = default;

    MOCK_METHOD1(LaunchBackupExtension, ErrCode(const BundleName &));
    MOCK_METHOD1(ExtStart, void(const string &));
};

class SchedSchedulerTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    void Init(IServiceReverse::Scenario scenario);

    sptr<SchedScheduler> schedPtr_;
    sptr<ServiceMock> servicePtr_;
    sptr<SvcSessionManagerMock> sessionManagerPtr_;
};

void SchedSchedulerTest::SetUp()
{
    servicePtr_ = sptr<ServiceMock>(new ServiceMock(SERVICE_ID));
    sessionManagerPtr_ = sptr<SvcSessionManagerMock>(new SvcSessionManagerMock(wptr(servicePtr_)));
    schedPtr_ = sptr(new SchedScheduler(wptr(servicePtr_), wptr(sessionManagerPtr_)));
}

void SchedSchedulerTest::TearDown()
{
    schedPtr_ = nullptr;
    sessionManagerPtr_ = nullptr;
    servicePtr_ = nullptr;
}

void SchedSchedulerTest::Init(IServiceReverse::Scenario scenario)
{
    vector<string> bundleNames;
    map<string, BackupExtInfo> backupExtNameMap;
    bundleNames.emplace_back(BUNDLE_NAME);
    auto setBackupExtNameMap = [](const string &bundleName) {
        BackupExtInfo info {};
        info.backupExtName = BUNDLE_NAME;
        info.receExtManageJson = true;
        info.receExtAppDone = true;
        return make_pair(bundleName, info);
    };
    transform(bundleNames.begin(), bundleNames.end(), inserter(backupExtNameMap, backupExtNameMap.end()),
              setBackupExtNameMap);

    EXPECT_CALL(*sessionManagerPtr_, GetBundleExtNames(_)).Times(1).WillOnce(Return());
    EXPECT_CALL(*sessionManagerPtr_, InitClient(_)).Times(1).WillOnce(Return());
    EXPECT_CALL(*sessionManagerPtr_, InitExtConn(_))
        .Times(1)
        .WillOnce(Invoke(sessionManagerPtr_.GetRefPtr(), &SvcSessionManagerMock::InvokeInitExtConn));
    sessionManagerPtr_->Active({
        .clientToken = CLIENT_TOKEN_ID,
        .scenario = scenario,
        .backupExtNameMap = move(backupExtNameMap),
    });
}

/**
 * @tc.number: SUB_Service_Sched_0100
 * @tc.name: SUB_Service_Sched_0100
 * @tc.desc: 测试 Sched 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_Sched_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_Sched_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::FINISH);
        schedPtr_->Sched();
        servicePtr_->OnStop();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by Sched.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_Sched_0100";
}

/**
 * @tc.number: SUB_Service_ExecutingQueueTasks_0100
 * @tc.name: SUB_Service_ExecutingQueueTasks_0100
 * @tc.desc: 测试 ExecutingQueueTasks 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_ExecutingQueueTasks_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_ExecutingQueueTasks_0100";
    try {
        Init(IServiceReverse::Scenario::RESTORE);
        EXPECT_CALL(*servicePtr_, LaunchBackupExtension(_)).Times(1).WillOnce(Return(0));
        EXPECT_CALL(*servicePtr_, ExtStart(_)).Times(1).WillOnce(Return());
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        schedPtr_->ExecutingQueueTasks(BUNDLE_NAME);
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::RUNNING);
        schedPtr_->ExecutingQueueTasks(BUNDLE_NAME);
        servicePtr_->OnStop();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by ExecutingQueueTasks.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_ExecutingQueueTasks_0100";
}

/**
 * @tc.number: SUB_Service_RemoveExtConn_0100
 * @tc.name: SUB_Service_RemoveExtConn_0100
 * @tc.desc: 测试 RemoveExtConn 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(SchedSchedulerTest, SUB_Service_RemoveExtConn_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "SchedSchedulerTest-begin SUB_Service_RemoveExtConn_0100";
    try {
        Init(IServiceReverse::Scenario::BACKUP);
        EXPECT_CALL(*servicePtr_, LaunchBackupExtension(_)).Times(1).WillOnce(Return(0));
        sessionManagerPtr_->SetServiceSchedAction(BUNDLE_NAME, BConstants::ServiceSchedAction::START);
        schedPtr_->ExecutingQueueTasks(BUNDLE_NAME);
        schedPtr_->RemoveExtConn(BUNDLE_NAME);
        servicePtr_->OnStop();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SchedSchedulerTest-an exception occurred by RemoveExtConn.";
    }
    GTEST_LOG_(INFO) << "SchedSchedulerTest-end SUB_Service_RemoveExtConn_0100";
}
} // namespace OHOS::FileManagement::Backup