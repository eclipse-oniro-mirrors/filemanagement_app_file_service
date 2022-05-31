/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_process/b_process.h"

#include <sys/wait.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_process/b_guard_signal.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BProcess::ExcuteCmd(vector<const char *> argv)
{
    argv.push_back(nullptr);

    // 临时将SIGCHLD恢复成默认值，从而能够从作为僵尸进程的子进程中获得返回值
    BGuardSignal guard(SIGCHLD);

    pid_t pid = 0;
    if ((pid = fork()) == 0) {
        exit((execvp(argv[0], const_cast<char **>(argv.data())) == -1) ? errno : 0);
    } else if (pid == -1) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
    } else if (WIFEXITED(status) && WEXITSTATUS(status)) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, to_string(WEXITSTATUS(status)));
    }
}
} // namespace OHOS::FileManagement::Backup