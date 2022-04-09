/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_proxy.h"
#include "filemgmt_libhilog.h"

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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS