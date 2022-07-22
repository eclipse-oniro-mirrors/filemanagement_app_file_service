/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H

#include <map>

#include "i_service_reverse.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseStub : public IRemoteStub<IServiceReverse>, protected NoCopyable {
public:
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ServiceReverseStub();
    ~ServiceReverseStub() override = default;

private:
    using ServiceInterface = int32_t (ServiceReverseStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ServiceInterface> opToInterfaceMap_;

    int32_t CmdBackupOnFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBackupOnBackupServiceDied(MessageParcel &data, MessageParcel &reply);

    int32_t CmdRestoreOnBundleStarted(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnBundleFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnAllBundlesFinished(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnFileReady(MessageParcel &data, MessageParcel &reply);
    int32_t CmdRestoreOnBackupServiceDied(MessageParcel &data, MessageParcel &reply);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_REVERSE_STUB_H
