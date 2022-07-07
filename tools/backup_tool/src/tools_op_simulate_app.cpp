/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cstdio>
#include <fcntl.h>
#include <string_view>
#include <vector>

#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_usr_config.h"
#include "b_tarball/b_tarball_factory.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static string GenHelpMsg()
{
    return "This operation helps to simulate the behaviour of an app.\n"
           "The command format is as follows :\n"
           "backup_tool simulate app tar user_config.json target.tar root_path\n"
           "backup_tool simulate app untar target.tar root_path";
}

void Tar(ToolsOp::CRefVStrView args)
{
    string_view fileName = args[0];
    string_view tarName = args[1];
    string_view root = args[2];

    BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(UniqueFd(open(fileName.data(), O_RDONLY, 0)));
    auto cache = cachedEntity.Structuralize();
    vector<string> includes = cache.GetIncludes();
    vector<string> excludes = cache.GetExcludes();

    auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
    (tarballFunc->tar)(root, {includes.begin(), includes.end()}, {excludes.begin(), excludes.end()});
}

void Untar(ToolsOp::CRefVStrView args)
{
    string_view tarName = args[0];
    string_view root = args[1];

    auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
    (tarballFunc->untar)(root);
}

static int Exec(ToolsOp::CRefVStrView args)
{
    try {
        if (args.size() == 4 && args[0] == "tar") {
            vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
            Tar(argsWithoutHead);
        } else if (args.size() == 3 && args[0] == "untar") {
            vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
            Untar(argsWithoutHead);
        } else {
            printf("Invalid arguments");
            return -EINVAL;
        }
        return 0;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %s", e.what());
        return -EPERM;
    }
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with
 * side effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"simulate", "app"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup