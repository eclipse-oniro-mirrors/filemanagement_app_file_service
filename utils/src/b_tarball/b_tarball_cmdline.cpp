/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_cmdline.h"

#include <sstream>
#include <string_view>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_process/b_guard_cwd.h"
#include "b_process/b_process.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static bool IsTarFatalErrorOccur(string_view output)
{
    vector<string_view> fatalError {"EOF",          "bad xform",     "bad header", "sparse overflow",
                                    "short header", "empty archive", "Not tar"};
    for (auto &item : fatalError) {
        if (output.find(item) != string_view::npos) {
            return true;
        }
    }
    return false;
}

void BTarballCmdline::Tar(string_view root, vector<string_view> includes, vector<string_view> excludes)
{
    // 切换到根路径，从而在打包时使用文件或目录的相对路径
    BGuardCwd guard(root);

    vector<string_view> argv = {
        "/system/bin/tar",
        "-cf",
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
    auto [bFatalError, errCode] = BProcess::ExecuteCmd(argv, IsTarFatalErrorOccur);
    if (bFatalError || (errCode && access(tarballPath_.data(), F_OK) != 0)) {
        stringstream ss;
        ss << "Is a fatal error occurred: " << bFatalError << ", error code : " << errCode;
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, ss.str());
    }
}

void BTarballCmdline::Untar(string_view root)
{
    vector<string_view> argv = {
        "tar", "-xf", tarballPath_, "-C", root,
    };
    auto [bFatalError, errCode] = BProcess::ExecuteCmd(argv, IsTarFatalErrorOccur);
    if (bFatalError || errCode) {
        throw BError(BError::Codes::UTILS_INVAL_PROCESS_ARG, "fatal error occurred in untar process");
    }
}

BTarballCmdline::BTarballCmdline(string_view tarballDir, string_view tarballName)
    : tarballDir_(tarballDir), tarballName_(tarballName)
{
    tarballPath_ = tarballDir_ + "/" + tarballName_;
}
} // namespace OHOS::FileManagement::Backup
