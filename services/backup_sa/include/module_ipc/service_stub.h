/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H

#include <map>

#include "i_service.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceStub : public IRemoteStub<IService> {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ServiceStub();
    ~ServiceStub() override = default;

private:
    using ServiceInterface = int32_t (ServiceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ServiceInterface> opToInterfaceMap_;

    int32_t CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInitBackupSession(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetFileOnServiceEnd(MessageParcel &data, MessageParcel &reply);
    int32_t CmdPublishFile(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAppDone(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_STUB_H