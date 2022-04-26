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

int32_t ServiceProxy::GetFd()
{
    HILOGI("Begin to GetFd");
    MessageParcel data;
    data.WriteInterfaceToken(GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    auto ret = Remote()->SendRequest(IService::SERVICE_CMD_OUTFD, data, reply, option);
    if (ret != NO_ERROR) {
        HILOGE("Get GetFd SendRequest %{public}d", ret);
        return -1;
    }

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