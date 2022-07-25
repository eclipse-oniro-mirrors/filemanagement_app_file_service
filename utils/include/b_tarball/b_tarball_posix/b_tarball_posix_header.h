/*
* 版权所有 (c) 华为技术有限公司 2022
*/

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
struct BTarballPosixHeader {
    char pathName[BConstants::PATHNAME_MAX_SIZE] {};
    char mode[BConstants::MODE_MAX_SIZE] {};
    char uid[BConstants::UGID_MAX_SIZE] {};
    char gid[BConstants::UGID_MAX_SIZE] {};
    char fileSize[BConstants::FILESIZE_MAX_SIZE] {};
    char mtime[BConstants::TIME_MAX_SIZE] {};
    char chksum[BConstants::CHKSUM_MAX_SIZE] {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
    char typeFlag {};
    char linkName[BConstants::LINKNAME_MAX_SIZE] {};
    char magic[BConstants::MAGIC_SIZE] {"ustar"};
    char version[BConstants::VERSION_SIZE] {'0', '0'};
    char uname[BConstants::UGNAME_MAX_SIZE] {};
    char gname[BConstants::UGNAME_MAX_SIZE] {};
    char devMajor[BConstants::DEV_MAX_SIZE] {};
    char devMinor[BConstants::DEV_MAX_SIZE] {};
    char prefix[BConstants::PREFIX_SIZE] {};
    char padding[BConstants::PADDING_SIZE] {};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_HEADER_H