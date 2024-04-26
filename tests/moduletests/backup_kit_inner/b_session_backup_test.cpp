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

#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "backup_kit_inner.h"
#include "unique_fd.h"
#include "utils_mock_global_variable.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static void OnFileReady(const BFileInfo &fileInfo, UniqueFd fd, ErrCode errCode)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest OnFileReady OK";
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest OnBundleStarted OK";
}

static void OnBundleFinished(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest OnBundleFinished OK";
}

static void OnAllBundlesFinished(ErrCode err)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest OnAllBundlesFinished OK";
}

static void OnBackupServiceDied()
{
    GTEST_LOG_(INFO) << "BSessionBackupTest OnBackupServiceDied OK";
}

class BSessionBackupTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    void Init();

    unique_ptr<BSessionBackup> backupPtr_;
    BSessionBackup::Callbacks callbacks_;
};

void BSessionBackupTest::SetUp()
{
    SetMockInitBackupOrRestoreSession(true);
    SetMockGetInstance(true);
    SetMockLoadSystemAbility(true);
    backupPtr_ = make_unique<BSessionBackup>();
}

void BSessionBackupTest::TearDown()
{
    backupPtr_ = nullptr;
}

void BSessionBackupTest::Init()
{
    callbacks_.onFileReady = OnFileReady;
    callbacks_.onBundleStarted = OnBundleStarted;
    callbacks_.onBundleFinished = OnBundleFinished;
    callbacks_.onAllBundlesFinished = OnAllBundlesFinished;
    callbacks_.onBackupServiceDied = OnBackupServiceDied;
}

/**
 * @tc.number: SUB_backup_b_session_backup_0100
 * @tc.name: SUB_backup_b_session_backup_0100
 * @tc.desc: 测试Start接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-begin SUB_backup_b_session_backup_0100";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        auto ret = backupPtr_->Start();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        ret = backupPtr_->Start();
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0100";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0200
 * @tc.name: SUB_backup_b_session_backup_0200
 * @tc.desc: 测试Finish接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-Finish SUB_backup_b_session_backup_0200";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        auto ret = backupPtr_->Finish();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        ret = backupPtr_->Finish();
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0200";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0300
 * @tc.name: SUB_backup_b_session_backup_0300
 * @tc.desc: 测试AppendBundles接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-AppendBundles SUB_backup_b_session_backup_0300";
    try {
        vector<BundleName> bundleNames;
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        auto ret = backupPtr_->AppendBundles(bundleNames);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        ret = backupPtr_->AppendBundles(bundleNames);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by AppendBundles.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0300";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0400
 * @tc.name: SUB_backup_b_session_backup_0400
 * @tc.desc: 测试Callbacks接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-begin SUB_backup_b_session_backup_0400";
    try {
        Init();
        BFileInfo backupFileInfo("", "", 0);
        callbacks_.onFileReady(backupFileInfo, UniqueFd(-1), 0);
        callbacks_.onBundleStarted(ErrCode(BError::Codes::OK), "");
        callbacks_.onBundleFinished(ErrCode(BError::Codes::OK), "");
        callbacks_.onAllBundlesFinished(ErrCode(BError::Codes::OK));
        callbacks_.onBackupServiceDied();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by Callbacks.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0400";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0500
 * @tc.name: SUB_backup_b_session_backup_0500
 * @tc.desc: 测试Init接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-begin SUB_backup_b_session_backup_0500";
    try {
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto backupPtr = BSessionBackup::Init(BSessionBackup::Callbacks {});
        EXPECT_EQ(backupPtr, nullptr);
        GTEST_LOG_(INFO) << "GetInstance is true";
        GTEST_LOG_(INFO) << "InitBackupSession is false";
        SetMockGetInstance(true);
        SetMockInitBackupOrRestoreSession(false);
        backupPtr = BSessionBackup::Init(BSessionBackup::Callbacks {});
        EXPECT_EQ(backupPtr, nullptr);
        GTEST_LOG_(INFO) << "InitBackupSession is true";
        SetMockInitBackupOrRestoreSession(true);
        Init();
        backupPtr = BSessionBackup::Init(callbacks_);
        EXPECT_NE(backupPtr, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by Init.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0500";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0600
 * @tc.name: SUB_backup_b_session_backup_0600
 * @tc.desc: 测试RegisterBackupServiceDied接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-begin SUB_backup_b_session_backup_0600";
    try {
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        backupPtr_->RegisterBackupServiceDied(nullptr);
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        backupPtr_->RegisterBackupServiceDied(nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by RegisterBackupServiceDied.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0600";
}

/**
 * @tc.number: SUB_backup_b_session_backup_0700
 * @tc.name: SUB_backup_b_session_backup_0700
 * @tc.desc: 测试析构流程接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionBackupTest, SUB_backup_b_session_backup_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionBackupTest-begin SUB_backup_b_session_backup_0700";
    try {
        SetMockGetInstance(true);
        SetMockLoadSystemAbility(true);
        Init();
        auto backupPtr = BSessionBackup::Init(callbacks_);
        EXPECT_NE(backupPtr, nullptr);

        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        backupPtr = nullptr;

        SetMockGetInstance(true);
        SetMockLoadSystemAbility(true);
        backupPtr = BSessionBackup::Init(callbacks_);
        EXPECT_NE(backupPtr, nullptr);

        GTEST_LOG_(INFO) << "LoadSystemAbility is false";
        SetMockLoadSystemAbility(false);
        backupPtr = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionBackupTest-an exception occurred by ~BSessionBackup.";
    }
    GTEST_LOG_(INFO) << "BSessionBackupTest-end SUB_backup_b_session_backup_0700";
}
} // namespace OHOS::FileManagement::Backup