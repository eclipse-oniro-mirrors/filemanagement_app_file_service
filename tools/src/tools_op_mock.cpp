/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <cstring>
#include <string>

#include "b_filesystem/b_file.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

static string GenHelpMsg()
{
    return "This operation helps to mock APIs provided by the backup SA. Supported APIs: \n"
           "GetLocalCapabilities appid1 appid2...\n";
}

static int32_t MockGetLocalCapabilities(ToolsOp::CRefVStrView args)
{
    std::vector<AppId> appIds;
    for (auto &&id : args) {
        appIds.push_back(id.data());
    }

    auto proxy = ServiceProxy::GetInstance();
    int32_t res = proxy->InitRestoreSession(appIds);
    if (res != 0) {
        fprintf(stderr, "Failed to mock GetLocalCapabilities because of %d\n", res);
        return res;
    }

    UniqueFd fd(proxy->GetLocalCapabilities());
    auto buf = BFile::ReadFile(fd);
    printf("Get JSonfile, whose content reads:\n%s\n", buf.get());
    return 0;
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to mock\n");
        return -EINVAL;
    }

    auto front = args.front();
    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
    if (front == "GetLocalCapabilities") {
        return MockGetLocalCapabilities(argsWithoutHead);
    }

    fprintf(stderr, "Mock unknown API %s\n", front.data());
    return -EINVAL;
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"mock"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS