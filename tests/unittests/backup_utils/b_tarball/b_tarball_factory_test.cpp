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
#include <gtest/gtest.h>

#include "b_tarball_factory.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class BTarballFactoryTest : public testing::Test {
public:
    static void SetUpTestCase() {};
    static void TearDownTestCase() {};
    void SetUp() {};
    void TearDown() {};
};

/**
 * @tc.number: SUB_b_tarball_factory_0100
 * @tc.name: b_tarball_factory_0100
 * @tc.desc: 测试BTarballFactory类create函数是否成功
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: SR000H0379
 */
HWTEST_F(BTarballFactoryTest, b_tarball_factory_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "BTarballFactoryTest-begin b_tarball_factory_0100";
    try {
        // 预置路径
        TestManager tm("b_tarball_factory_0100");
        string root = tm.GetRootDirCurTest();
        string_view implType = "cmdline";
        string_view tarballPath = root + "/test.tar";

        // 调用create获取打包解包能力
        auto impl = BTarballFactory::Create(implType, tarballPath);

        // 判断是否获取打包解包能力成功
        if (!impl) {
            ASSERT_TRUE(false);
        }
        if (!impl->tar) {
            EXPECT_TRUE(false);
        }
        if (!impl->untar) {
            EXPECT_TRUE(false);
        }
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "BTarballFactoryTest-an exception occurred by BTarballFactory.";
    }
    GTEST_LOG_(INFO) << "BTarballFactoryTest-end b_tarball_factory_0100";
}
} // namespace OHOS::FileManagement::Backup