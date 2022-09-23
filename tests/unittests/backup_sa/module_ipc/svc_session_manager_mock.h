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

#ifndef MOCK_SEC_SESSION_MANAGER_MOCK_H
#define MOCK_SEC_SESSION_MANAGER_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>

#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
class SvcSessionManagerMock : public SvcSessionManager {
public:
    SvcSessionManagerMock(wptr<Service> reversePtr) : SvcSessionManager(reversePtr) {};
    virtual ~SvcSessionManagerMock() = default;

    void InvokeInitExtConn(std::map<BundleName, BackupExtInfo> &backupExtNameMap)
    {
        if (backupExtNameMap.empty()) {
            return;
        }

        for (auto &&it : backupExtNameMap) {
            auto callDied = [](const std::string &&bundleName) { return; };

            auto callConnDone = [](const std::string &&bundleName) { return; };

            auto backUpConnection = sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnDone));
            it.second.backUpConnection = backUpConnection;
        }
    }
    MOCK_METHOD1(GetBundleExtNames, void(std::map<std::string, BackupExtInfo> &));
    MOCK_METHOD1(InitClient, void(Impl &));
    MOCK_METHOD1(InitExtConn, void(std::map<BundleName, BackupExtInfo> &));
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SEC_SESSION_MANAGER_MOCK_H