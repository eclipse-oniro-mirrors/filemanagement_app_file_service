/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
#include "ienhance_service_mock.h"
#include "module_ipc/service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
const string BUNDLE_NAME_DEFAULT = "com.example.default";
const string BUNDLE_NAME_NORMAL = "com.example.normal";
const string BUNDLE_NAME_OTHER = "com.example.other";
constexpr int32_t SERVICE_ID = 5203;
constexpr int32_t USER_ID = 100;
} // namespace

class DefaultAppManagerTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        service_ = sptr(new Service(SERVICE_ID));
    }
    static void TearDownTestCase()
    {
        service_ = nullptr;
    }
    void SetUp() override
    {
        service_->defaultAppManager_->defaultAppMap_.clear();
    }
    void TearDown() override {}

    static inline sptr<Service> service_;
};

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_Add_0100
 * @tc.desc: Verify SetDefaultBundleName with result=true adds bundle to map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_Add_0100, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_Remove_0200
 * @tc.desc: Verify SetDefaultBundleName with result=false removes bundle from map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_Remove_0200, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));

    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_RemoveNonExistent_0300
 * @tc.desc: Verify SetDefaultBundleName with result=false on non-existent bundle does nothing
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_RemoveNonExistent_0300,
    testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_OTHER}, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));
}

