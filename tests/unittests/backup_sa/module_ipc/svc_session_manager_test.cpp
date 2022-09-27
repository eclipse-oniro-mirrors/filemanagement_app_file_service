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
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

#include "b_error/b_error.h"
#include "i_service_reverse.h"
#include "module_ipc/svc_session_manager.h"
#include "service_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace testing;
using namespace std;

class SvcSessionManagerMock : public SvcSessionManager {
public:
    explicit SvcSessionManagerMock(sptr<ServiceMock> reversePtr) : SvcSessionManager(reversePtr) {};
    ~SvcSessionManagerMock() = default;
    MOCK_METHOD1(GetBundleExtNames, void(std::map<string, BackupExtInfo> &));
    MOCK_METHOD1(InitExtConn, void(std::map<string, BackupExtInfo> &));
    MOCK_METHOD1(InitClient, void(Impl &));
};

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string MANAGE_JSON = "manage.json";
const string FILE_NAME = "1.tar";

constexpr int32_t CLIENT_TOKEN_ID = 100;

map<string, BackupExtInfo> g_backupExtNameMap;
sptr<ServiceMock> g_serviceMock = sptr<ServiceMock>(new ServiceMock());
} // namespace

class SvcSessionManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() override;
    void TearDown() override {};
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
}

/**
 * @tc.number: SUB_backup_sa_session_verifycaller_0100
 * @tc.name: SUB_backup_sa_session_verifycaller_0100
 * @tc.desc: Test function of VerifyCaller interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_verifycaller_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_verifycaller_0100";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::RESTORE,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        sessionManager.VerifyCaller(CLIENT_TOKEN_ID, IServiceReverse::Scenario::RESTORE);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by verifycaller.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_verifycaller_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_getscenario_0100
 * @tc.name: SUB_backup_sa_session_getscenario_0100
 * @tc.desc: Test function of GetScenario interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getscenario_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getscenario_0100";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::RESTORE,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        IServiceReverse::Scenario scenario = sessionManager.GetScenario();
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
 * @tc.desc: Test function of OnBunleFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0100";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::RESTORE,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        bool condition = sessionManager.OnBunleFileReady(BUNDLE_NAME);
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
 * @tc.desc: Test function of OnBunleFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0200, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0200";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::BACKUP,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        bool condition = sessionManager.OnBunleFileReady(BUNDLE_NAME, MANAGE_JSON);
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
 * @tc.desc: Test function of OnBunleFileReady interface for FAILED.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_onbunlefileready_0300, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_onbunlefileready_0300";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::BACKUP,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        bool condition = sessionManager.OnBunleFileReady(BUNDLE_NAME, FILE_NAME);
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
 * @tc.desc: Test function of RemoveExtInfo interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: 0
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_removeextinfo_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_removeextinfo_0100";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::BACKUP,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        sessionManager.RemoveExtInfo(BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by RemoveExtInfo.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_removeextinfo_0100";
}

/**
 * @tc.number: SUB_backup_sa_session_getbackupextnamevec_0100
 * @tc.name: SUB_backup_sa_session_getbackupextnamevec_0100
 * @tc.desc: Test function of GetBackupExtNameVec interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: 0
 */
HWTEST_F(SvcSessionManagerTest, SUB_backup_sa_session_getbackupextnamevec_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_session_getbackupextnamevec_0100";
    try {
        SvcSessionManagerMock sessionManager(g_serviceMock);
        EXPECT_CALL(sessionManager, GetBundleExtNames(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitExtConn(_)).WillOnce(Return());
        EXPECT_CALL(sessionManager, InitClient(_)).WillOnce(Return());
        sessionManager.Active(
            {.clientToken = CLIENT_TOKEN_ID,
            .scenario = IServiceReverse::Scenario::BACKUP,
            .backupExtNameMap = move(g_backupExtNameMap),
        });
        vector<pair<string, string>> extNameVec;
        sessionManager.GetBackupExtNameVec(extNameVec);
        for (auto [bundleName, backupExtName] : extNameVec) {
            EXPECT_EQ(bundleName, BUNDLE_NAME);
            EXPECT_EQ(backupExtName, BUNDLE_NAME);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by GetBackupExtNameVec.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_session_getbackupextnamevec_0100";
}
} // namespace OHOS::FileManagement::Backup