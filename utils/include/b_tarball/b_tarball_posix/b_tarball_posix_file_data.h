/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H

#include <sys/stat.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixFileData {
public:
    void Publish(const UniqueFd &inFile, const UniqueFd &outFile);

public:
    BTarballPosixFileData() = default;
    BTarballPosixFileData(struct stat &statInfo);
    ~BTarballPosixFileData() = default;

private:
    struct stat stat_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H