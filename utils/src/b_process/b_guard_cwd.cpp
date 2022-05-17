/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_process/b_guard_cwd.h"
#include "b_error/b_error.h"

#include <cstdlib>
#include <unistd.h>

namespace OHOS::FileManagement::Backup {
using namespace std;

BGuardCwd::BGuardCwd(std::string_view tgtDir)
{
    pwd_ = getcwd(nullptr, 0);
    if (!pwd_) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, "Out of memory");
    }
    if (chdir(tgtDir.data())) {
        std::free(pwd_);
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
    }
}

BGuardCwd::~BGuardCwd()
{
    chdir(pwd_);
    free(pwd_);
}
} // namespace OHOS::FileManagement::Backup
