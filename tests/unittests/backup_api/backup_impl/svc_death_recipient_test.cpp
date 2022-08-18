/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <gtest/gtest.h>

#include "iservice_registry.h"
#include "service_reverse_mock.h"
#include "svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

const string FILE_NAME = "temp.json";

class SvcDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp();
    void TearDown() {};
};

void SvcDeathRecipientTest::SetUp() {}

void CallBack(const wptr<IRemoteObject> &obj)
{
    printf("SvcSessionManagerTest-CallBack success\n");
}

/**
 * @tc.number: SUB_backup_sa_deathecipient_OnRemoteDied_0100
 * @tc.name: SUB_backup_sa_deathecipient_OnRemoteDied_0100
 * @tc.desc: Test function of OnRemoteDied interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: SR000H0378
 */
HWTEST_F(SvcDeathRecipientTest, SUB_backup_sa_deathecipient_OnRemoteDied_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-begin SUB_backup_sa_deathecipient_OnRemoteDied_0100";
    try {
        sptr<ServiceReverseMock> remote = sptr(new ServiceReverseMock());
        sptr<SvcDeathRecipient> deathRecipient = sptr(new SvcDeathRecipient(CallBack));
        remote->AddDeathRecipient(deathRecipient);
        deathRecipient->OnRemoteDied(remote);
        remote = nullptr;
        deathRecipient = nullptr;
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "SvcSessionManagerTest-an exception occurred by OnRemoteDied.";
    }
    GTEST_LOG_(INFO) << "SvcSessionManagerTest-end SUB_backup_sa_deathecipient_OnRemoteDied_0100";
}
} // namespace OHOS::FileManagement::Backup