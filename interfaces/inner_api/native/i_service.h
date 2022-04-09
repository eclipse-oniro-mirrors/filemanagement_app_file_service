/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>

#include "complex_object.h"
#include "iremote_broker.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class IService : public IRemoteBroker {
public:
    enum {
        SERVICE_CMD_ECHO = 0,
        SERVICE_CMD_DUMPOBJ = 1,
        SERVICE_CMD_OUTFD = 2,
    };

    virtual int32_t EchoServer(const std::string &echoStr) = 0;
    virtual void DumpObj(const ComplexObject &obj) = 0;
    virtual int32_t GetFd() = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H