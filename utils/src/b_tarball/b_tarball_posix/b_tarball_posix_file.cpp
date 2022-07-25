/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_file.h"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixFile::BTarballPosixFile(const string &pathName) : pathName_(pathName)
{
    struct stat inStat = {};
    file_ = UniqueFd(open(pathName.c_str(), O_RDONLY | O_NOFOLLOW));
    if (file_ == -1) {
        if (errno == ELOOP) { // if this file is a symbolic link
            if (lstat(pathName.c_str(), &inStat) == -1) {
                printf("BTarballPosixFile::BTarballPosixFile: lstat\n");
                throw exception();
            }
        } else {
            printf("BTarballPosixFile::BTarballPosixFile: open\n");
            throw exception();
        }
    } else {
        if (fstat(file_, &inStat) == -1) {
            printf("BTarballPosixFile::BTarballPosixFile: fstat\n");
            throw exception();
        }
    }

    extData_ = BTarballPosixExtendedData(pathName, inStat);
    extHdr_ = BTarballPosixExtendedHeader(extData_);
    paxHdr_ = BTarballPosixPaxHeader(pathName, inStat);
    fileData_ = BTarballPosixFileData(inStat);
}

void BTarballPosixFile::Publish(const UniqueFd &outFile)
{
    extHdr_.Publish(outFile);
    extData_.Publish(outFile);
    paxHdr_.Publish(outFile);
    fileData_.Publish(file_, outFile);
}
} // namespace OHOS::FileManagement::Backup