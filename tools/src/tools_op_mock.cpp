/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <cstring>
#include <string>

#include "b_file_info.h"
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
           "path_cap_file fileName appid2...\n";
}

static int32_t MockPublishFile(string_view fileName, ToolsOp::CRefVStrView args)
{
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        printf("Get an empty backup sa proxy");
        return -EFAULT;
    }
    auto res = proxy->GetFileOnServiceEnd();
    TmpFileSN tmpFileSN = get<TmpFileSN>(res);
    fprintf(stderr, "Get Temporary file serial number: %d\n", tmpFileSN);

    UniqueFd fd(move(get<UniqueFd>(res)));
    if (fd < 0) {
        printf("Failed to get fd");
        return -fd;
    }

    BFileInfo fileInfo(args[0].data(),fileName.data(),tmpFileSN);
    return proxy->PublishFile(fileInfo);
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to mock\n");
        return -EINVAL;
    }

    auto front = args.front();
    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());

    return MockPublishFile(front, argsWithoutHead);
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