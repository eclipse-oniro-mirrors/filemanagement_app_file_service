/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "module_ipc/service_reverse_proxy.h"

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ServiceReverseProxy::BackupOnFileReady(string bundleName, string fileName, int fd)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteString(bundleName) || !data.WriteString(fileName) ||
        !data.WriteFileDescriptor(fd)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_BACKUP_ON_FILE_READY, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    };

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_BACKUP_ON_SUB_TASK_STARTED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnBundleFinished(int32_t errCode, string bundleName, int32_t bundleTotalFiles)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName) ||
        !data.WriteInt32(bundleTotalFiles)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_BACKUP_ON_SUB_TASK_FINISHED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::BackupOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_BACKUP_ON_TASK_FINISHED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnBundleStarted(int32_t errCode, string bundleName)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_RESTORE_ON_SUB_TASK_STARTED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnBundleFinished(int32_t errCode, string bundleName)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode) || !data.WriteString(bundleName)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_RESTORE_ON_SUB_TASK_FINISHED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}

void ServiceReverseProxy::RestoreOnAllBundlesFinished(int32_t errCode)
{
    HILOGI("Begin");
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor()) || !data.WriteInt32(errCode)) {
        throw BError(BError::Codes::SA_BROKEN_IPC);
    }

    MessageParcel reply;
    MessageOption option;
    if (int err = Remote()->SendRequest(IServiceReverse::SERVICER_RESTORE_ON_TASK_FINISHED, data, reply, option);
        err != ERR_OK) {
        throw BError(BError::Codes::SA_BROKEN_IPC, to_string(err));
    }
}
} // namespace OHOS::FileManagement::Backup