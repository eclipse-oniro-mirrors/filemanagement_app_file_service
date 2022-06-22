/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_process/b_process.h"

#include <algorithm>
#include <regex>
#include <sys/wait.h>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_process/b_guard_signal.h"
#include "filemgmt_libhilog.h"
#include "securec.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int BProcess::ExecuteCmd(vector<string_view> argvSv)
{
    vector<const char *> argv;
    auto getStringViewData = [](const auto &arg) { return arg.data(); };
    transform(argvSv.begin(), argvSv.end(), back_inserter(argv), getStringViewData);
    argv.push_back(nullptr);

    // 临时将SIGCHLD恢复成默认值，从而能够从作为僵尸进程的子进程中获得返回值
    BGuardSignal guard(SIGCHLD);

    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        throw BError(BError::Codes::UTILS_INTERRUPTED_PROCESS, std::generic_category().message(errno));
    }

    pid_t pid = 0;
    if ((pid = fork()) == 0) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(pipe_fd[0]);
        UniqueFd fd(pipe_fd[1]);
        if (dup2(pipe_fd[1], STDERR_FILENO) == -1) {
            throw BError(BError::Codes::UTILS_INTERRUPTED_PROCESS, std::generic_category().message(errno));
        }
        exit((execvp(argv[0], const_cast<char **>(argv.data())) == -1) ? errno : 0);
    }

    UniqueFd fd(pipe_fd[0]);
    close(pipe_fd[1]);

    if (pid == -1) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
    }

    const int BUF_LEN = 1024;
    auto buf = make_unique<char[]>(BUF_LEN);
    int status = 0;
    do {
        while ((void)memset_s(buf.get(), BUF_LEN, 0, BUF_LEN), read(pipe_fd[0], buf.get(), BUF_LEN - 1) > 0) {
            if (!regex_match(buf.get(), regex("^\\W*$"))) {
                HILOGE("child process output error: %{public}s", buf.get());
            }
        }

        if (waitpid(pid, &status, 0) == -1) {
            throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, std::generic_category().message(errno));
        } else if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // bionic libc++的异常机制存在问题，导致应用在正常的错误下Crash。为确保测试顺利展开，此处暂时屏蔽崩溃错误。
            return EPERM;
        }
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}
} // namespace OHOS::FileManagement::Backup