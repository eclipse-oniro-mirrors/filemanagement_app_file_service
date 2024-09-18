/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "backup_para_mock.h"
#include "bms_adapter_mock.h"
#include "b_jsonutil_mock.h"
#include "ipc_skeleton_mock.h"
#include "sa_backup_connection_mock.h"
#include "service_reverse_proxy_mock.h"
#include "svc_backup_connection_mock.h"
#include "svc_extension_proxy_mock.h"
#include "svc_session_manager_mock.h"

#include "service_incremental.cpp"

namespace OHOS::FileManagement::Backup {
using namespace std;

void Service::OnStart() {}

void Service::OnStop() {}

UniqueFd Service::GetLocalCapabilities()
{
    return UniqueFd(-1);
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force) {}

string Service::VerifyCallerAndGetCallerName()
{
    return "";
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Start()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppFileReady(const string &fileName, UniqueFd fd, int32_t errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppDone(ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::ServiceResultReport(const std::string restoreRetInfo,
    BackupRestoreScenario sennario, ErrCode errCode)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd, const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &detailInfos, RestoreTypeEnum restoreType, int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesRestoreSession(UniqueFd fd,
                                             const std::vector<BundleName> &bundleNames,
                                             RestoreTypeEnum restoreType,
                                             int32_t userId)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesBackupSession(const std::vector<BundleName> &bundleNames)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::AppendBundlesDetailsBackupSession(const std::vector<BundleName> &bundleNames,
    const std::vector<std::string> &bundleInfos)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::Finish()
{
    return BError(BError::Codes::OK);
}

ErrCode Service::LaunchBackupExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::LaunchBackupSAExtension(const BundleName &bundleName)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::GetFileHandle(const string &bundleName, const string &fileName)
{
    return BError(BError::Codes::OK);
}

void Service::OnBackupExtensionDied(const string &&bundleName) {}

void Service::ExtConnectDied(const string &callName) {}

void Service::ExtStart(const string &bundleName) {}

int Service::Dump(int fd, const vector<u16string> &args)
{
    return 0;
}

void Service::ExtConnectFailed(const string &bundleName, ErrCode ret)
{
    GTEST_LOG_(INFO) << "ExtConnectFailed is OK";
}

void Service::ExtConnectDone(string bundleName) {}

void Service::ClearSessionAndSchedInfo(const string &bundleName) {}

void Service::VerifyCaller() {}

void Service::VerifyCaller(IServiceReverse::Scenario scenario) {}

void Service::OnAllBundlesFinished(ErrCode errCode) {}

void Service::OnStartSched() {}

void Service::SendStartAppGalleryNotify(const BundleName &bundleName) {}

void Service::SessionDeactive() {}

ErrCode Service::GetBackupInfo(BundleName &bundleName, std::string &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateTimer(BundleName &bundleName, uint32_t timeOut, bool &result)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::UpdateSendRate(std::string &bundleName, int32_t sendRate, bool &result)
{
    return BError(BError::Codes::OK);
}

void Service::OnSABackup(const std::string &bundleName,
                         const int &fd,
                         const std::string &result,
                         const ErrCode &errCode)
{
}

void Service::OnSARestore(const std::string &bundleName, const std::string &result, const ErrCode &errCode) {}

void Service::SendEndAppGalleryNotify(const BundleName &bundleName)
{
}

void Service::NoticeClientFinish(const string &bundleName, ErrCode errCode)
{
}

ErrCode Service::ReportAppProcessInfo(const std::string processInfo, BackupRestoreScenario sennario)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartExtTimer(bool &isExtStart)
{
    return BError(BError::Codes::OK);
}

ErrCode Service::StartFwkTimer(bool &isFwkStart)
{
    return BError(BError::Codes::OK);
}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    RestoreTypeEnum) {}

void Service::SetCurrentSessProperties(std::vector<BJsonEntityCaps::BundleInfo>&, std::vector<std::string>&,
    std::map<std::string, std::vector<BJsonUtil::BundleDetailInfo>>&, std::map<std::string, bool>&, RestoreTypeEnum) {}

void Service::SetCurrentSessProperties(BJsonEntityCaps::BundleInfo&, std::map<std::string, bool>&) {}

void Service::ReleaseOnException() {}

bool SvcRestoreDepsManager::UpdateToRestoreBundleMap(const string&, const string&)
{
    return true;
}
} // namespace OHOS::FileManagement::Backup

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;
using namespace testing::ext;

constexpr int32_t SERVICE_ID = 5203;

class ServiceIncrementalTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() {};
    void TearDown() {};

    static inline sptr<Service> service = nullptr;
    static inline shared_ptr<BackupParaMock> param = nullptr;
    static inline shared_ptr<BJsonUtilMock> jsonUtil = nullptr;
    static inline shared_ptr<SvcSessionManagerMock> session = nullptr;
    static inline shared_ptr<BundleMgrAdapterMock> bms = nullptr;
    static inline sptr<SvcExtensionProxyMock> svcProxy = nullptr;
    static inline sptr<SvcBackupConnectionMock> connect = nullptr;
    static inline sptr<SABackupConnectionMock> saConnect = nullptr;
    static inline shared_ptr<IPCSkeletonMock> skeleton = nullptr;
    static inline sptr<ServiceReverseProxyMock> srProxy = nullptr;
};

void ServiceIncrementalTest::SetUpTestCase(void)
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
    service = sptr(new Service(SERVICE_ID));
    param = make_shared<BackupParaMock>();
    BackupParaMock::backupPara = param;
    jsonUtil = make_shared<BJsonUtilMock>();
    BJsonUtilMock::jsonUtil = jsonUtil;
    session = make_shared<SvcSessionManagerMock>();
    SvcSessionManagerMock::sessionManager = session;
    svcProxy = sptr(new SvcExtensionProxyMock());
    bms = make_shared<BundleMgrAdapterMock>();
    BundleMgrAdapterMock::bms = bms;
    connect = sptr(new SvcBackupConnectionMock());
    SvcBackupConnectionMock::connect = connect;
    saConnect = sptr(new SABackupConnectionMock());
    SABackupConnectionMock::saConnect = saConnect;
    skeleton = make_shared<IPCSkeletonMock>();
    IPCSkeletonMock::skeleton = skeleton;
    srProxy = sptr(new ServiceReverseProxyMock());
}

void ServiceIncrementalTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
    service = nullptr;
    BackupParaMock::backupPara = nullptr;
    param = nullptr;
    BJsonUtilMock::jsonUtil = nullptr;
    jsonUtil = nullptr;
    SvcSessionManagerMock::sessionManager = nullptr;
    session = nullptr;
    svcProxy = nullptr;
    BundleMgrAdapterMock::bms = nullptr;
    bms = nullptr;
    SvcBackupConnectionMock::connect = nullptr;
    connect = nullptr;
    SABackupConnectionMock::saConnect = nullptr;
    saConnect = nullptr;
    IPCSkeletonMock::skeleton = nullptr;
    skeleton = nullptr;
    srProxy = nullptr;
}

