/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "directory_ex.h"
#include "file_ex.h"

namespace OHOS::FileManagement::Backup {
class BJsonCachedEntityTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_backup_b_json_construction_0100
 * @tc.name: b_json_construction_0100
 * @tc.desc: Test function of  construction interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BJsonCachedEntityTest, b_json_construction_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_construction_0100";
    try {
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open("/data/test/tempone.json", O_RDWR|O_CREAT, 0600)));
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred by construction.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_construction_0100";
}

/**
 * @tc.number: SUB_backup_b_json_Structuralize_0100
 * @tc.name: b_json_Structuralize_0100
 * @tc.desc: Test function of  Structuralize interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BJsonCachedEntityTest, b_json_Structuralize_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_Structuralize_0100";
    try {
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open("/data/test/temptwo.json", O_RDWR|O_CREAT, 0600)));
        jce.Structuralize();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_Structuralize_0100";
}

/**
 * @tc.number: SUB_backup_b_json_Persist_0100
 * @tc.name: b_json_Persist_0100
 * @tc.desc: Test function of  Persist interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BJsonCachedEntityTest, b_json_Persist_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_Persist_0100";
    try {
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open("/data/test/tempthree.json", O_RDWR|O_CREAT, 0600)));
        auto cache = jce.Structuralize();
        uint64_t space = 100;
        cache.SetFreeDiskSpace(space);
        jce.Persist();
        uint64_t result = cache.GetFreeDiskSpace();
        EXPECT_EQ(result, space);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_Persist_0100";
}

/**
 * @tc.number: SUB_backup_b_json_ReloadFromFile_0100
 * @tc.name: b_json_ReloadFromFile_0100
 * @tc.desc: Test function of  ReloadFromFile interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BJsonCachedEntityTest, b_json_ReloadFromFile_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_ReloadFromFile_0100";
    try {
        std::string filePath = "/data/tempfour.json";
        std::string content = "{\"key1\":1,\"key2\":2,\"key3\":3}";
        bool isOk = SaveStringToFile(filePath, content, true);
        EXPECT_TRUE(isOk);
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open("/data/tempfour.json", O_RDWR)));
        auto cache = jce.Structuralize();
        uint64_t space = 100;
        cache.SetFreeDiskSpace(space);
        jce.Persist();
        jce.ReloadFromFile();
        ForceRemoveDirectory(filePath);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_ReloadFromFile_0100";
}

/**
 * @tc.number: SUB_backup_b_json_GetFd_0100
 * @tc.name: b_json_GetFd_0100
 * @tc.desc: Test function of  GetFd interface for SUCCESS.
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 0
 * @tc.require: AR000H0A04
 */
HWTEST_F(BJsonCachedEntityTest, b_json_GetFd_0100, testing::ext::TestSize.Level0)
{
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-begin b_json_GetFd_0100";
    try {
        BJsonCachedEntity<BJsonEntityCaps> jce(UniqueFd(open("/data/test/tempfive.json", O_RDWR|O_CREAT, 0600)));
        jce.GetFd();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BJsonCachedEntityTest-an exception occurred.";
    }
    GTEST_LOG_(INFO) << "BJsonCachedEntityTest-end b_json_GetFd_0100";
}
} // namespace OHOS::FileManagement::Backup