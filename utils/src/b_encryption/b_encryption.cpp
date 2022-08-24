/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_encryption/b_encryption.h"

namespace OHOS::FileManagement::Backup::BEncryption {
unsigned int CalculateChksum(const char *byteBlock, int blockSize)
{
    unsigned int chksum = 0U;
    for (int i = 0; i < blockSize; ++i) {
        chksum += 0xFF & *(byteBlock + i);
    }
    return chksum;
}
} // namespace OHOS::FileManagement::Backup::BEncryption