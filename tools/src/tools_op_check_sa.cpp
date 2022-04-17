/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <iostream>
#include <sstream>

#include "errors.h"
#include "i_service.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "tools_op.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

static std::string GenHelpMsg()
{
    return "This operation helps to check if the backup sa is available";
}

static int Exec(ToolsOp::CRefVStrView args)
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        fprintf(stderr, "Get an empty samgr\n");
        return -EFAULT;
    }

    auto remote = samgr->GetSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
    if (!remote) {
        fprintf(stderr, "Get an empty backup sa\n");
        return -EFAULT;
    }

    auto proxy = iface_cast<Backup::IService>(remote);
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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS