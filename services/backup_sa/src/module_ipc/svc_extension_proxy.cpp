/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include "module_ipc/svc_extension_proxy.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

UniqueFd SvcExtensionProxy::GetFileHandle(const string &fileName)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName");
        return UniqueFd(-1);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IExtension::CMD_GET_FILE_HANDLE, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return UniqueFd(-ret);
    }

    HILOGI("Successful");
    UniqueFd fd(reply.ReadFileDescriptor());
    return fd;
}

ErrCode SvcExtensionProxy::HandleClear()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IExtension::CMD_HANDLE_CLAER, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(-ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::HandleBackup()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IExtension::CMD_HANDLE_BACKUP, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(-ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode SvcExtensionProxy::PublishFile(const string &fileName)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName");
        return ErrCode(-EPERM);
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IExtension::CMD_PUBLISH_FILE, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ErrCode(-ret);
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}
} // namespace OHOS::FileManagement::Backup