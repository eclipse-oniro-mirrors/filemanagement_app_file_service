/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>

#include "b_file_info.h"
#include "complex_object.h"
#include "iremote_broker.h"
#include "unique_fd.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class IService : public IRemoteBroker {
public:
    enum {
        SERVICE_CMD_ECHO,
        SERVICE_CMD_DUMPOBJ,
        SERVICE_CMD_INIT_RESTORE_SESSION,
        SERVICE_CMD_GET_LOCAL_CAPABILITIES,
    };

    virtual int32_t EchoServer(const std::string &echoStr) = 0;
    virtual void DumpObj(const ComplexObject &obj) = 0;
    virtual int32_t InitRestoreSession(std::vector<AppId> apps) = 0;
    virtual int32_t GetLocalCapabilities() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H