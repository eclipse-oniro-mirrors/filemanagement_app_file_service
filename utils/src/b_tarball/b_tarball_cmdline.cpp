/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_cmdline.h"

#include <string_view>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_process/b_guard_cwd.h"
#include "b_process/b_process.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BTarballCmdline::Tar(string_view root, vector<string_view> includes, vector<string_view> excludes)
{
    // 切换到根路径，从而在打包时使用文件或目录的相对路径
    BGuardCwd guard(root);

    vector<string_view> argv = {
        "/system/bin/tar",
        "-cvf",
        tarballPath_,
    };

    if (includes.empty()) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "tar includes argument must be not empty");
    }

    for (auto &&include : includes) {
        argv.push_back(include);
    }
    for (auto &&exclude : excludes) {
        argv.push_back("--exclude");
        argv.push_back(exclude);
    }

    // 如果打包后生成了打包文件，则默认打包器打包时生成的错误可以忽略(比如打包一个不存在的文件)
    if (int err = BProcess::ExecuteCmd(argv); (err && access(tarballPath_.data(), F_OK) != 0)) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, to_string(err));
    }
}

void BTarballCmdline::Untar(string_view root)
{
    BProcess::ExecuteCmd({
        "tar",
        "-xvf",
        tarballPath_,
        "-C",
        root,
    });
}

BTarballCmdline::BTarballCmdline(string_view tarballDir, string_view tarballName)
    : tarballDir_(tarballDir), tarballName_(tarballName)
{
    tarballPath_ = tarballDir_ + "/" + tarballName_;
}
} // namespace OHOS::FileManagement::Backup