/**
 * @tc.name: DefaultAppManagerTest_SetDefaultBundleName_MultipleBundles_0400
 * @tc.desc: Verify SetDefaultBundleName handles multiple bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_SetDefaultBundleName_MultipleBundles_0400,
    testing::ext::TestSize.Level1)
{
    vector<string> bundleNames = {BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER};
    service_->defaultAppManager_->SetDefaultBundleName(bundleNames, false);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));

    service_->defaultAppManager_->SetDefaultBundleName(bundleNames, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_OTHER));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_NonExistent_0500
 * @tc.desc: Verify IsDefaultBundle returns false for bundle not in map
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_NonExistent_0500,
    testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_NORMAL));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_Null_0600
 * @tc.desc: Verify IsDefaultBundle returns false for empty name
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_Null_0600, testing::ext::TestSize.Level1)
{
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(""));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_AllDefault_0700
 * @tc.desc: Verify IsDefaultBundle returns true when all bundles are default
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_AllDefault_0700,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle({BUNDLE_NAME_DEFAULT}));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_NotDefault_0800
 * @tc.desc: Verify IsDefaultBundle returns false when bundle is not default
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_NotDefault_0800,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_OTHER}, true);
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle({BUNDLE_NAME_DEFAULT}));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_MultipleMixed_0900
 * @tc.desc: Verify IsDefaultBundle returns first bundle's result when multiple are passed
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_MultipleMixed_0900,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    vector<string> bundleNames = {BUNDLE_NAME_DEFAULT, BUNDLE_NAME_NORMAL};
    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(bundleNames));
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_EmptyVector_1000
 * @tc.desc: Verify IsDefaultBundle returns false for empty vector
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_EmptyVector_1000,
    testing::ext::TestSize.Level1)
{
    std::vector<std::string> bundles = {};
    EXPECT_FALSE(service_->defaultAppManager_->IsDefaultBundle(bundles));
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_Create_1500
 * @tc.desc: Verify GetMigrateInstance creates new MigrateManager for default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_Create_1500,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_NE(instance, nullptr);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_Reuse_1600
 * @tc.desc: Verify GetMigrateInstance returns same instance for same default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_Reuse_1600,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_EQ(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_DifferentBundle_1700
 * @tc.desc: Verify GetMigrateInstance returns different instances for different default bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_DifferentBundle_1700,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_OTHER, USER_ID);
    EXPECT_NE(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_UserIdChanged_1710
 * @tc.desc: Verify GetMigrateInstance recreates instance when userId changes
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_UserIdChanged_1710,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance1, nullptr);

    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID + 1);
    ASSERT_NE(instance2, nullptr);
    EXPECT_NE(instance1, instance2);
    EXPECT_EQ(instance2->GetUserId(), USER_ID + 1);
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_HasClearedFlag_1800
 * @tc.desc: Verify ClearDefaultAppData sets hasCleared_ flag on MigrateManager
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_HasClearedFlag_1800,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_FALSE(instance->hasCleared_.load());

    service_->defaultAppManager_->ClearDefaultAppData();
    EXPECT_TRUE(instance->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_MultipleBundles_1900
 * @tc.desc: Verify ClearDefaultAppData clears all default bundles
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_MultipleBundles_1900,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_OTHER, USER_ID);

    service_->defaultAppManager_->ClearDefaultAppData();
    EXPECT_TRUE(instance1->hasCleared_.load());
    EXPECT_TRUE(instance2->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_ClearDefaultAppData_Idempotent_2000
 * @tc.desc: Verify ClearDefaultAppData is idempotent (calling twice doesn't crash)
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearDefaultAppData_Idempotent_2000,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);

    EXPECT_TRUE(service_->defaultAppManager_->ClearDefaultAppData());
    EXPECT_TRUE(service_->defaultAppManager_->ClearDefaultAppData());
}

/**
 * @tc.name: DefaultAppManagerTest_IsDefaultBundle_AfterClear_2100
 * @tc.desc: Verify IsDefaultBundle still returns true after ClearDefaultAppData (bundle remains in map)
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_IsDefaultBundle_AfterClear_2100,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    service_->defaultAppManager_->ClearDefaultAppData();

    EXPECT_TRUE(service_->defaultAppManager_->IsDefaultBundle(BUNDLE_NAME_DEFAULT));
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_AfterClear_2200
 * @tc.desc: Verify GetMigrateInstance returns same instance after ClearDefaultAppData
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_AfterClear_2200,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    service_->defaultAppManager_->ClearDefaultAppData();
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    EXPECT_EQ(instance1, instance2);
}

/**
 * @tc.name: DefaultAppManagerTest_GetMigrateInstance_NonDefault_2700
 * @tc.desc: Verify GetMigrateInstance returns nullptr for non-default bundle
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetMigrateInstance_NonDefault_2700,
    testing::ext::TestSize.Level1)
{
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_NORMAL, USER_ID);
    EXPECT_EQ(instance, nullptr);
}

/**
 * @tc.name: DefaultAppManagerTest_GetUserId_2800
 * @tc.desc: Verify GetUserId returns the user id passed to GetMigrateInstance
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_GetUserId_2800, testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance, nullptr);
    EXPECT_EQ(instance->GetUserId(), USER_ID);
}

/**
 * @tc.name: DefaultAppManagerTest_ClearUserInstances_Clear_2900
 * @tc.desc: Verify ClearUserInstances clears instances belonging to the given user
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearUserInstances_Clear_2900,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT, BUNDLE_NAME_OTHER}, true);
    auto instance1 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    auto instance2 = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_OTHER, USER_ID);
    ASSERT_NE(instance1, nullptr);
    ASSERT_NE(instance2, nullptr);

    service_->defaultAppManager_->ClearUserInstances(USER_ID);
    EXPECT_TRUE(instance1->hasCleared_.load());
    EXPECT_TRUE(instance2->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_ClearUserInstances_OtherUser_3000
 * @tc.desc: Verify ClearUserInstances does not clear instances of other users
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_ClearUserInstances_OtherUser_3000,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance, nullptr);

    service_->defaultAppManager_->ClearUserInstances(USER_ID + 1);
    EXPECT_FALSE(instance->hasCleared_.load());
}

/**
 * @tc.name: DefaultAppManagerTest_DestroyDefaultTask_NoService_3100
 * @tc.desc: Verify DestroyDefaultTask returns when enhance service is not loaded
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_DestroyDefaultTask_NoService_3100,
    testing::ext::TestSize.Level1)
{
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance, nullptr);
    instance->DestroyDefaultTask();
}

/**
 * @tc.name: DefaultAppManagerTest_DestroyDefaultTask_WithService_3200
 * @tc.desc: Verify DestroyDefaultTask invokes the enhance service
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_DestroyDefaultTask_WithService_3200,
    testing::ext::TestSize.Level1)
{
    MockIEnhanceService mockEnhanceService;
    EnhanceServiceManager::GetInstance().service_ = &mockEnhanceService;
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance, nullptr);

    EXPECT_CALL(mockEnhanceService, DestroyDefaultTask(BUNDLE_NAME_DEFAULT)).WillOnce(testing::Return(0));
    instance->DestroyDefaultTask();
    EnhanceServiceManager::GetInstance().service_ = nullptr;
}

/**
 * @tc.name: DefaultAppManagerTest_DestroyDefaultTask_Throw_3300
 * @tc.desc: Verify DestroyDefaultTask swallows exceptions from the enhance service
 * @tc.type: FUNC
 */
HWTEST_F(DefaultAppManagerTest, DefaultAppManagerTest_DestroyDefaultTask_Throw_3300,
    testing::ext::TestSize.Level1)
{
    MockIEnhanceService mockEnhanceService;
    EnhanceServiceManager::GetInstance().service_ = &mockEnhanceService;
    service_->defaultAppManager_->SetDefaultBundleName({BUNDLE_NAME_DEFAULT}, true);
    auto instance = service_->defaultAppManager_->GetMigrateInstance(BUNDLE_NAME_DEFAULT, USER_ID);
    ASSERT_NE(instance, nullptr);

    EXPECT_CALL(mockEnhanceService, DestroyDefaultTask(BUNDLE_NAME_DEFAULT)).WillOnce(testing::Throw(42));
    instance->DestroyDefaultTask();
    EnhanceServiceManager::GetInstance().service_ = nullptr;
}
} // namespace OHOS::FileManagement::Backup
