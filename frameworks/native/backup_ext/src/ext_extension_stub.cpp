/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include "ext_extension_stub.h"

#include <cstdint>
#include <sstream>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
ExtExtensionStub::ExtExtensionStub()
{
    opToInterfaceMap_[CMD_GET_FILE_HANDLE] = &ExtExtensionStub::CmdGetFileHandle;
    opToInterfaceMap_[CMD_HANDLE_CLAER] = &ExtExtensionStub::CmdHandleClear;
}

int32_t ExtExtensionStub::OnRemoteRequest(uint32_t code,
                                          MessageParcel &data,
                                          MessageParcel &reply,
                                          MessageOption &option)
{
    HILOGI("Begin to call procedure indexed %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        stringstream ss;
        ss << "Cannot response request " << code << ": unknown procedure";
        return BError(BError::Codes::SA_INVAL_ARG, ss.str());
    }

    const std::u16string descriptor = ExtExtensionStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        return BError(BError::Codes::SA_INVAL_ARG, "Invalid remote descriptor");
    }

    return (this->*(interfaceIndex->second))(data, reply);
}

ErrCode ExtExtensionStub::CmdGetFileHandle(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    string fileName;
    if (!data.ReadString(fileName)) {
        return BError(BError::Codes::SA_INVAL_ARG, "Failed to receive fileName");
    }

    UniqueFd fd = GetFileHandle(fileName);
    if (!reply.WriteInt32(fd)) {
        stringstream ss;
        ss << "Failed to send the result " << fd;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}

ErrCode ExtExtensionStub::CmdHandleClear(MessageParcel &data, MessageParcel &reply)
{
    HILOGE("Begin");
    ErrCode res = HandleClear();
    if (!reply.WriteInt32(res)) {
        stringstream ss;
        ss << "Failed to send the result " << res;
        return BError(BError::Codes::SA_BROKEN_IPC, ss.str()).GetCode();
    }
    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup