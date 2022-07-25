/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_PAX_HEADER_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_PAX_HEADER_H

#include <sys/stat.h>

#include "b_tarball_posix_header.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixPaxHeader {
public:
    void Publish(const UniqueFd &outFile);

public:
    BTarballPosixPaxHeader() = default;
    BTarballPosixPaxHeader(const std::string &pathName, const struct stat &statInfo);
    ~BTarballPosixPaxHeader() = default;

private:
    BTarballPosixHeader header_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_PAX_HEADER_H