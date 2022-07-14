/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <iostream>
#include <sstream>

#include "errors.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static std::string GenHelpMsg()
{
    return "\tThis operation helps to check if the backup sa is available.";
}

static int Exec(map<string, vector<string>> mapArgToVal)
{
    auto proxy = ServiceProxy::GetInstance();
    if (!proxy) {
        fprintf(stderr, "Get an empty backup sa proxy\n");
        return -EFAULT;
    }

    printf("successful\n");
    return 0;
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"check", "sa"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup