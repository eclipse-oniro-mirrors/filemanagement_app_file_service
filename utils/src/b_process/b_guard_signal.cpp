/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_process/b_guard_signal.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
BGuardSignal::BGuardSignal(int sig)
{
    prevHandler_ = signal(sig, SIG_DFL);
    if (prevHandler_ == SIG_ERR) {
        stringstream ss;
        ss << "Invalid sigal " << sig << ", received error " << system_category().message(errno);
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, ss.str());
    }
}

BGuardSignal::~BGuardSignal()
{
    if (signal(sig_, prevHandler_) == SIG_ERR) {
        HILOGE("Failed to reset sig %{public}d", sig_);
    }
}
} // namespace OHOS::FileManagement::Backup