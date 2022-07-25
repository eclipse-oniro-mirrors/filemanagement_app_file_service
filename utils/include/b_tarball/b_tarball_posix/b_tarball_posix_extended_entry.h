/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H

#include <optional>
#include <sys/stat.h>

#include "b_resources/b_constants.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixExtendedEntry {
public:
    static std::optional<BTarballPosixExtendedEntry> TryToGetEntry(BConstants::EntryKey entryKey,
                                                                   const std::string &pathName,
                                                                   const struct stat &statInfo);

public:
    size_t GetEntrySize();
    std::string GetEntry();
    void Publish(const UniqueFd &outFile);

public:
    ~BTarballPosixExtendedEntry() = default;

private:
    size_t entrySize_ {0};
    std::string entryName_;
    std::string entryValue_;
    std::string entry_;

private:
    BTarballPosixExtendedEntry(const std::string &entryName, const std::string &entryValue);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_ENTRY_H