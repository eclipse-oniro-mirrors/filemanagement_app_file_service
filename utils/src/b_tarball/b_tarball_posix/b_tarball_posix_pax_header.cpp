/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_pax_header.h"

#include <cstdio>
#include <grp.h>
#include <pwd.h>
#include <sys/sysmacros.h>

#include "b_resources/b_constants.h"
#include "securec.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

static inline unsigned int CalculateChksum(const char *byteBlock, int blockSize)
{
    unsigned int chksum = 0U;
    for (int i = 0; i < blockSize; ++i) {
        chksum += 0xFF & *(byteBlock + i);
    }
    return chksum;
}

BTarballPosixPaxHeader::BTarballPosixPaxHeader(const string &pathName, const struct stat &statInfo)
{
    string tmpPathName = pathName.front() == '/' ? pathName.substr(1) : pathName;
    strncpy_s(header_.pathName, BConstants::PATHNAME_MAX_SIZE, tmpPathName.c_str(), BConstants::PATHNAME_MAX_SIZE - 1);
    (void)snprintf_s(header_.mode, BConstants::MODE_MAX_SIZE, BConstants::MODE_MAX_SIZE - 1, "%o", statInfo.st_mode);
    (void)snprintf_s(header_.uid, BConstants::UGID_MAX_SIZE, BConstants::UGID_MAX_SIZE - 1, "%o", statInfo.st_uid);
    (void)snprintf_s(header_.gid, BConstants::UGID_MAX_SIZE, BConstants::UGID_MAX_SIZE - 1, "%o", statInfo.st_gid);
    if (statInfo.st_size <= BConstants::FILESIZE_MAX) {
        (void)snprintf_s(header_.fileSize, BConstants::FILESIZE_MAX_SIZE, BConstants::FILESIZE_MAX_SIZE - 1, "%lo",
                         statInfo.st_size);
    }
    (void)snprintf_s(header_.mtime, BConstants::TIME_MAX_SIZE, BConstants::TIME_MAX_SIZE - 1, "%lo", statInfo.st_mtime);
    switch (statInfo.st_mode & S_IFMT) {
        case S_IFREG:
            header_.typeFlag = BConstants::TYPEFLAG_REGULAR_FILE; // typeFlag = '0'
            break;
        case S_IFDIR:
            header_.typeFlag = BConstants::TYPEFLAG_DIRECTORY; // typeFlag = '5'
            break;
        case S_IFLNK:
            strncpy_s(header_.fileSize, BConstants::FILESIZE_MAX_SIZE, "0", 1); // size of link file is 0
            header_.typeFlag = BConstants::TYPEFLAG_SYMBOLIC_LINK;              // typeFlag = '2'
            if (readlink(pathName.c_str(), header_.linkName, BConstants::LINKNAME_MAX_SIZE) == -1) {
                printf("BTarballPosixPaxHeader::BTarballPosixPaxHeader: readlink\n");
                throw exception();
            }
            break;
    }
    strncpy_s(header_.uname, BConstants::UGNAME_MAX_SIZE, getpwuid(statInfo.st_uid)->pw_name,
              BConstants::UGNAME_MAX_SIZE - 1);
    strncpy_s(header_.gname, BConstants::UGNAME_MAX_SIZE, getgrgid(statInfo.st_gid)->gr_name,
              BConstants::UGNAME_MAX_SIZE - 1);
    (void)snprintf_s(header_.devMajor, BConstants::DEV_MAX_SIZE, BConstants::DEV_MAX_SIZE - 1, "%o",
                     major(statInfo.st_dev));
    (void)snprintf_s(header_.devMinor, BConstants::DEV_MAX_SIZE, BConstants::DEV_MAX_SIZE - 1, "%o",
                     minor(statInfo.st_dev));
    unsigned int chksum = CalculateChksum((char *)&header_, BConstants::HEADER_SIZE);
    (void)snprintf_s(header_.chksum, BConstants::CHKSUM_MAX_SIZE - 1, BConstants::CHKSUM_MAX_SIZE - 2, "%6o",
                     chksum); // chksum字段的字符串休止符在字段数组的倒数第2个位置
}

void BTarballPosixPaxHeader::Publish(const UniqueFd &outFile)
{
    if (write(outFile, &header_, BConstants::HEADER_SIZE) == -1) {
        printf("BTarballPosixPaxHeader::Publish: write\n");
        throw exception();
    }
}
} // namespace OHOS::FileManagement::Backup