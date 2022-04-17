/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "errors.h"
#include "tools_op.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

int ParseOpAndExecute(int argc, char const *argv[])
{
    for (int i = 1; i < argc; i++) {
        // 暂存 {argv[1]...argv[i]};
        std::vector<string_view> curOp;
        for (int j = 1; j <= i; ++j) {
            curOp.emplace_back(argv[j]);
        }

        // 尝试匹配当前命令，成功后执行
        auto tryOpSucceed = [&curOp](const ToolsOp &op) { return op.TryMatch(curOp); };
        auto &&opeartions = ToolsOp::GetAllOperations();
        auto matchedOp = std::find_if(opeartions.begin(), opeartions.end(), tryOpSucceed);
        if (matchedOp != opeartions.end()) {
            // 暂存 {argv[i + 1]...argv[argc - 1]};
            std::vector<string_view> curArgs;
            for (int j = i + 1; j < argc; ++j) {
                curArgs.emplace_back(argv[j]);
            }

            return matchedOp->Execute(curArgs);
        }
    }
    fprintf(stderr, "Invalid operation\n");
    return -EPERM;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

int main(int argc, char const *argv[])
{
    return OHOS::FileManagement::Backup::ParseOpAndExecute(argc, argv);
}