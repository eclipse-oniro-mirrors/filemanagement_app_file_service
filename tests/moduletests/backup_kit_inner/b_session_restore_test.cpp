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

#include <cstdint>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "backup_kit_inner.h"
#include "test_manager.h"
#include "unique_fd.h"
#include "utils_mock_global_variable.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace OHOS::FileManagement::Backup {
using namespace std;

static void OnFileReady(const BFileInfo &fileInfo, UniqueFd fd, int32_t errCode)
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

    unique_ptr<BSessionRestore> restorePtr_ = nullptr;
    BSessionRestore::Callbacks callbacks_;
};

void BSessionRestoreTest::SetUp()
{
    SetMockInitBackupOrRestoreSession(true);
    SetMockGetInstance(true);
    SetMockLoadSystemAbility(true);
    restorePtr_ = make_unique<BSessionRestore>();
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
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0100";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0100 restorePtr_ == nullptr";
            return;
        }
        auto ret = restorePtr_->Start();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        ret = restorePtr_->Start();
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
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
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0200";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0200 restorePtr_ == nullptr";
            return;
        }
        Init();
        BFileInfo bFileInfo("", "", 0);
        callbacks_.onFileReady(bFileInfo, UniqueFd(-1), 0);
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
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0300";
    try {
        GTEST_LOG_(INFO) << "GetInstance is true";
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0300 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        vector<string> bundlesToRestore;
        auto restorePtr = BSessionRestore::Init({});
        EXPECT_EQ(restorePtr, nullptr);
        GTEST_LOG_(INFO) << "GetInstance is true";
        GTEST_LOG_(INFO) << "InitBackupSession is false";
        SetMockGetInstance(true);
        SetMockInitBackupOrRestoreSession(false);
        restorePtr = BSessionRestore::Init({});
        EXPECT_EQ(restorePtr, nullptr);
        GTEST_LOG_(INFO) << "InitBackupSession is true";
        SetMockInitBackupOrRestoreSession(true);
        Init();
        restorePtr = BSessionRestore::Init(callbacks_);
        EXPECT_NE(restorePtr, nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Init.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0300";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0400
 * @tc.name: SUB_backup_b_session_restore_0400
 * @tc.desc: 测试PublishFile接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0400";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0400 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        BFileInfo bFileInfo("", "", 0);
        auto ret = restorePtr_->PublishFile(bFileInfo);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->PublishFile(bFileInfo);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by PublishFile.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0400";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0500
 * @tc.name: SUB_backup_b_session_restore_0500
 * @tc.desc: 测试GetFileHandle接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0500";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0500 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        string bundleName = "";
        string fileName = "";
        auto ret = restorePtr_->GetFileHandle(bundleName, fileName);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->GetFileHandle(bundleName, fileName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by GetFileHandle.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0500";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0600
 * @tc.name: SUB_backup_b_session_restore_0600
 * @tc.desc: 测试AppendBundles接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0600, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0600";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0600 restorePtr_ == nullptr";
            return;
        }
        const string fileName = "1.tar";
        TestManager tm("SUB_backup_b_session_restore_0600");
        string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd remoteCap(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        string bundleName = "";
        vector<string> bundlesToRestore;
        vector<string> detailInfos;
        bundlesToRestore.emplace_back(bundleName);

        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto ret = restorePtr_->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->AppendBundles(move(remoteCap), bundlesToRestore, detailInfos);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by AppendBundles.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0600";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0601
 * @tc.name: SUB_backup_b_session_restore_0601
 * @tc.desc: 测试AppendBundles接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0601, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0601";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0601 restorePtr_ == nullptr";
            return;
        }
        const string fileName = "1.tar";
        TestManager tm("SUB_backup_b_session_restore_0601");
        string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd remoteCap(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        string bundleName = "";
        vector<string> bundlesToRestore;
        bundlesToRestore.emplace_back(bundleName);
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto ret = restorePtr_->AppendBundles(move(remoteCap), bundlesToRestore);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->AppendBundles(move(remoteCap), bundlesToRestore);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by AppendBundles.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0601";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0700
 * @tc.name: SUB_backup_b_session_restore_0700
 * @tc.desc: 测试Finish接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0700, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0700";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0700 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto ret = restorePtr_->Finish();
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->Finish();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0700";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0800
 * @tc.name: SUB_backup_b_session_restore_0800
 * @tc.desc: 测试RegisterBackupServiceDied接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0800, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0800";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0800 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        restorePtr_->RegisterBackupServiceDied(nullptr);
        GTEST_LOG_(INFO) << "GetInstance is true but not equal to parameter";
        SetMockGetInstance(true);
        restorePtr_->RegisterBackupServiceDied(nullptr);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by RegisterBackupServiceDied.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0800";
}

/**
 * @tc.number: SUB_backup_b_session_restore_0900
 * @tc.name: SUB_backup_b_session_restore_0900
 * @tc.desc: 测试析构流程接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_0900, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_0900";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_0900 restorePtr_ == nullptr";
            return;
        }
        SetMockGetInstance(true);
        SetMockLoadSystemAbility(true);
        Init();
        auto restorePtr = BSessionRestore::Init(callbacks_);
        EXPECT_NE(restorePtr, nullptr);

        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        restorePtr = nullptr;

        SetMockGetInstance(true);
        SetMockLoadSystemAbility(true);
        restorePtr = BSessionRestore::Init(callbacks_);
        EXPECT_NE(restorePtr, nullptr);

        GTEST_LOG_(INFO) << "LoadSystemAbility is false";
        SetMockLoadSystemAbility(false);
        restorePtr = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by ~BSessionRestore.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_0900";
}

/**
 * @tc.number: SUB_backup_b_session_restore_1000
 * @tc.name: SUB_backup_b_session_restore_1000
 * @tc.desc: 测试Release接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_1000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_1000";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_1000 restorePtr_ == nullptr";
        return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto ret = restorePtr_->Release();
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->Release();
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Finish.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_1000";
}

/**
 * @tc.number: SUB_backup_b_session_restore_1100
 * @tc.name: SUB_backup_b_session_restore_1100
 * @tc.desc: 测试Cancel接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_1100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_1100";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_1100 restorePtr_ == nullptr";
        return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        BundleName bundleName;
        SetMockGetInstance(false);
        auto ret = restorePtr_->Cancel(bundleName);
        EXPECT_NE(ret, ErrCode(BError::Codes::OK));

        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        ret = restorePtr_->Cancel(bundleName);
        EXPECT_EQ(ret, ErrCode(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by Cancel.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_1100";
}

/**
 * @tc.number: SUB_backup_b_session_restore_1200
 * @tc.name: SUB_backup_b_session_restore_1200
 * @tc.desc: 测试 GetLocalCapabilities 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issuesI9KPRL
 */
HWTEST_F(BSessionRestoreTest, SUB_backup_b_session_restore_1200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_backup_b_session_restore_1200";
    try {
        if (restorePtr_ == nullptr) {
            GTEST_LOG_(INFO) << "SUB_backup_b_session_restore_1200 restorePtr_ == nullptr";
            return;
        }
        GTEST_LOG_(INFO) << "GetInstance is false";
        SetMockGetInstance(false);
        auto err = restorePtr_->GetLocalCapabilities();
        EXPECT_LT(err, 0);
        GTEST_LOG_(INFO) << "GetInstance is true";
        SetMockGetInstance(true);
        err = restorePtr_->GetLocalCapabilities();
        EXPECT_LT(err, 0);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by GetLocalCapabilities.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_backup_b_session_restore_1200";
}

/**
 * @tc.number: SUB_BSessionRestore_GetCompatibilityInfo_0000
 * @tc.name: SUB_BSessionRestore_GetCompatibilityInfo_0000
 * @tc.desc: 测试 GetCompatibilityInfo 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: NA
 */
HWTEST_F(BSessionRestoreTest, SUB_BSessionRestore_GetCompatibilityInfo_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BSessionRestoreTest-begin SUB_BSessionRestore_GetCompatibilityInfo_0000";
    try {
        ASSERT_TRUE(restorePtr_ != nullptr);
        std::string bundleName = "com.example.app";
        std::string extInfo = "";
        std::string compatInfo = "";
        SetMockGetInstance(false);
        auto err = restorePtr_->GetCompatibilityInfo(bundleName, extInfo, compatInfo);
        EXPECT_EQ(err, BError(BError::Codes::SDK_BROKEN_IPC).GetCode());

        SetMockGetInstance(true);
        err = restorePtr_->GetCompatibilityInfo(bundleName, extInfo, compatInfo);
        EXPECT_EQ(err, ERR_OK);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BSessionRestoreTest-an exception occurred by GetCompatibilityInfo.";
    }
    GTEST_LOG_(INFO) << "BSessionRestoreTest-end SUB_BSessionRestore_GetCompatibilityInfo_0000";
}
} // namespace OHOS::FileManagement::Backup