/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>
#include <tuple>

#include "b_file_info.h"
#include "complex_object.h"
#include "i_service_reverse.h"
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
        SERVICE_CMD_INIT_BACKUP_SESSION,
        SERVICE_CMD_GET_LOCAL_CAPABILITIES,
        SERVICE_CMD_GET_FILE_ON_SERVICE_END,
        SERVICE_CMD_PUBLISH_FILE,
    };

    virtual int32_t EchoServer(const std::string &echoStr) = 0;
    virtual void DumpObj(const ComplexObject &obj) = 0;
    virtual ErrCode InitRestoreSession(sptr<IServiceReverse> remote, std::vector<AppId> apps) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, std::vector<AppId> apps) = 0;
    virtual UniqueFd GetLocalCapabilities() = 0;
    virtual std::tuple<ErrCode, TmpFileSN, UniqueFd> GetFileOnServiceEnd() = 0;
    virtual ErrCode PublishFile(const BFileInfo &fileInfo) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H