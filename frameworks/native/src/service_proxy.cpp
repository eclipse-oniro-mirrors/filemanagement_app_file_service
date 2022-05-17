/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy.h"

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileManagement {
using namespace std;

namespace Backup {
int32_t ServiceProxy::EchoServer(const string &echoStr)
{
    HILOGI("Begin to echo %{public}s", echoStr.c_str());
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    data.WriteString(echoStr);

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(IService::SERVICE_CMD_ECHO, data, reply, option);

    return reply.ReadInt32();
}

void ServiceProxy::DumpObj(const ComplexObject &obj)
{
    MessageParcel data;
    data.WriteParcelable(&obj);

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(IService::SERVICE_CMD_DUMPOBJ, data, reply, option);
}

int32_t ServiceProxy::InitRestoreSession(sptr<IServiceReverse> remote, std::vector<AppId> apps)
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

    if (!data.WriteStringVector(apps)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send appIds").GetCode();
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_RESTORE_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the quest for " << ret;
        return BError(BError::Codes::SDK_INVAL_ARG, ss.str()).GetCode();
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

int32_t ServiceProxy::InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, std::vector<AppId> appIDs)
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
    if (!data.WriteStringVector(appIDs)) {
        return BError(BError::Codes::SDK_INVAL_ARG, "Failed to send appIds").GetCode();
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_BACKUP_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        stringstream ss;
        ss << "Failed to send out the quest because of " << ret;
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

tuple<ErrCode, TmpFileSN, UniqueFd> ServiceProxy::GetFileOnServiceEnd()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_GET_FILE_ON_SERVICE_END, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return {ret, 0, UniqueFd(-1)};
    }

    HILOGI("Successful");
    return {reply.ReadInt32(),reply.ReadUint32(),UniqueFd(reply.ReadFileDescriptor())};
}

ErrCode ServiceProxy::PublishFile(const BFileInfo &fileInfo)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    if (!data.WriteParcelable(&fileInfo)) {
        HILOGI("Failed to send the fileInfo");
        return -EPIPE;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_PUBLISH_FILE, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ret;
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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS