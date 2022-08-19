/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy.h"

#include <cstddef>
#include <sstream>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "service_proxy_load_callback.h"
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

ErrCode ServiceProxy::AppFileReady(const string &fileName, UniqueFd fd)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the filename").GetCode();
    }
    if (!data.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fd").GetCode();
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

ErrCode ServiceProxy::GetExtFileName(string &bundleName, string &fileName)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteString(bundleName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the bundleName").GetCode();
    }
    if (!data.WriteString(fileName)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send the fileName").GetCode();
    }

    MessageParcel reply;
    MessageOption option;
    option.SetFlags(MessageOption::TF_ASYNC);
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_GET_EXT_FILE_NAME, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the request because of " << ret;
        BError(BError::Codes::SDK_INVAL_ARG, ss.str());
    }
    HILOGI("Successful");
    return ret;
}

sptr<IService> ServiceProxy::GetInstance()
{
    unique_lock<mutex> lock(proxyMutex_);
    if (serviceProxy_ != nullptr) {
        return serviceProxy_;
    }

    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        HILOGE("Get an empty samgr");
        return nullptr;
    }
    sptr<ServiceProxyLoadCallback> loadCallback = new ServiceProxyLoadCallback();
    if (loadCallback == nullptr) {
        HILOGE("loadCallback is nullptr.");
        return nullptr;
    }
    int32_t ret = samgr->LoadSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, loadCallback);
    if (ret != ERR_OK) {
        HILOGE("Failed to Load systemAbility, systemAbilityId:%d, ret code:%d", FILEMANAGEMENT_BACKUP_SERVICE_SA_ID,
               ret);
        return nullptr;
    }

    auto waitStatus = proxyConVar_.wait_for(lock, std::chrono::milliseconds(BConstants::BACKUP_LOADSA_TIMEOUT_MS),
                                            []() { return serviceProxy_ != nullptr; });
    if (!waitStatus) {
        HILOGE("srrvice load sa timeout");
        return nullptr;
    }
    return serviceProxy_;
}

void ServiceProxy::FinishStartSA(const sptr<IRemoteObject> &remoteObject)
{
    HILOGI("start");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = iface_cast<IService>(remoteObject);
    if ((!serviceProxy_) || (!serviceProxy_->AsObject())) {
        HILOGE("Failed to get backup srevice proxy.");
        return;
    }
    proxyConVar_.notify_one();
}

void ServiceProxy::FinishStartSAFailed()
{
    HILOGI("start");
    unique_lock<mutex> lock(proxyMutex_);
    serviceProxy_ = nullptr;
}
} // namespace OHOS::FileManagement::Backup