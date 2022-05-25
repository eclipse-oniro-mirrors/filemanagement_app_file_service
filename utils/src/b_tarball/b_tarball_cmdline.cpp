/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_cmdline.h"

#include "b_process/b_guard_cwd.h"
#include "b_process/b_process.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BTarballCmdline::Tar(string_view root, vector<string_view> includes, vector<string_view> excludes)
{
    // 切换到根路径，从而在打包时使用文件或目录的相对路径
    BGuardCwd guard(root);

    vector<const char *> argv = {
        "/system/bin/tar",
        "-cvf",
        tarballPath_.data(),
    };

    for (auto &&include : includes) {
        argv.push_back(include.data());
    }
    for (auto &&exclude : excludes) {
        argv.push_back("--exclude");
        argv.push_back(exclude.data());
    }

    BProcess::ExcuteCmd(argv);
}

void BTarballCmdline::Untar(string_view root)
{
    BProcess::ExcuteCmd({
        "tar",
        "-xvf",
        tarballPath_.data(),
        "-C",
        root.data(),
    });
}

BTarballCmdline::BTarballCmdline(string_view tarballDir, string_view tarballName)
    : tarballDir_(tarballDir), tarballName_(tarballName)
{
    tarballPath_ = tarballDir_ + "/" + tarballName_;
}
} // namespace OHOS::FileManagement::Backup
