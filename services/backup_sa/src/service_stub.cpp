/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service_stub.h"
#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

ServiceStub::ServiceStub()
{
    opToInterfaceMap_[SERVICE_CMD_ECHO] = &ServiceStub::CmdEchoServer;
    opToInterfaceMap_[SERVICE_CMD_DUMPOBJ] = &ServiceStub::CmdDumpObj;
    opToInterfaceMap_[SERVICE_CMD_OUTFD] = &ServiceStub::CmdGetFd;
}

int32_t ServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGI("Begin to call procedure with code %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        HILOGE("Cannot response request %d: unknown tranction", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        HILOGE("Check remote descriptor failed");
        return IPC_STUB_INVALID_DATA_ERR;
    }

    return (this->*(interfaceIndex->second))(data, reply);
}

int32_t ServiceStub::CmdEchoServer(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin to dispatch cmd EchoServer");
    string echoStr = data.ReadString();
    int32_t strLen = EchoServer(echoStr);
    reply.WriteInt32(strLen);
    HILOGI("EchoServer has recved str %{public}s with length %{public}d", echoStr.c_str(), strLen);

    return ERR_NONE;
}

int32_t ServiceStub::CmdDumpObj(MessageParcel &data, MessageParcel &reply)
{
    auto obj = ComplexObject::Unmarshalling(data);
    DumpObj(*obj);

    return ERR_NONE;
}
int32_t ServiceStub::CmdGetFd(MessageParcel &data, MessageParcel &reply)
{
    HILOGI("Begin to dispatch Cmd GetFd");
    int fd = GetFd();
    auto ret = reply.WriteFileDescriptor(fd);
    if (!ret) {
        HILOGI("ServiceStub::WriteFileDescriptor %{public}d", ret);
    }
    return ret;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS