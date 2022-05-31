/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_filesystem/b_file.h"
#include "backup_kit.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static string GenHelpMsg()
{
    return "the functionality of the restore api. Arg list:\n"
           "path_cap_file bundleName1 bundleName2...";
}

static int32_t Init(string_view pathCapFile, ToolsOp::CRefVStrView args)
{
    std::vector<BundleName> bundleNames;
    for (auto &&bundleName : args) {
        bundleNames.emplace_back(bundleName.data());
    }

    auto restore = BSessionRestore::Init(bundleNames, {});
    if (restore == nullptr) {
        printf("Failed to init restore");
        return -EPERM;
    }
    UniqueFd fdRemote(restore->GetLocalCapabilities());
    if (fdRemote < 0) {
        printf("Failed to receive fd");
        return fdRemote;
    }
    if (lseek(fdRemote, 0, SEEK_SET) == -1) {
        fprintf(stderr, "Failed to lseek. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    struct stat stat = {};
    if (fstat(fdRemote, &stat) == -1) {
        fprintf(stderr, "Failed to fstat. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    UniqueFd fdLocal(open(pathCapFile.data(), O_WRONLY | O_CREAT, S_IRWXU));
    if (fdLocal < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    if (sendfile(fdLocal, fdRemote, nullptr, stat.st_size) == -1) {
        fprintf(stderr, "Failed to Copy file. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }

    return 0;
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to restore\n");
        return -EINVAL;
    }
    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
    return Init(args.front(), argsWithoutHead);
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"restore"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup