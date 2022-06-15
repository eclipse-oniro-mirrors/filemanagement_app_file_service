/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_session_backup.h"
#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_proxy.h"
#include "service_reverse.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<BSessionBackup> BSessionBackup::Init(UniqueFd remoteCap,
                                                vector<BundleName> bundlesToBackup,
                                                Callbacks callbacks)
{
    try {
        auto backup = make_unique<BSessionBackup>();
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            return nullptr;
        }

        int32_t res = proxy->InitBackupSession(sptr(new ServiceReverse(callbacks)), move(remoteCap), bundlesToBackup);
        if (res != 0) {
            HILOGE("Failed to Backup because of %{public}d", res);
            return nullptr;
        }
        return backup;
    } catch (const exception e) {
        HILOGE("Failed to Backup because of %{public}s", e.what());
    }
    return nullptr;
}

ErrCode BSessionBackup::Start()
{
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        return ErrCode(BError::Codes::SDK_BROKEN_IPC);
    }

    return proxy->Start();
}
} // namespace OHOS::FileManagement::Backup