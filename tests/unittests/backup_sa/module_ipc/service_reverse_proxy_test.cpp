/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "b_error/b_error.h"
#include "b_file_info.h"
#include "iservice_registry.h"
#include "module_ipc/service_reverse_proxy.h"
#include "service_mock.h"
#include "service_reverse_mock.h"
#include "test_manager.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class ServiceReverseProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp();
    void TearDown();
    shared_ptr<ServiceReverseProxy> proxy_ = nullptr;
    sptr<ServiceReverseMock> mock_ = nullptr;
};

void ServiceReverseProxyTest::SetUp()
{
    mock_ = sptr(new ServiceReverseMock());
    proxy_ = make_shared<ServiceReverseProxy>(mock_);
}
void ServiceReverseProxyTest::TearDown()
{
    mock_ = nullptr;
    proxy_ = nullptr;
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnFileReady_0100
 * @tc.desc: Test function of BackupOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->BackupOnFileReady(BUNDLE_NAME, FILE_NAME, fd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnFileReady_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100
 * @tc.desc: Test function of BackupOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100
 * @tc.desc: Test function of BackupOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100
 * @tc.desc: Test function of BackupOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->BackupOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by BackupOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_BackupOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100
 * @tc.desc: Test function of RestoreOnBundleStarted interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnBundleStarted(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleStarted.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleStarted_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100
 * @tc.desc: Test function of RestoreOnBundleFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnBundleFinished(BError(BError::Codes::OK), BUNDLE_NAME);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnBundleFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnBundleFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100
 * @tc.desc: Test function of RestoreOnAllBundlesFinished interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest,
         SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100,
         testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));
        proxy_->RestoreOnAllBundlesFinished(BError(BError::Codes::OK));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnAllBundlesFinished.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnAllBundlesFinished_0100";
}

/**
 * @tc.number: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.name: SUB_ServiceReverse_proxy_RestoreOnFileReady_0100
 * @tc.desc: Test function of RestoreOnFileReady interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(ServiceReverseProxyTest, SUB_ServiceReverse_proxy_RestoreOnFileReady_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-begin SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
    try {
        EXPECT_CALL(*mock_, SendRequest(_, _, _, _))
            .Times(1)
            .WillOnce(Invoke(mock_.GetRefPtr(), &ServiceReverseMock::InvokeSendRequest));

        TestManager tm("ServiceReverseProxyTest_GetFd_0200");
        std::string filePath = tm.GetRootDirCurTest().append(FILE_NAME);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        proxy_->RestoreOnFileReady(BUNDLE_NAME, FILE_NAME, fd);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "ServiceReverseProxyTest-an exception occurred by RestoreOnFileReady.";
    }
    GTEST_LOG_(INFO) << "ServiceReverseProxyTest-end SUB_ServiceReverse_proxy_RestoreOnFileReady_0100";
}
} // namespace OHOS::FileManagement::Backup