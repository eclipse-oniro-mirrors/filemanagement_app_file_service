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

#include "module_ipc/svc_session_manager.h"

#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>

#include "ability_util.h"
#include "b_resources/b_constants.h"
#include "ext_extension_mock.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

namespace {
constexpr int SCHED_NUM = 1;
}

void SvcSessionManager::VerifyCallerAndScenario(uint32_t clientToken, IServiceReverse::Scenario scenario) const
{
    return;
}

void SvcSessionManager::Active(Impl newImpl)
{
    extConnectNum_ = 0;
    impl_ = newImpl;
}

void SvcSessionManager::Deactive(const wptr<IRemoteObject> &remoteInAction, bool force)
{
    return;
}

void SvcSessionManager::VerifyBundleName(string &bundleName)
{
    return;
}

sptr<IServiceReverse> SvcSessionManager::GetServiceReverseProxy()
{
    return impl_.clientProxy;
}

IServiceReverse::Scenario SvcSessionManager::GetScenario()
{
    return impl_.scenario;
}

void SvcSessionManager::GetBundleExtNames(map<BundleName, BackupExtInfo> &backupExtNameMap) {}

bool SvcSessionManager::OnBunleFileReady(const string &bundleName, const string &fileName)
{
    return true;
}

UniqueFd SvcSessionManager::OnBunleExtManageInfo(const string &bundleName, UniqueFd fd)
{
    return UniqueFd(-1);
}

void SvcSessionManager::RemoveExtInfo(const string &bundleName)
{
    return;
}

wptr<SvcBackupConnection> SvcSessionManager::GetExtConnection(const BundleName &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return nullptr;
    }
    if (!it->second.backUpConnection) {
        auto callDied = [](const string &&bundleName) {};
        auto callConnDone = [](const string &&bundleName) {};
        it->second.backUpConnection = sptr<SvcBackupConnection>(new SvcBackupConnection(callDied, callConnDone));
        sptr<BackupExtExtensionMock> mock = sptr(new BackupExtExtensionMock());
        it->second.backUpConnection->OnAbilityConnectDone({}, mock->AsObject(), 0);
    }
    return wptr(it->second.backUpConnection);
}

void SvcSessionManager::InitExtConn(std::map<BundleName, BackupExtInfo> &backupExtNameMap)
{
    return;
}

void SvcSessionManager::DumpInfo(const int fd, const std::vector<std::u16string> &args)
{
    return;
}

void SvcSessionManager::InitClient(Impl &newImpl)
{
    return;
}

void SvcSessionManager::SetExtFileNameRequest(const string &bundleName, const string &fileName)
{
    return;
}

std::set<std::string> SvcSessionManager::GetExtFileNameRequest(const std::string &bundleName)
{
    std::set<std::string> fileNameInfo;
    return fileNameInfo;
}

map<BundleName, BackupExtInfo>::iterator SvcSessionManager::GetBackupExtNameMap(const string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    return it;
}

bool SvcSessionManager::GetSchedBundleName(string &bundleName)
{
    if (extConnectNum_ == 0) {
        GTEST_LOG_(INFO) << "GetSchedBundleName is zero";
        extConnectNum_++;
        return false;
    } else if (extConnectNum_ == SCHED_NUM || extConnectNum_ == (SCHED_NUM + 1)) {
        GTEST_LOG_(INFO) << "GetSchedBundleName is one two";
        bundleName = "com.example.app2backup";
        extConnectNum_++;
        return true;
    }

    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return false;
    }
    it->second.backupExtName = bundleName;
    GTEST_LOG_(INFO) << "GetSchedBundleName is " << it->second.backupExtName;
    return true;
}

BConstants::ServiceSchedAction SvcSessionManager::GetServiceSchedAction(const std::string &bundleName)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return BConstants::ServiceSchedAction::WAIT;
    }
    GTEST_LOG_(INFO) << "GetServiceSchedAction is " << it->second.schedAction;
    return it->second.schedAction;
}

void SvcSessionManager::SetServiceSchedAction(const string &bundleName, BConstants::ServiceSchedAction action)
{
    auto it = impl_.backupExtNameMap.find(bundleName);
    if (it == impl_.backupExtNameMap.end()) {
        return;
    }
    it->second.schedAction = action;
}

string SvcSessionManager::GetBackupExtName(const string &bundleName)
{
    return "";
}
} // namespace OHOS::FileManagement::Backup