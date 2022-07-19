/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "tools_op.h"

#include <regex>
#include <sstream>

namespace OHOS::FileManagement::Backup {
using namespace std;

const std::string ToolsOp::GetName() const
{
    std::stringstream ss;

    auto &&allSubOps = desc_.opName;
    for (size_t j = 0; j < allSubOps.size(); ++j) {
        ss << allSubOps[j];
        if (j != allSubOps.size() - 1) {
            ss << ' ';
        }
    }

    return ss.str();
}

bool ToolsOp::Register(ToolsOp &&op)
{
    auto &&opName = op.GetDescriptor().opName;
    auto isIncorrect = [&opName](const std::string_view &subOp) {
        std::vector<std::string> patterns {
            "\\W", // 匹配任意不是字母、数字、下划线的字符
            "^$",  // 匹配空串
        };

        for (auto subOp : opName) {
            for (auto pattern : patterns) {
                std::regex re(pattern);
                if (std::regex_search(string(subOp), re)) {
                    fprintf(stderr, "Sub-op '%s' failed to pass regex '%s'\n", subOp.data(), pattern.c_str());
                    return true;
                }
            }
        }

        return false;
    };
    if (std::any_of(opName.begin(), opName.end(), isIncorrect)) {
        fprintf(stderr, "Failed to register an illegal operation '%s'\n", op.GetName().c_str());
        return false;
    }

    ToolsOp::opsAvailable_.emplace_back(std::move(op));

    // sort with ascending order
    std::sort(opsAvailable_.begin(), opsAvailable_.end(), [](const ToolsOp &lop, const ToolsOp &rop) {
        return lop.desc_.opName < rop.desc_.opName;
    });
    return true;
}

bool ToolsOp::TryMatch(CRefVStrView op) const
{
    return op == desc_.opName;
}

int ToolsOp::Execute(map<string, vector<string>> args) const
{
    if (!desc_.funcExec) {
        fprintf(stderr, "Incomplete operation: executor is missing\n");
        return -EPERM;
    }
    return desc_.funcExec(args);
}
} // namespace OHOS::FileManagement::Backup