/**
 * @tc.number: SUB_ServiceIncremental_GetUserIdDefault_0000
 * @tc.name: SUB_ServiceIncremental_GetUserIdDefault_0000
 * @tc.desc: 测试 GetUserIdDefault 的正常/异常分支
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: issueIAKC3I
 */
HWTEST_F(ServiceIncrementalTest, SUB_ServiceIncremental_GetUserIdDefault_0000, TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-begin SUB_ServiceIncremental_GetUserIdDefault_0000";
    try {
        EXPECT_CALL(*param, GetBackupDebugOverrideAccount())
            .WillOnce(Return(make_pair<bool, int32_t>(true, DEBUG_ID + 1)));
        auto ret = GetUserIdDefault();
        EXPECT_EQ(ret, DEBUG_ID + 1);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(true, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::SYSTEM_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid()).WillOnce(Return(BConstants::XTS_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, BConstants::DEFAULT_USER_ID);

        EXPECT_CALL(*param, GetBackupDebugOverrideAccount()).WillOnce(Return(make_pair<bool, int32_t>(false, 0)));
        EXPECT_CALL(*skeleton, GetCallingUid())
            .WillOnce(Return(BConstants::SPAN_USERID_UID + BConstants::SPAN_USERID_UID));
        ret = GetUserIdDefault();
        EXPECT_EQ(ret, 2);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceIncrementalTest-an exception occurred by GetUserIdDefault.";
    }
    GTEST_LOG_(INFO) << "ServiceIncrementalTest-end SUB_ServiceIncremental_GetUserIdDefault_0000";
}
}