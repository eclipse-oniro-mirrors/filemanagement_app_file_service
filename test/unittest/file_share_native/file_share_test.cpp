/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include <cassert>
#include <fcntl.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "access_token_error.h"
#include "accesstoken_kit.h"
#include "file_permission.h"
#include "file_share.h"
#include "filemgmt_libn.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "parameter.h"
#include "sandbox_helper.h"
#include "uri.h"

namespace {
using namespace std;
using namespace OHOS::AppFileService;
using namespace OHOS::Security::AccessToken;

const int E_OK = 0;
const string bundleA = "com.example.filesharea";

class FileShareTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};
bool CheckFileManagerFullMountEnable()
{
    char value[FULL_MOUNT_ENABLE_SIZE] = "false";
    int retSystem = GetParameter(FULL_MOUNT_ENABLE_PARAMETER, "false", value, FULL_MOUNT_ENABLE_SIZE);
    if (retSystem > 0 && !strcmp(value, "true")) {
        return true;
    }
    return false;
}

/**
 * @tc.name: file_share_test_0000
 * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_CreateShareFile_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0000";

    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, E_OK);

    uriList.clear();
    retList.clear();
    uri.clear();
    uri = "file://" + bundleNameA + "/data/storage/el2/base/files/../files/test.txt";
    uriList.push_back(uri);
    ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0000";
}

/**
 * @tc.name: file_share_test_0001
 * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_CreateShareFile_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0001";

    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "file://" + bundleNameA + "/data/test/el2/base/files/test.txt";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0001";
}

/**
 * @tc.name: file_share_test_0002
 * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_CreateShareFile_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0002";
    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
    uint32_t tokenId = 100;

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, static_cast<int32_t>(AccessTokenError::ERR_TOKENID_NOT_EXIST));
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0002";
}

/**
 * @tc.name: file_share_test_0003
 * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_CreateShareFile_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0003";
    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0003";
}

/**
 * @tc.name: file_share_test_0004
 * @tc.desc: Test function of CreateShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_CreateShareFile_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CreateShareFile_0004";
    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 4;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CreateShareFile_0004";
}

/**
 * @tc.name: file_share_test_0005
 * @tc.desc: Test function of DeleteShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_DeleteShareFile_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_DeleteShareFile_0005";
    int32_t uid = 100;

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);
    vector<string> sharePathList;
    string bundleNameA = "com.example.filesharea";
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
    sharePathList.push_back(uri);
    int32_t ret = FileShare::DeleteShareFile(tokenId, sharePathList);
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_DeleteShareFile_0005";
}

/**
 * @tc.name: file_share_test_0006
 * @tc.desc: Test function of DeleteShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_DeleteShareFile_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_DeleteShareFile_0006";
    uint32_t tokenId = 104;
    vector<string> sharePathList;
    string bundleNameA = "com.example.filesharea";
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";
    sharePathList.push_back(uri);
    int32_t ret = FileShare::DeleteShareFile(tokenId, sharePathList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_DeleteShareFile_0006";
}

/**
 * @tc.name: File_share_DeleteShareFile_0007
 * @tc.desc: Test function of DeleteShareFile() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H63TL
 */
HWTEST_F(FileShareTest, File_share_DeleteShareFile_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_DeleteShareFile_0007";
    int32_t uid = 100;

    string bundleNameA = "com.example.filesharea";
    string fileStr = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(fileStr.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retList;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retList);
    EXPECT_EQ(ret, E_OK);

    vector<string> sharePathList;
    string uriErr = "file://" + bundleNameA + "/data/storage/el2/base/files/abc/../test.txt";
    sharePathList.push_back(uriErr);
    ret = FileShare::DeleteShareFile(tokenId, sharePathList);
    EXPECT_EQ(ret, E_OK);

    string sharePath = "/data/service/el2/" + to_string(uid) + "/share/" + bundleNameB + "/rw/" + bundleNameA +
                       "/data/storage/el2/base/files/test.txt";
    ret = access(sharePath.c_str(), F_OK);
    EXPECT_EQ(ret, E_OK);

    sharePathList.push_back(uri);
    ret = FileShare::DeleteShareFile(tokenId, sharePathList);
    EXPECT_EQ(ret, E_OK);

    ret = access(sharePath.c_str(), F_OK);
    EXPECT_EQ(ret, -1);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_DeleteShareFile_0007";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0001
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0001";
    std::string fileUri = "file://media/Photo/12/IMG_12345_0011/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/11/IMG_12345_0011.jpg");
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0001";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0002
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0002";
    std::string fileUri = "file://media/Photo/12/IMG_12345_/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0002";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0003
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0003";
    std::string fileUri = "file://media/Photo/12/IMG_12345_a0011/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0003";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0004
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0004";
    std::string fileUri = "file://media/Photo/IMG_12345_999999/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0004";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0005
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0005";
    std::string fileUri = "file://media/Photo/12/IMG_12345_999999/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/575/IMG_12345_999999.jpg");
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0005";
}

