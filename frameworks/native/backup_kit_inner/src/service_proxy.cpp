/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

int32_t ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    MessageParcel reply;
    MessageOption option;

    if (!remote) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Empty reverse stub").GetCode();
    }
    if (!data.WriteRemoteObject(remote->AsObject().GetRefPtr())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the reverse stub").GetCode();
    }

    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_RESTORE_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request for " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

int32_t ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote,
                                        UniqueFd fd,
                                        const std::vector<BundleName> &bundleNames)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    MessageParcel reply;
    MessageOption option;

    if (!remote) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Empty reverse stub").GetCode();
    }
    if (!data.WriteRemoteObject(remote->AsObject().GetRefPtr())) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the reverse stub").GetCode();
    }

    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
    }
    if (!data.WriteStringVector(bundleNames)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send bundleNames").GetCode();
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_BACKUP_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode ServiceProxy::Start()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_START, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

UniqueFd ServiceProxy::GetLocalCapabilities()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_GET_LOCAL_CAPABILITIES, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return UniqueFd(-ret);
    }

    HILOGI("Successful");
    UniqueFd fd(reply.ReadFileDescriptor());
    return fd;
}

tuple<ErrCode, TmpFileSN, UniqueFd> ServiceProxy::GetFileOnServiceEnd(string &bundleName)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(bundleName)) {
        return {BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the bundleName").GetCode(), 0, UniqueFd(-1)};
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_GET_FILE_ON_SERVICE_END, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return {ret, 0, UniqueFd(-1)};
    }

    HILOGI("Successful");
    return {reply.ReadInt32(), reply.ReadUint32(), UniqueFd(reply.ReadFileDescriptor())};
}

ErrCode ServiceProxy::PublishFile(const BFileInfo &fileInfo)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteParcelable(&fileInfo)) {
        HILOGE("Failed to send the fileInfo");
        return -EPIPE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_PUBLISH_FILE, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }

    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppFileReady(const string &fileName)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the filename").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_APP_FILE_READY, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

ErrCode ServiceProxy::AppDone(ErrCode errCode)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteInt32(errCode)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the errCode").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_APP_DONE, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

sptr<IService> ServiceProxy::GetInstance()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("Get an empty samgr");
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
    if (!remote) {
        HILOGE("Get an empty backup sa");
        return nullptr;
    }

    auto proxy = iface_cast<Backup::IService>(remote);
    if (!proxy) {
        HILOGE("Get an empty backup sa proxy");
        return nullptr;
    }

    // 无需缓存 Proxy，因为 SAMgr 可能因为服务死亡等原因返回不同指针
    return proxy;
}
} // namespace OHOS::FileManagement::Backup