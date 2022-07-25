/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_file_data.h"

#include <cstdio>
#include <linux/unistd.h>

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixFileData::BTarballPosixFileData(struct stat &statInfo) : stat_(statInfo) {}

void BTarballPosixFileData::Publish(const UniqueFd &inFile, const UniqueFd &outFile)
{
    switch (stat_.st_mode & S_IFMT) {
        case S_IFLNK:
        case S_IFDIR:
            break;
        case S_IFREG:
            loff_t inOffset = 0;
            loff_t outOffset = lseek(outFile, 0, SEEK_CUR);
            ssize_t lengthToCopy = stat_.st_size;
            do {
                ssize_t nBytesCopied =
                    syscall(__NR_copy_file_range, inFile.Get(), &inOffset, outFile.Get(), &outOffset, lengthToCopy, 0);
                if (nBytesCopied == -1) {
                    printf("BTarballPosixFileData::Publish: copy_file_range\n");
                    throw exception();
                }
                lseek(outFile, nBytesCopied, SEEK_CUR);
                lengthToCopy -= nBytesCopied;
            } while (lengthToCopy > 0);

            char block[BConstants::BLOCK_SIZE] {};
            size_t nBytesCompleted = BConstants::BLOCK_SIZE - stat_.st_size % BConstants::BLOCK_SIZE;
            if (nBytesCompleted < BConstants::BLOCK_SIZE) {
                if (write(outFile, block, nBytesCompleted) == -1) {
                    printf("BTarballPosixFileData::Publish: write\n");
                    throw exception();
                }
            }
            break;
    }
}
} // namespace OHOS::FileManagement::Backup