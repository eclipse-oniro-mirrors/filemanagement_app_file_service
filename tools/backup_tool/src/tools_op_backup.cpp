/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_filesystem/b_file.h"
#include "backup_kit_inner.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static string GenHelpMsg()
{
    return "the functionality of the backup api. Arg list:\n"
           "path_cap_file bundleName1 bundleName2...";
}

static int32_t InitPathCapFile(string_view pathCapFile, ToolsOp::CRefVStrView args)
{
    std::vector<BundleName> bundleNames;
    for (auto &&bundleName : args) {
        bundleNames.emplace_back(bundleName.data());
    }

    UniqueFd fd(open(pathCapFile.data(), O_RDONLY));
    if (fd < 0) {
        fprintf(stderr, "Failed to open file error: %d %s\n", errno, strerror(errno));
        return -errno;
    }

    auto backup = BSessionBackup::Init(move(fd), bundleNames, {});
    if (backup == nullptr) {
        printf("Failed to init backup");
        return -EPERM;
    }
    return 0;
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to backup\n");
        return -EINVAL;
    }

    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
    return InitPathCapFile(args.front(), argsWithoutHead);
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"backup"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup