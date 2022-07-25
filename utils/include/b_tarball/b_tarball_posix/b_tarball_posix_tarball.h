/*
* 版权所有 (c) 华为技术有限公司 2022
*/

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H

#include <string>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixTarball {
public:
    void Emplace(const std::string pathName);
    void Publish();
    void Clear();

public:
    BTarballPosixTarball() = default;
    BTarballPosixTarball(const std::string pathTarball);
    ~BTarballPosixTarball() = default;

private:
    UniqueFd fileTarball_ {-1};
    bool isPublished_ {false};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_TARBALL_H