/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H

#include <string>

namespace OHOS {
namespace FileManagement {
namespace Backup {
using AppId = std::string;

struct BFileInfo {
    AppId owner;
    std::string fileName;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
