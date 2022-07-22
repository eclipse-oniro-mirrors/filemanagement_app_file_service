/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_session_restore.h"
#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<BSessionRestore> BSessionRestore::Init(std::vector<BundleName> bundlesToRestore, Callbacks callbacks)
{
    HILOGI("Begin");
    try {
        auto restore = make_unique<BSessionRestore>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            return nullptr;
        }
        int32_t res = proxy->InitRestoreSession(new ServiceReverse(callbacks), bundlesToRestore);
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

tuple<ErrCode, TmpFileSN, UniqueFd> BSessionRestore::GetFileOnServiceEnd(string &bundleName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return {BError(BError::Codes::SDK_BROKEN_IPC), 0, UniqueFd(-1)};
    }
    return proxy->GetFileOnServiceEnd(bundleName);
}

ErrCode BSessionRestore::PublishFile(BFileInfo fileInfo)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return ErrCode(BError::Codes::SDK_BROKEN_IPC);
    }
    return proxy->PublishFile(fileInfo);
}

ErrCode BSessionRestore::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC);
    }

    return proxy->Start();
}

ErrCode BSessionRestore::GetExtFileName(string &bundleName, string &fileName)
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return BError(BError::Codes::SDK_BROKEN_IPC, "Failed to proxy because of is empty").GetCode();
    }

    return proxy->GetExtFileName(bundleName, fileName);
}
} // namespace OHOS::FileManagement::Backup