/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_H

#include <string>
#include <tuple>

#include "b_file_info.h"
#include "i_service_reverse.h"
#include "iremote_broker.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class IService : public IRemoteBroker {
public:
    enum {
        SERVICE_CMD_INIT_RESTORE_SESSION,
        SERVICE_CMD_INIT_BACKUP_SESSION,
        SERVICE_CMD_GET_LOCAL_CAPABILITIES,
        SERVICE_CMD_GET_FILE_ON_SERVICE_END,
        SERVICE_CMD_PUBLISH_FILE,
        SERVICE_CMD_APP_FILE_READY,
        SERVICE_CMD_APP_DONE,
        SERVICE_CMD_START,
        SERVICE_CMD_GET_EXT_FILE_NAME,
    };

    virtual ErrCode InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                                      UniqueFd fd,
                                      const std::vector<BundleName> &bundleNames) = 0;
    virtual ErrCode Start() = 0;
    virtual UniqueFd GetLocalCapabilities() = 0;
    virtual std::tuple<ErrCode, TmpFileSN, UniqueFd> GetFileOnServiceEnd(std::string &bundleName) = 0;
    virtual ErrCode PublishFile(const BFileInfo &fileInfo) = 0;
    virtual ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) = 0;
    virtual ErrCode AppDone(ErrCode errCode) = 0;
    virtual ErrCode GetExtFileName(std::string &bundleName, std::string &fileName) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Filemanagement.Backup.IService")
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_H