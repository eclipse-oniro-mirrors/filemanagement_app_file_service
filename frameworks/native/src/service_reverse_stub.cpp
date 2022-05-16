/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include "service_reverse_stub.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceReverseStub::OnRemoteRequest(uint32_t code,
                                            MessageParcel &data,
                                            MessageParcel &reply,
                                            MessageOption &option)
{
    HILOGI("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        stringstream ss;
        ss << "Cannot response request " << code << ": unknown procedure";
        return BError(BError::Codes::SA_INVAL_ARG, ss.str());
    }

    const std::u16string descriptor = ServiceReverseStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
    }

    return (this->*(interfaceIndex->second))(data, reply);
}

ServiceReverseStub::ServiceReverseStub()
{
    opToInterfaceMap_[SERVICER_BACKUP_ON_FILE_READY] = &ServiceReverseStub::CmdBackupOnFileReady;
    opToInterfaceMap_[SERVICER_BACKUP_ON_SUB_TASK_STARTED] = &ServiceReverseStub::CmdBackupOnSubTaskStarted;
    opToInterfaceMap_[SERVICER_BACKUP_ON_SUB_TASK_FINISHED] = &ServiceReverseStub::CmdBackupOnSubTaskFinished;
    opToInterfaceMap_[SERVICER_BACKUP_ON_TASK_FINISHED] = &ServiceReverseStub::CmdBackupOnTaskFinished;
    opToInterfaceMap_[SERVICER_BACKUP_ON_SUB_TASK_STARTED] = &ServiceReverseStub::CmdBackupOnSubTaskStarted;
    opToInterfaceMap_[SERVICER_RESTORE_ON_SUB_TASK_FINISHED] = &ServiceReverseStub::CmdRestoreOnSubTaskFinished;
    opToInterfaceMap_[SERVICER_RESTORE_ON_TASK_FINISHED] = &ServiceReverseStub::CmdRestoreOnTaskFinished;
}

int32_t ServiceReverseStub::CmdBackupOnFileReady(MessageParcel &data, MessageParcel &reply)
{
    auto appId = data.ReadString();
    auto fileName = data.ReadString();
    int fd = data.ReadFileDescriptor();
    BackupOnFileReady(appId, fileName, fd);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnSubTaskStarted(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto appId = data.ReadString();
    BackupOnSubTaskStarted(errCode, appId);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnSubTaskFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto appId = data.ReadString();

    BackupOnSubTaskFinished(errCode, appId);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdBackupOnTaskFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    BackupOnTaskFinished(errCode);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnSubTaskStarted(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto appId = data.ReadString();
    RestoreOnSubTaskStarted(errCode, appId);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnSubTaskFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    auto appId = data.ReadString();
    RestoreOnSubTaskFinished(errCode, appId);
    return BError(BError::Codes::OK);
}

int32_t ServiceReverseStub::CmdRestoreOnTaskFinished(MessageParcel &data, MessageParcel &reply)
{
    int32_t errCode = data.ReadInt32();
    RestoreOnTaskFinished(errCode);
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup