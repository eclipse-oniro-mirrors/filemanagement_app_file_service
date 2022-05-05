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
#include "backup_kit.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

static string GenHelpMsg()
{
    return "the functionality of the restore api. Arg list:\n"
           "path_cap_file app_id1 app_id2...";
}

static int32_t GetPathCapFile(string_view pathCapFile, ToolsOp::CRefVStrView args)
{
    std::vector<AppId> appIds;
    for (auto &&id : args) {
        appIds.push_back(id.data());
    }

    auto restore = BSessionRestore::Init(appIds);
    if (restore == nullptr) {
        printf("Failed to init restore");
        return -EPERM;
    }
    UniqueFd fd(restore->GetLocalCapabilities());
    if (fd < 0) {
        printf("Failed to receive fd");
        return fd;
    }
    auto buf = BFile::ReadFile(fd);
    UniqueFd fdfile(open(pathCapFile.data(), O_WRONLY | O_CREAT, S_IRWXU));
    if (fdfile < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    if (write(fdfile, buf.get(), strlen(buf.get())) <= 0) {
        fprintf(stderr, "Failed to write file. error: %d %s\n", errno, strerror(errno));
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
    return GetPathCapFile(args.front(), argsWithoutHead);
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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS