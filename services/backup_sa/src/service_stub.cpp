/*
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC模块的IO，具体业务逻辑实现在service.cpp中
 *     - 注意点2：所有调用开头处打印 Begin 字样，通过BError返回正常结果/错误码，这是出于防抵赖的目的
 */

#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"
#include "service_stub.h"

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
        stringstream ss;
        ss << "Cannot response request " << code << ": unknown tranction";
        return BError(BError::Codes::SA_INVAL_ARG, ss.str());
    }

    const std::u16string descriptor = ServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
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

    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdDumpObj(MessageParcel &data, MessageParcel &reply)
{
    auto obj = ComplexObject::Unmarshalling(data);
    DumpObj(*obj);

    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdInitRestoreSession(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    std::vector<string> appIds;
    if (!data.ReadStringVector(&appIds)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive appIds");
    }

    int32_t res = InitRestoreSession(appIds);
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str());
    }
    return BError(BError::Codes::OK);
}

int32_t ServiceStub::CmdGetLocalCapabilities(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");

    int fd = GetLocalCapabilities();

    if (!reply.WriteFileDescriptor(fd)) {
        return BError(BError::Codes::SA_BROKEN_IPC, "Failed to send out the file");
    }
    return BError(BError::Codes::OK);
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS