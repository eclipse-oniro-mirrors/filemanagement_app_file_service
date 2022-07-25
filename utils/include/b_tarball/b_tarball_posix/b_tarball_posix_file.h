/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H

#include "b_tarball_posix_extended_data.h"
#include "b_tarball_posix_extended_header.h"
#include "b_tarball_posix_file_data.h"
#include "b_tarball_posix_pax_header.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixFile {
public:
    void Publish(const UniqueFd &outFile);

public:
    BTarballPosixFile() = default;
    BTarballPosixFile(const std::string &pathName);
    ~BTarballPosixFile() = default;

private:
    std::string pathName_;
    UniqueFd file_ {-1};
    BTarballPosixExtendedData extData_;
    BTarballPosixExtendedHeader extHdr_;
    BTarballPosixPaxHeader paxHdr_;
    BTarballPosixFileData fileData_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_H