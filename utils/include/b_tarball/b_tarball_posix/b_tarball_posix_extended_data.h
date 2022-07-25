/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_DATA_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_DATA_H

#include <vector>

#include "b_tarball_posix_extended_entry.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixExtendedData {
public:
    size_t GetEntriesSize();
    void Publish(const UniqueFd &outFile);

public:
    BTarballPosixExtendedData() = default;
    BTarballPosixExtendedData(const std::string &pathName, const struct stat &statInfo);
    ~BTarballPosixExtendedData() = default;

private:
    size_t entriesSize_ {0};
    std::vector<BTarballPosixExtendedEntry> entries_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_DATA_H