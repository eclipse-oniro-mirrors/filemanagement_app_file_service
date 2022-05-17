/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H

#include <string>
#include <string_view>
#include <vector>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BTarballCmdline final : protected NoCopyable {
public:
    void Tar(std::string_view root, std::vector<std::string_view> includes, std::vector<std::string_view> excludes);
    void Untar(std::string_view root);

public:
    BTarballCmdline(std::string_view tarballDir, std::string_view tarballName);

private:
    std::string tarballDir_;
    std::string tarballName_;
    std::string tarballPath_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H