/**
 * @tc.name: File_share_GetPhysicalPath_0006
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0006";
    std::string fileUri = "file://media/Photo/12/IMG_12345_999999/test.jpg/other";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0006";
}

/**
 * @tc.name: GrantPermission_test_0000
 * @tc.desc: Test function of GrantPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GrantPermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin GrantPermission";
    int32_t targetCode = 0;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string uri = "file://" + bundleA + "/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = uri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::GrantPermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end GrantPermission";
}

/**
 * @tc.name: GrantPermission_test_0001
 * @tc.desc: Test function of GrantPermission() interface for error URI.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, GrantPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin GrantPermission_test_0001";
    int32_t targetCode = EPERM;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string inexistentUri = "file://" + bundleA + "/data/storage/el2/base/files/inexistentTest.txt";
    string mediaUri = "file://media/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = inexistentUri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    uriPolicyInfo.uri = mediaUri;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::GrantPermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end GrantPermission_test_0001";
}

/**
 * @tc.name: PersistPermission_test_0000
 * @tc.desc: Test function of PersistPermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin PersistPermission_test_0000";
    int32_t targetCode = 0;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string uri = "file://" + bundleA + "/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = uri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::PersistPermission(uriPolicies);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end PersistPermission_test_0000";
}

/**
 * @tc.name: PersistPermission_test_0001
 * @tc.desc: Test function of PersistPermission() interface for error URI.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, PersistPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin PersistPermission_test_0001";
    int32_t targetCode = EPERM;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string inexistentUri = "file://" + bundleA + "/data/storage/el2/base/files/inexistentTest.txt";
    string mediaUri = "file://media/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = inexistentUri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    uriPolicyInfo.uri = mediaUri;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::PersistPermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end PersistPermission_test_0001";
}

/**
 * @tc.name: DesistPersistedPermission_test_0000
 * @tc.desc: Test function of RevokePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DesistPersistedPermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin DesistPersistedPermission_test_0000";
    int32_t targetCode = 0;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string uri = "file://" + bundleA + "/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = uri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::RevokePermission(uriPolicies);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end DesistPersistedPermission_test_0000";
}

/**
 * @tc.name: DesistPersistedPermission_test_0001
 * @tc.desc: Test function of RevokePermission() interface for error URI.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DesistPersistedPermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin DesistPersistedPermission_test_0001";
    int32_t targetCode = EPERM;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string inexistentUri = "file://" + bundleA + "/data/storage/el2/base/files/inexistentTest.txt";
    string mediaUri = "file://media/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = inexistentUri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    uriPolicyInfo.uri = mediaUri;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::RevokePermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end DesistPersistedPermission_test_0001";
}

/**
 * @tc.name: ActivatePermission_test_0000
 * @tc.desc: Test function of ActivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin ActivatePermission_test_0000";
    int32_t targetCode = 0;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string uri = "file://" + bundleA + "/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = uri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::ActivatePermission(uriPolicies);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end ActivatePermission_test_0000";
}

/**
 * @tc.name: ActivatePermission_test_0001
 * @tc.desc: Test function of ActivatePermission() interface for error URI.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, ActivatePermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin ActivatePermission_test_0001";
    int32_t targetCode = EPERM;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string inexistentUri = "file://" + bundleA + "/data/storage/el2/base/files/inexistentTest.txt";
    string mediaUri = "file://media/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = inexistentUri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    uriPolicyInfo.uri = mediaUri;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::ActivatePermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end ActivatePermission_test_0001";
}

/**
 * @tc.name: DeactivatePermission_test_0000
 * @tc.desc: Test function of DeactivatePermission() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_0000, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin DeactivatePermission_test_0000";
    int32_t targetCode = 0;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string uri = "file://" + bundleA + "/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = uri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::DeactivatePermission(uriPolicies);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end DeactivatePermission_test_0000";
}

/**
 * @tc.name: DeactivatePermission_test_0001
 * @tc.desc: Test function of DeactivatePermission() interface for error URI.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require:
 */
HWTEST_F(FilePermissionTest, DeactivatePermission_test_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FilePermissionTest-begin DeactivatePermission_test_0001";
    int32_t targetCode = EPERM;
    if (!CheckFileManagerFullMountEnable()) {
        targetCode = E_DEVICENOTSUPPORT;
    }
    string inexistentUri = "file://" + bundleA + "/data/storage/el2/base/files/inexistentTest.txt";
    string mediaUri = "file://media/data/storage/el2/base";
    int32_t tokenId = 0;
    std::vector<UriPolicyInfo> uriPolicies;
    UriPolicyInfo uriPolicyInfo;
    uriPolicyInfo.uri = inexistentUri;
    uint32_t policyFlag = OperateMode::READ_MODE | OperateMode::WRITE_MODE;
    uriPolicies.emplace_back(uriPolicyInfo);
    uriPolicyInfo.uri = mediaUri;
    uriPolicies.emplace_back(uriPolicyInfo);
    int32_t ret = FilePermission::DeactivatePermission(tokenId, uriPolicies, policyFlag);
    EXPECT_EQ(ret, targetCode);
    GTEST_LOG_(INFO) << "FilePermissionTest-end DeactivatePermission_test_0001";
}
} // namespace