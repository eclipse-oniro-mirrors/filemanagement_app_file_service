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

#include "b_session_backup.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static BSessionBackup::Callbacks callbacks_;

BSessionBackup::~BSessionBackup() {}

unique_ptr<BSessionBackup> BSessionBackup::Init(UniqueFd remoteCap,
                                                vector<BundleName> bundlesToBackup,
                                                Callbacks callbacks)
{
    try {
        callbacks_ = callbacks;
        auto backup = make_unique<BSessionBackup>();
        return backup;
    } catch (const exception &e) {
        return nullptr;
    }
    return nullptr;
}

void BSessionBackup::RegisterBackupServiceDied(function<void()> functor)
{
    return;
}

ErrCode BSessionBackup::Start()
{
    callbacks_.onAllBundlesFinished(0);
    callbacks_.onAllBundlesFinished(1);
    callbacks_.onBackupServiceDied();
    callbacks_.onBundleStarted(1, "com.example.app2backup");
    callbacks_.onBundleFinished(1, "com.example.app2backup");
    callbacks_.onBundleStarted(0, "com.example.app2backup");
    BFileInfo bFileInfo("com.example.app2backup", "1.tar", 0);
    TestManager tm("BSessionBackupMock_GetFd_0100");
    string filePath = tm.GetRootDirCurTest().append("1.tar");
    UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
    callbacks_.onFileReady(bFileInfo, move(fd));
    callbacks_.onBundleFinished(0, "com.example.app2backup");
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup