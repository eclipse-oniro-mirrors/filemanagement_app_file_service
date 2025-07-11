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
#include <gtest/gtest.h>

#include <singleton.h>
#include <string>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "access_token_error.h"
#include "accesstoken_kit.h"
#include "file_share.cpp"
#include "file_share.h"
#include "ipc_skeleton.h"
#include "log.h"
#include "sandbox_helper.h"
#include "uri.h"

namespace {
using namespace std;
using namespace OHOS::AppFileService;
using namespace OHOS::Security::AccessToken;

const int E_OK = 0;
const string BUNDLE_A = "com.example.filesharea";

class FileShareTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

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
    string file = "/data/app/el2/" + to_string(uid) + "/base/" + bundleNameA + "/files/test.txt";
    int32_t fd = open(file.c_str(), O_RDWR | O_CREAT);
    ASSERT_TRUE(fd != -1) << "FileShareTest Create File Failed!";
    close(fd);
    string uri = "file://" + bundleNameA + "/data/storage/el2/base/files/test.txt";

    string bundleNameB = "com.example.fileshareb";
    uint32_t tokenId = AccessTokenKit::GetHapTokenID(uid, bundleNameB, 0);

    int32_t flag = 3;
    vector<string> uriList(1, uri);
    vector<int32_t> retLists;
    int32_t ret = FileShare::CreateShareFile(uriList, tokenId, flag, retLists);
    EXPECT_EQ(ret, E_OK);

    vector<string> sharePathLists;
    string uriErr = "file://" + bundleNameA + "/data/storage/el2/base/files/abc/../test.txt";
    sharePathLists.push_back(uriErr);
    ret = FileShare::DeleteShareFile(tokenId, sharePathLists);
    EXPECT_EQ(ret, E_OK);

    string sharePath = "/data/service/el2/" + to_string(uid) + "/share/" + bundleNameB + "/rw/" + bundleNameA +
                       "/data/storage/el2/base/files/test.txt";
    ret = access(sharePath.c_str(), F_OK);
    EXPECT_EQ(ret, E_OK);

    sharePathLists.push_back(uri);
    ret = FileShare::DeleteShareFile(tokenId, sharePathLists);
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
    std::string strPrefix = "file://media/";
    std::string fileUri = "Photo/12/IMG_12345_999999/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(strPrefix + SandboxHelper::Encode(fileUri), "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/575/IMG_12345_999999.jpg");

