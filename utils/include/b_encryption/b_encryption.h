/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_ENCRYPTION_H
#define OHOS_FILEMGMT_BACKUP_B_ENCRYPTION_H

namespace OHOS::FileManagement::Backup::BEncryption {
unsigned int CalculateChksum(const char *byteBlock, int blockSize);
} // namespace OHOS::FileManagement::Backup::BEncryption

#endif // OHOS_FILEMGMT_BACKUP_B_ENCRYPTION_H