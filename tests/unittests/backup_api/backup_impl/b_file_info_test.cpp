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

#include "b_file_info.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

namespace {
const string BUNDLE_NAME = "com.example.app2backup";
const string FILE_NAME = "1.tar";
} // namespace

class BFileInfoTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() override {};
    void TearDown() override {};
};

/**
 * @tc.number: SUB_BFile_Info_Marshalling_0100
 * @tc.name: SUB_BFile_Info_Marshalling_0100
 * @tc.desc: Test function of Marshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Marshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Marshalling_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    auto infoPtr = bFileInfo.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);

    // check the data
    EXPECT_EQ(bFileInfo.owner, infoPtr->owner);
    EXPECT_EQ(bFileInfo.fileName, infoPtr->fileName);
    EXPECT_EQ(bFileInfo.sn, infoPtr->sn);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Marshalling_0100";
}

/**
 * @tc.number: SUB_BFile_Info_ReadFromParcel_0100
 * @tc.name: SUB_BFile_Info_ReadFromParcel_0100
 * @tc.desc: Test function of ReadFromParcel interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_ReadFromParcel_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_ReadFromParcel_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);

    // ReadFromParcel
    BFileInfo bFileInfoTemp {"", "", -1};
    bFileInfoTemp.ReadFromParcel(parcel);

    // check the data
    EXPECT_EQ(bFileInfo.owner, bFileInfoTemp.owner);
    EXPECT_EQ(bFileInfo.fileName, bFileInfoTemp.fileName);
    EXPECT_EQ(bFileInfo.sn, bFileInfoTemp.sn);

    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_ReadFromParcel_0100";
}

/**
 * @tc.number: SUB_BFile_Info_Unmarshalling_0100
 * @tc.name: SUB_BFile_Info_Unmarshalling_0100
 * @tc.desc: Test function of Unmarshalling interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0378
 */
HWTEST_F(BFileInfoTest, SUB_BFile_Info_Unmarshalling_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BFileInfoTest-begin SUB_BFile_Info_Unmarshalling_0100";
    BFileInfo bFileInfo(BUNDLE_NAME, FILE_NAME, -1);
    Parcel parcel;
    // marshalling
    EXPECT_EQ(bFileInfo.Marshalling(parcel), true);
    // unmarshalling
    BFileInfo bFileInfoTemp {"", "", -1};
    auto infoPtr = bFileInfoTemp.Unmarshalling(parcel);
    EXPECT_NE(infoPtr, nullptr);

    // check the data
    EXPECT_EQ(bFileInfo.owner, infoPtr->owner);
    EXPECT_EQ(bFileInfo.fileName, infoPtr->fileName);
    EXPECT_EQ(bFileInfo.sn, infoPtr->sn);
    GTEST_LOG_(INFO) << "BFileInfoTest-end SUB_BFile_Info_Unmarshalling_0100";
}
} // namespace OHOS::FileManagement::Backup