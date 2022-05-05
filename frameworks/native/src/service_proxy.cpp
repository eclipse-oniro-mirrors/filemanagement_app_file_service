/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy.h"
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

int32_t ServiceProxy::InitRestoreSession(std::vector<AppId> apps)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteStringVector(apps)) {
        HILOGE("Failed to send appIds");
        return -EPIPE;
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_RESTORE_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ret;
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

int32_t ServiceProxy::InitBackupSession(UniqueFd fd, std::vector<AppId> appIDs)
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteFileDescriptor(fd)) {
        HILOGI("Failed to send the fd");
        return -EPIPE;
    }
    if (!data.WriteStringVector(appIDs)) {
        HILOGE("Failed to send appIDs");
        return -EPIPE;
    }

    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_INIT_BACKUP_SESSION, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ret;
    }
    HILOGI("Successful");
    return reply.ReadInt32();
}

int32_t ServiceProxy::GetLocalCapabilities()
{
    HILOGI("Start");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    int32_t ret = Remote()->SendRequest(IService::SERVICE_CMD_GET_LOCAL_CAPABILITIES, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Received error %{public}d when doing IPC", ret);
        return ret;
    }

    HILOGI("Successful");
    int fd = reply.ReadFileDescriptor();
    return fd;
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