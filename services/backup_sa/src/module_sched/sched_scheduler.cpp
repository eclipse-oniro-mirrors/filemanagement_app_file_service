/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_sched/sched_scheduler.h"

#include <cstdint>
#include <tuple>
#include <utility>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "module_ipc/service.h"
#include "module_ipc/svc_session_manager.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void SchedScheduler::QueueGetFileRequest(const string &bundleName, const string &fileName)
{
    shared_lock lock(lock_);
    getFileRequests_.emplace_back(make_tuple(bundleName, fileName));
}

void SchedScheduler::Sched(const string &bundName)
{
    shared_lock lock(lock_);
    auto revPtrStrong = reversePtr_.promote();
    if (!revPtrStrong) {
        throw BError(BError::Codes::SA_INVAL_ARG, "It's curious that the backup sa dies before the backup client");
    }
    if (!revPtrStrong->TryExtConnect(bundName)) {
        HILOGI("wait for extension connect %{public}s", bundName.data());
        return;
    }
    int size = static_cast<int>(getFileRequests_.size());
    for (int i = size - 1; i >= 0; i--) {
        auto [bundleName, fileName] = getFileRequests_[i];
        if (bundName == bundleName) {
            auto task = [reversePtr {reversePtr_}, bundleName {bundleName}, fileName {fileName}]() {
                auto revPtrStrong = reversePtr.promote();
                if (!revPtrStrong) {
                    throw BError(BError::Codes::SA_INVAL_ARG,
                                 "It's curious that the backup sa dies before the backup client");
                }
                revPtrStrong->GetFileHandle(bundleName, fileName);
            };
            WorkQueue([task]() {
                try {
                    task();
                } catch (const BError &e) {
                    HILOGE("%{public}s", e.what());
                } catch (const exception &e) {
                    HILOGE("%{public}s", e.what());
                } catch (...) {
                    HILOGE("");
                }
            });
            getFileRequests_.erase(getFileRequests_.begin() + i);
        }
    }
}

void SchedScheduler::WorkQueue(const function<void()> &task)
{
    threadPool_.AddTask(task);
}

void SchedScheduler::QueueSetExtBundleName(const string &bundleName, const string &backupExtName)
{
    shared_lock lock(extLock_);
    extBundleName_.emplace_back(make_tuple(bundleName, backupExtName));
}

void SchedScheduler::SchedConn()
{
    shared_lock lock(extLock_);
    if (extBundleName_.empty()) {
        return;
    }
    if (extStartName_.size() >= BConstants::EXT_CONNECT_MAX_COUNT) {
        return;
    }
    for (auto iter = extBundleName_.begin(); iter != extBundleName_.end();) {
        if (extStartName_.size() >= BConstants::EXT_CONNECT_MAX_COUNT) {
            break;
        }
        auto it = *iter;
        auto [bundleName, backupExtName] = it;
        extStartName_.insert(bundleName);

        auto task = [reversePtr {reversePtr_}, bundleName {bundleName}, backupExtName {backupExtName}]() {
            HILOGE("begin start bundleName = %{public}s", bundleName.data());
            ErrCode ret = reversePtr->LaunchBackupExtension(bundleName, backupExtName);
            if (ret) {
                HILOGE("bundleName Extension Died = %{public}s , ret = %{public}d", bundleName.data(), ret);
                reversePtr->OnBackupExtensionDied(move(bundleName), ret);
            }
        };
        WorkQueue([task]() {
            try {
                task();
            } catch (const BError &e) {
                HILOGE("%{public}s", e.what());
            } catch (const exception &e) {
                HILOGE("%{public}s", e.what());
            } catch (...) {
                HILOGE("");
            }
        });

        iter = extBundleName_.erase(iter);
    }
}

void SchedScheduler::ExtStart(string bundleName)
{
    HILOGE("begin");
    auto task = [reversePtr {reversePtr_}, bundleName]() {
        auto revPtrStrong = reversePtr.promote();
        if (!revPtrStrong) {
            throw BError(BError::Codes::SA_INVAL_ARG, "It's curious that the backup sa dies before the backup client");
        }
        if (!revPtrStrong->TryExtConnect(bundleName)) {
            HILOGI("wait for extension connect %{public}s", bundleName.data());
            throw BError(BError::Codes::SA_INVAL_ARG);
        }
        revPtrStrong->ExtStart(bundleName);
    };

    WorkQueue([task]() {
        try {
            task();
        } catch (const BError &e) {
            HILOGE("%{public}s", e.what());
        } catch (const exception &e) {
            HILOGE("%{public}s", e.what());
        } catch (...) {
            HILOGE("");
        }
    });
}

void SchedScheduler::RemoveExtConn(const string &bundleName)
{
    shared_lock lock(extLock_);
    extStartName_.erase(bundleName);
    if (extStartName_.size() < BConstants::EXT_CONNECT_MAX_COUNT) {
        SchedConn();
    }
}
}; // namespace OHOS::FileManagement::Backup