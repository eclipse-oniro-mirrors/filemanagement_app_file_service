/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <iostream>
#include <sstream>

#include "tools_op.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

static std::string GenHelpMsg()
{
    return "This operation helps to dump the help messages";
}

static int Exec(ToolsOp::CRefVStrView args)
{
    stringstream ss;
    auto &&allOps = ToolsOp::GetAllOperations();
    for (size_t i = 0; i < allOps.size(); ++i) {
        auto desc = allOps[i].GetDescriptor();

        // echo: <op seqs>\n
        ss << '<' << allOps[i].GetName() << '>' << std::endl;

        // echo: help msgs\n\n
        if (desc.funcGenHelpMsg) {
            ss << desc.funcGenHelpMsg() << std::endl;
        }
        if (i != allOps.size() - 1) {
            ss << std::endl;
        }
    }
    printf("%s", ss.str().c_str());
    return 0;
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"help"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS