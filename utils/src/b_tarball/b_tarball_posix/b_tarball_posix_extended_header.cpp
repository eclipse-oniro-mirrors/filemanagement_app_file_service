/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_extended_header.h"

#include <cstdio>

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

BTarballPosixExtendedHeader::BTarballPosixExtendedHeader(BTarballPosixExtendedData &extData)
{
    (void)snprintf_s(header_.fileSize, BConstants::FILESIZE_MAX_SIZE, BConstants::FILESIZE_MAX_SIZE - 1, "%lo",
                     extData.GetEntriesSize());
    header_.typeFlag = BConstants::TYPEFLAG_EXTENDED; // typeFlag = 'x'
    unsigned int chksum = CalculateChksum((char *)&header_, BConstants::HEADER_SIZE);
    (void)snprintf_s(header_.chksum, BConstants::CHKSUM_MAX_SIZE - 1, BConstants::CHKSUM_MAX_SIZE - 2, "%6o",
                     chksum); // chksum字段的字符串休止符在字段数组的倒数第2个位置
}

void BTarballPosixExtendedHeader::Publish(const UniqueFd &outFile)
{
    if (strcmp(header_.fileSize, "0") != 0) {
        if (write(outFile, &header_, BConstants::HEADER_SIZE) == -1) {
            printf("ExtendedHeader::Publish: write\n");
            throw exception();
        }
    }
}
} // namespace OHOS::FileManagement::Backup