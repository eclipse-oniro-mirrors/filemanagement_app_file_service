/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H
#define OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H

#include <map>

#include "i_extension.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ExtExtensionStub : public IRemoteStub<IExtension> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

public:
    ExtExtensionStub();
    ~ExtExtensionStub() = default;

private:
    ErrCode CmdGetFileHandle(MessageParcel &data, MessageParcel &reply);
    ErrCode CmdHandleClear(MessageParcel &data, MessageParcel &reply);

private:
    using ExtensionInterface = int32_t (ExtExtensionStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ExtensionInterface> opToInterfaceMap_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_EXTENSION_STUB_H