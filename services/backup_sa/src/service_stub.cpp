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
    opToInterfaceMap_[SERVICE_CMD_INIT_RESTORE_SESSION] = &ServiceStub::CmdInitRestoreSession;
    opToInterfaceMap_[SERVICE_CMD_GET_LOCAL_CAPABILITIES] = &ServiceStub::CmdGetLocalCapabilities;
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

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    std::vector<string> appIds;
    if (!data.ReadStringVector(&appIds)) {
        HILOGE("Failed to receive appIds");
        return -EINVAL;
    }
    int32_t res = InitRestoreSession(appIds);

    if (!reply.WriteInt32(res)) {
        HILOGE("Failed to send the result %{public}d", res);
        return -EPIPE;
    }
    HILOGE("Successful");
    return ERR_NONE;
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    int fd = GetLocalCapabilities();
    if (!reply.WriteFileDescriptor(fd)) {
        HILOGI("Failed to send the result");
        return -EPIPE;
    }
    HILOGE("Successful");
    return ERR_NONE;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS