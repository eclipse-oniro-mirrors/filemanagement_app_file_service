/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_session_restore.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

unique_ptr<BSessionRestore> BSessionRestore::Init(std::vector<AppId> appsToRestore, Callbacks callbacks)
{
    HILOGI("Begin");
    try {
        auto restore = make_unique<BSessionRestore>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacks), appsToRestore);
        if (res != 0) {
            HILOGE("Failed to Restore because of %{public}d", res);
            return nullptr;
        }
        return restore;
    } catch (const exception e) {
        HILOGE("Failed to Restore because of %{public}s", e.what());
    }
    return nullptr;
}

UniqueFd BSessionRestore::GetLocalCapabilities()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return UniqueFd(-1);
    }
    return UniqueFd(proxy->GetLocalCapabilities());
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS