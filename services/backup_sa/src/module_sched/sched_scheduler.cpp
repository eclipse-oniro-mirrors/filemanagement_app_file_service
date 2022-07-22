/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_sched/sched_scheduler.h"

#include <tuple>
#include <utility>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
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
    if (!session_.TryExtConnect(bundName)) {
        HILOGI("wait for extension connect %{public}s", bundName.data());
        return;
    }
    for (int i = getFileRequests_.size() - 1; i >= 0; i--) {
        auto [bundleName, fileName] = getFileRequests_[i];
        if (bundName == bundleName) {
            WorkQueue(bundleName, fileName);
            getFileRequests_.erase(getFileRequests_.begin() + i);
        }
    }
}

void SchedScheduler::WorkQueue(string bundleName, string fileName)
{
    auto task = [sched {this}, bundleName, fileName]() { sched->GetFileHandle(bundleName, fileName); };

    // REM: 这里异步化了，需要做并发控制
    threadPool_.AddTask([task]() {
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

void SchedScheduler::GetFileHandle(const string &bundleName, const string &fileName)
{
    session_.GetFileHandle(bundleName, fileName);
}
}; // namespace OHOS::FileManagement::Backup