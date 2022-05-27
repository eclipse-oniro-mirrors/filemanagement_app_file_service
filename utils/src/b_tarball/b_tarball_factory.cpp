/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_factory.h"

#include <climits>
#include <cstring>
#include <iostream>
#include <libgen.h>
#include <map>
#include <sstream>
#include <unistd.h>

#include "b_error/b_error.h"
#include "b_tarball/b_tarball_cmdline.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

/**
 * @brief 检验Untar输入参数
 *
 * @param root 用于存储解包文件的根目录
 * 要求输入绝对路径
 */
static void UntarFort(string_view root)
{
    auto resolvedPath = make_unique<char[]>(PATH_MAX);
    if (!realpath(root.data(), resolvedPath.get()) || (string_view(resolvedPath.get()) != root)) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "The root must be an existing canonicalized path");
    }
}

/**
 * @brief 校验Tar输入参数
 *
 * @param tarballDir 打包文件所在目录
 * @param root 文件待打包的根目录
 * 要求输入绝对路径
 * @param includes root中需要打包的路径
 * 要求输入相对路径，除禁止路径穿越外无其余要求，不填默认全部打包
 * @param excludes 待打包路径中无需打包的部分
 * 要求输入相对路径，路径穿越场景无实际意义因此予以禁止。可用于排除部分子目录
 */
static void TarFort(string_view tarballDir,
                    string_view root,
                    vector<string_view> includes,
                    vector<string_view> excludes)
{
    auto resolvedPath = make_unique<char[]>(PATH_MAX);
    if (!realpath(root.data(), resolvedPath.get()) || (string_view(resolvedPath.get()) != root)) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "The root must be an existing canonicalized path");
    }
    if (tarballDir.find_first_of(root) == string::npos) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "The root's path can't be the ancestor of the tarball's");
    }

    auto detectPathTraversal = [&resolvedPath](string_view root, string_view relativePath) {
        stringstream ss;
        ss << root << '/' << relativePath;
        string absPath = ss.str();
        if (char *canoPath = realpath(absPath.c_str(), resolvedPath.get());
            (!canoPath && (errno != ENOENT)) ||
            (canoPath && (string_view(canoPath).find_first_of(root) == string::npos))) {
            throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Dected a path traversal attack");
        }
    };
    for (auto &&include : includes) {
        detectPathTraversal(root, include);
    }
    for (auto &&exclude : excludes) {
        detectPathTraversal(root, exclude);
    }
}

/**
 * @brief 校验tarball路径，并将之拆分为路径和文件名
 *
 * @param tarballPath tarball全路径
 * @return tuple<string, string> 路径和文件名
 */
static tuple<string, string> GetTarballDirAndName(string_view tarballPath)
{
    char *buf4Dir = strdup(tarballPath.data());
    if (!buf4Dir) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Out of memory");
    }
    string tarballDir = dirname(buf4Dir);
    free(buf4Dir);

    char *buf4Name = strdup(tarballPath.data());
    if (!buf4Name) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Out of memory");
    }
    string tarballName = basename(buf4Name);
    free(buf4Name);

    auto resolvedPath = make_unique<char[]>(PATH_MAX);
    if (!realpath(tarballDir.data(), resolvedPath.get())) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, generic_category().message(errno));
    }
    if (auto canonicalizedTarballDir = string_view(resolvedPath.get()); canonicalizedTarballDir != tarballDir) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Tarball path differed after canonicalizing");
    }
    if (auto suffix = string_view(".tar");
        tarballPath.length() <= suffix.length() ||
        !equal(tarballPath.rbegin(), next(tarballPath.rbegin(), suffix.length()), suffix.rbegin(), suffix.rend())) {
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, "Tarball path didn't end with '.tar'");
    }
    return {tarballDir, tarballName};
}

/**
 * @brief 绑定命令行实现的打包器
 *
 * @param tarballDir taball路径
 * @param tarballName taball文件名
 * @return unique_ptr<BTarballFactory::Impl> 打包器实现，包括tar和untar两种方法
 * @see GetTarballDirAndName
 */
static unique_ptr<BTarballFactory::Impl> BindCmdline(string_view tarballDir, string_view tarballName)
{
    auto ptr = make_shared<BTarballCmdline>(tarballDir, tarballName);

    return make_unique<BTarballFactory::Impl>(BTarballFactory::Impl {
        .tar = bind(&BTarballCmdline::Tar, ptr, placeholders::_1, placeholders::_2, placeholders::_3),
        .untar = bind(&BTarballCmdline::Untar, ptr, placeholders::_1),
    });
}

unique_ptr<BTarballFactory::Impl> BTarballFactory::Create(string_view implType, string_view tarballPath)
{
    static map<string_view, function<unique_ptr<BTarballFactory::Impl>(string_view, string_view)>> mapType2Tarball = {
        {"cmdline", BindCmdline},
    };

    try {
        auto [tarballDir, tarballName] = GetTarballDirAndName(tarballPath);
        auto tarballImpl = mapType2Tarball.at(implType)(tarballDir, tarballName);
        if (tarballImpl->tar) {
            tarballImpl->tar = [tarballDir {string(tarballDir)}, tar {tarballImpl->tar}](
                                   string_view root, vector<string_view> includes, vector<string_view> excludes) {
                TarFort(tarballDir, root, includes, excludes);
                tar(root, includes, excludes);
            };
        }
        if (tarballImpl->untar) {
            tarballImpl->untar = [untar {tarballImpl->untar}](string_view root) {
                UntarFort(root);
                untar(root);
            };
        }
        return tarballImpl;
    } catch (const out_of_range &e) {
        stringstream ss;
        ss << "Unsupported implementation " << implType;
        throw BError(BError::Codes::UTILS_INVAL_TARBALL_ARG, ss.str());
    }
}
} // namespace OHOS::FileManagement::Backup