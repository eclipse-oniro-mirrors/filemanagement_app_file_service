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

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "backup_kit_inner.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static void OnFileReady(const BFileInfo &fileInfo, UniqueFd fd)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest OnFileReady OK";
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest OnBundleStarted OK";
}

static void OnBundleFinished(ErrCode err, const BundleName name)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest OnBundleFinished OK";
}

static void OnAllBundlesFinished(ErrCode err)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest OnAllBundlesFinished OK";
}

static void OnBackupServiceDied()
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest OnBackupServiceDied OK";
}

class BSessionRestoreTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override;
    void TearDown() override;

    void Init();

    unique_ptr<BSessionRestore> restorePtr_;
    BSessionRestore::Callbacks callbacks_;
};

void BSessionRestoreTest::SetUp()
{
    restorePtr_ = unique_ptr<BSessionRestore>();
}

void BSessionRestoreTest::TearDown()
{
    restorePtr_ = nullptr;
}

void BSessionRestoreTest::Init()
{
    callbacks_.onFileReady = OnFileReady;
    callbacks_.onBundleStarted = OnBundleStarted;
    callbacks_.onBundleFinished = OnBundleFinished;
    callbacks_.onAllBundlesFinished = OnAllBundlesFinished;
    callbacks_.onBackupServiceDied = OnBackupServiceDied;
}

/**
 * @tc.number: SUB_backup_b_session_restore_0100
 * @tc.name: SUB_backup_b_session_restore_0100
 * @tc.desc: 测试Start接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0100";
    try {
        auto ret = restorePtr_->Start();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Start.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0100";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0200
 * @tc.name: SUB_backup_b_session_restore_0200
 * @tc.desc: 测试Callbacks接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0200";
    try {
        Init();
        BFileInfo bFileInfo("", "", 0);
        callbacks_.onFileReady(bFileInfo, UniqueFd(-1));
        callbacks_.onBundleStarted(ErrCode(BError::Codes::OK), "");
        callbacks_.onBundleFinished(ErrCode(BError::Codes::OK), "");
        callbacks_.onAllBundlesFinished(ErrCode(BError::Codes::OK));
        callbacks_.onBackupServiceDied();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Callbacks.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0200";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0300
 * @tc.name: SUB_backup_b_session_restore_0300
 * @tc.desc: 测试Init接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0300";
    try {
        vector<string> bundlesToBackup;
        auto backupPtr = BSessionRestore::Init(bundlesToBackup, {});
        EXPECT_EQ(backupPtr, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Init.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0300";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0400
 * @tc.name: SUB_backup_b_session_restore_0400
 * @tc.desc: 测试GetLocalCapabilities接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0377
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0400";
    try {
        auto fd = restorePtr_->GetLocalCapabilities();
        EXPECT_NE(fd, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0400";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0500
 * @tc.name: SUB_backup_b_session_restore_0500
 * @tc.desc: 测试PublishFile接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0500";
    try {
        BFileInfo bFileInfo("", "", 0);
        auto ret = restorePtr_->PublishFile(bFileInfo);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0500";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0600
 * @tc.name: SUB_backup_b_session_restore_0600
 * @tc.desc: 测试GetExtFileName接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0600";
    try {
        string bundleName = "";
        string fileName = "";
        restorePtr_->GetExtFileName(bundleName, fileName);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by GetExtFileName.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0600";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0700
 * @tc.name: SUB_backup_b_session_restore_0700
 * @tc.desc: 测试RegisterBackupServiceDied接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H037V
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0700";
    try {
        Init();
        restorePtr_->RegisterBackupServiceDied(nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by RegisterBackupServiceDied.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0700";
}

} // namespace OHOS::FileManagement::Backup