    std::string fileUri2 = "Photo/12/IMG_12345_999999/test .jpg";
    std::string physicalPath2;
    ret = SandboxHelper::GetPhysicalPath(strPrefix + SandboxHelper::Encode(fileUri2), "100", physicalPath2);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath2, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/575/IMG_12345_999999.jpg");
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
 * @tc.name: File_share_GetPhysicalPath_0007
 * @tc.desc: Test function of GetPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalPath_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0007";
    std::string fileUri0 = "file://docs/storage/Users/currentUser/../test../test1";
    std::string fileUri1 = "file://docs/storage/Users/currentUser/test../../test";
    std::string fileUri2 = "file://docs/storage/Users/currentUser/test../../";
    std::string fileUri3 = "file://docs/storage/Users/currentUser/test../test../..";
    std::string fileUri4 = "file://docs/storage/Users/currentUser/test/..test/..";
    std::string fileUri5 = "file://docs/storage/Users/currentUser/test/test../test";
    std::string fileUri6 = "file://docs/storage/Users/currentUser/test../test../test";
    std::string fileUri7 = "file://docs/storage/Users/currentUser/test../test../test../..test";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetPhysicalPath(fileUri0, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetPhysicalPath(fileUri1, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetPhysicalPath(fileUri2, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetPhysicalPath(fileUri3, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetPhysicalPath(fileUri4, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetPhysicalPath(fileUri5, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    ret = SandboxHelper::GetPhysicalPath(fileUri6, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    ret = SandboxHelper::GetPhysicalPath(fileUri7, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalPath_0007";
}

/**
 * @tc.name: File_share_GetMediaSharePath_0001
 * @tc.desc: Test function of GetMediaSharePath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetMediaSharePath_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetMediaSharePath_0001";
    std::string fileUri = "file://media/Photo/test/IMG_12345_999999/test.jpg";
    vector<string> uriList;
    vector<string> physicalPathList;
    uriList.push_back(fileUri);
    int32_t ret = SandboxHelper::GetMediaSharePath(uriList, physicalPathList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetMediaSharePath_0001";
}

/**
 * @tc.name: File_share_GetMediaSharePath_0002
 * @tc.desc: Test function of GetMediaSharePath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetMediaSharePath_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetMediaSharePath_0002";
    std::string fileUri = "file://media/Photo";
    vector<string> uriList;
    vector<string> physicalPathList;
    uriList.push_back(fileUri);
    int32_t ret = SandboxHelper::GetMediaSharePath(uriList, physicalPathList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetMediaSharePath_0002";
}

/**
 * @tc.name: File_share_GetMediaSharePath_0003
 * @tc.desc: Test function of GetMediaSharePath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetMediaSharePath_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetMediaSharePath_0003";
    std::string fileUri = "file://media/Photo/test/IMG_12345_/test.JPG";
    std::string fileUri1 = "file://media/Photo/test/IMG_12345_/test.JPG";
    vector<string> uriList;
    vector<string> physicalPathList;
    uriList.push_back(fileUri);
    uriList.push_back(fileUri1);
    int32_t ret = SandboxHelper::GetMediaSharePath(uriList, physicalPathList);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetMediaSharePath_0003";
}

/**
 * @tc.name: File_share_CheckValidPath_0001
 * @tc.desc: Test function of CheckValidPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_CheckValidPath_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_CheckValidPath_0001";
    std::string filePath1 = "";
    auto ret = SandboxHelper::CheckValidPath(filePath1);
    EXPECT_FALSE(ret);

    std::string filePath2(PATH_MAX, 't');
    ret = SandboxHelper::CheckValidPath(filePath2);
    EXPECT_FALSE(ret);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_CheckValidPath_0001";
}

/**
 * @tc.name: File_share_IsValidPath_0002
 * @tc.desc: Test function of CheckValidPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_IsValidPath_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_IsValidPath_0002";
    bool result = SandboxHelper::IsValidPath("../test../test1");
    EXPECT_FALSE(result);
    result = SandboxHelper::IsValidPath("/../test../test1");
    EXPECT_FALSE(result);
    result = SandboxHelper::IsValidPath("test../../test");
    EXPECT_FALSE(result);
    result = SandboxHelper::IsValidPath("test../../");
    EXPECT_FALSE(result);
    result = SandboxHelper::IsValidPath("test../test../..");
    EXPECT_FALSE(result);
    result = SandboxHelper::IsValidPath("/test/..test/..");
    EXPECT_FALSE(result);

    result = SandboxHelper::IsValidPath("test");
    EXPECT_TRUE(result);
    result = SandboxHelper::IsValidPath("/test/test../test");
    EXPECT_TRUE(result);
    result = SandboxHelper::IsValidPath("/test../test../test");
    EXPECT_TRUE(result);
    result = SandboxHelper::IsValidPath("/test../test../test../");
    EXPECT_TRUE(result);
    result = SandboxHelper::IsValidPath("/test../test../test../..test");
    EXPECT_TRUE(result);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_IsValidPath_0002";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0001
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetBackupPhysicalPath_0001";
    std::string fileUri = "file://media/Photo/12/IMG_12345_0011/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/11/IMG_12345_0011.jpg");
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0001";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0002
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetBackupPhysicalPath_0002";
    std::string fileUri = "file://media/Photo/12/IMG_12345_/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0002";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0003
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetBackupPhysicalPath_0003";
    std::string fileUri = "file://media/Photo/12/IMG_12345_a0011/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0003";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0004
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetBackupPhysicalPath_0004";
    std::string fileUri = "file://media/Photo/IMG_12345_999999/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0004";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0005
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetBackupPhysicalPath_0005";
    std::string strPrefix = "file://media/";
    std::string fileUri = "Photo/12/IMG_12345_999999/test.jpg";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(strPrefix + SandboxHelper::Encode(fileUri), "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/575/IMG_12345_999999.jpg");

    std::string fileUri2 = "Photo/12/IMG_12345_999999/test .jpg";
    std::string physicalPath2;
    ret = SandboxHelper::GetBackupPhysicalPath(strPrefix + SandboxHelper::Encode(fileUri2), "100", physicalPath2);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalPath2, "/mnt/hmdfs/100/account/cloud_merge_view/files/Photo/575/IMG_12345_999999.jpg");
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0005";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0006
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0006, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0006";
    std::string fileUri = "file://media/Photo/12/IMG_12345_999999/test.jpg/other";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0006";
}

/**
 * @tc.name: File_share_GetBackupPhysicalPath_0007
 * @tc.desc: Test function of GetBackupPhysicalPath() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetBackupPhysicalPath_0007, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalPath_0007";
    std::string fileUri0 = "file://docs/storage/Users/currentUser/../test../test1";
    std::string fileUri1 = "file://docs/storage/Users/currentUser/test../../test";
    std::string fileUri2 = "file://docs/storage/Users/currentUser/test../../";
    std::string fileUri3 = "file://docs/storage/Users/currentUser/test../test../..";
    std::string fileUri4 = "file://docs/storage/Users/currentUser/test/..test/..";
    std::string fileUri5 = "file://docs/storage/Users/currentUser/test/test../test";
    std::string fileUri6 = "file://docs/storage/Users/currentUser/test../test../test";
    std::string fileUri7 = "file://docs/storage/Users/currentUser/test../test../test../..test";
    std::string physicalPath;
    int32_t ret = SandboxHelper::GetBackupPhysicalPath(fileUri0, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri1, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri2, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri3, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri4, "100", physicalPath);
    EXPECT_EQ(ret, -EINVAL);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri5, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri6, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    ret = SandboxHelper::GetBackupPhysicalPath(fileUri7, "100", physicalPath);
    EXPECT_EQ(ret, E_OK);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetBackupPhysicalPath_0007";
}

/**
 * @tc.name: File_share_GetNetworkIdFromUri_001
 * @tc.desc: Test function of GetNetworkIdFromUri() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetNetworkIdFromUri_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetNetworkIdFromUri_001";
    std::string fileUri = "file://docs/storage/Users/currentUser/test.jpg?networkid=***";
    std::string result = "***";
    std::string networkId;
    SandboxHelper::GetNetworkIdFromUri(fileUri, networkId);
    EXPECT_EQ(result, networkId);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetNetworkIdFromUri_001";
}

/**
 * @tc.name: File_share_GetNetworkIdFromUri_002
 * @tc.desc: Test function of GetNetworkIdFromUri() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetNetworkIdFromUri_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetNetworkIdFromUri_002";
    std::string fileUri = "file://docs/storage/Users/currentUser/test.jpg?networkid=";
    std::string result = "";
    std::string networkId;
    SandboxHelper::GetNetworkIdFromUri(fileUri, networkId);
    EXPECT_EQ(result, networkId);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetNetworkIdFromUri_002";
}

/**
 * @tc.name: File_share_GetNetworkIdFromUri_003
 * @tc.desc: Test function of GetNetworkIdFromUri() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetNetworkIdFromUri_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetNetworkIdFromUri_003";
    std::string fileUri = "file://docs/storage/Users/currentUser/test.jpg?networkid=123456&789";
    std::string result = "123456";
    std::string networkId;
    SandboxHelper::GetNetworkIdFromUri(fileUri, networkId);
    EXPECT_EQ(result, networkId);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetNetworkIdFromUri_003";
}

/**
 * @tc.name: File_share_GetNetworkIdFromUri_004
 * @tc.desc: Test function of GetNetworkIdFromUri() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetNetworkIdFromUri_004, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetNetworkIdFromUri_004";
    std::string fileUri = "file://docs/storage/Users/currentUser/test.jpg?networkid=123456/test/1.txt";
    std::string result = "";
    std::string networkId;
    SandboxHelper::GetNetworkIdFromUri(fileUri, networkId);
    EXPECT_EQ(result, networkId);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetNetworkIdFromUri_004";
}

/**
 * @tc.name: File_share_GetNetworkIdFromUri_005
 * @tc.desc: Test function of GetNetworkIdFromUri() interface for FAILURE.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetNetworkIdFromUri_005, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetNetworkIdFromUri_005";
    std::string fileUri = "file://docs/storage/Users/currentUser/test.jpg?query=123456789&?networkid=123456";
    std::string result = "123456";
    std::string networkId;
    SandboxHelper::GetNetworkIdFromUri(fileUri, networkId);
    EXPECT_EQ(result, networkId);
    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetNetworkIdFromUri_005";
}

/**
 * @tc.name: File_share_GetDocsDir_001
 * @tc.desc: Test function of GetDocsDir()
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetDocsDir_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetDocsDir_001";
    std::string uri = "file://docs/storage/Users/currentUser/test.jpg?networkid=123456";
    FileShareInfo info;
    info.providerBundleName_ = "docs";
    info.targetBundleName_ = "com.demo.a";
    info.providerLowerPath_ = "/mnt/hmdfs/100/account/device_view/123456/files/docs/test.jpg";
    info.currentUid_ = "100";

    int32_t ret = GetDocsDir(uri, info);
    EXPECT_EQ(ret, -EINVAL);

    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetDocsDir_001";
}

/**
 * @tc.name: File_share_GetDocsDir_002
 * @tc.desc: Test function of GetDocsDir()
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetDocsDir_002, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetDocsDir_002";
    std::string uri = "file://docs/storage/Users/currentUser/test.jpg";
    FileShareInfo info;
    info.providerBundleName_ = "docs";
    info.targetBundleName_ = "com.demo.a";
    info.providerLowerPath_ = "/mnt/hmdfs/100/account/device_view/123456/files/docs/test.jpg";
    info.currentUid_ = "100";

    int32_t ret = GetDocsDir(uri, info);
    EXPECT_EQ(ret, -EINVAL);

    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetDocsDir_002";
}

/**
 * @tc.name: File_share_GetDocsDir_003
 * @tc.desc: Test function of GetDocsDir()
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetDocsDir_003, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetDocsDir_003";
    std::string uri = "/mnt/hmdfs/100/account/device_view";
    FileShareInfo info;
    info.providerBundleName_ = "docs";
    info.targetBundleName_ = "com.demo.a";
    info.providerLowerPath_ = "/mnt/hmdfs/100/account/device_view/123456/files/docs/test.jpg";
    info.currentUid_ = "100";

    int32_t ret = GetDocsDir(uri, info);
    EXPECT_EQ(ret, -EINVAL);

    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetDocsDir_003";
}

/**
 * @tc.name: File_share_GetLowerDir_001
 * @tc.desc: Test function of GetLowerDir()
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetLowerDir_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetLowerDir_001";
    std::string lowerPathHead = "/data/app/el5/<currentUserId>/database/<PackageName>/<networkId>";
    std::string usrId = "100";
    std::string bundleName = "com.demo.a";
    std::string networkId = "123456";

    std::string ret = SandboxHelper::GetLowerDir(lowerPathHead, usrId, bundleName, networkId);
    EXPECT_EQ(ret, "/data/app/el5/100/database/com.demo.a/123456");

    lowerPathHead = "/data/app/el5";
    ret = SandboxHelper::GetLowerDir(lowerPathHead, usrId, bundleName, networkId);
    EXPECT_EQ(ret, lowerPathHead);

    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetLowerDir_001";
}

/**
 * @tc.name: File_share_GetPhysicalDir_001
 * @tc.desc: Test function of GetPhysicalDir()
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I7PDZL
 */
HWTEST_F(FileShareTest, File_share_GetPhysicalDir_001, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "FileShareTest-begin File_share_GetPhysicalDir_001";
    std::string uri = "file://com.demo.a/data/storage/el2/distributedfiles/remote_share.txt";
    std::string usrId = "100";
    std::string physicalDir;

    int32_t ret = SandboxHelper::GetPhysicalDir(uri, usrId, physicalDir);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(physicalDir, "/mnt/hmdfs/100/account/device_view/local/data/com.demo.a/");

    uri = "file://com.demo.a/data/storage/el2/distributedfiles/../remote_share.txt";
    ret = SandboxHelper::GetPhysicalDir(uri, usrId, physicalDir);
    EXPECT_EQ(ret, -EINVAL);

    uri = "file://media/data/storage/el2/distributedfiles/remote_share.txt";
    ret = SandboxHelper::GetPhysicalDir(uri, usrId, physicalDir);
    EXPECT_EQ(ret, -EINVAL);

    uri = "file://com.demo.a/data/storage/el12/distributedfiles/remote_share.txt";
    ret = SandboxHelper::GetPhysicalDir(uri, usrId, physicalDir);
    EXPECT_EQ(ret, -EINVAL);

    GTEST_LOG_(INFO) << "FileShareTest-end File_share_GetPhysicalDir_001";
}

} // namespace