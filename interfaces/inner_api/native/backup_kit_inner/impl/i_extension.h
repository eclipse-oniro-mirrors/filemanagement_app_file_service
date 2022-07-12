/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_EXTENSION_H
#define OHOS_FILEMGMT_BACKUP_I_EXTENSION_H

#include <iremote_broker.h>

#include "errors.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class IExtension : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.FileManagement.Backup.IExtension");

    enum {
        CMD_GET_FILE_HANDLE = 1,
        CMD_HANDLE_CLAER,
    };

public:
    virtual UniqueFd GetFileHandle(std::string &fileName) = 0;
    virtual ErrCode  HandleClear() = 0;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_EXTENSION_H