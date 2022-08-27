/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#ifndef MOCK_EXTENSION_MOCK_H
#define MOCK_EXTENSION_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "i_extension.h"
#include "iremote_stub.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class BackupExtExtensionMock : public IRemoteStub<IExtension> {
public:
    int code_ = 0;
    BackupExtExtensionMock() : code_(0) {}
    virtual ~BackupExtExtensionMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        reply.WriteInt32(BError(BError::Codes::OK));
        return BError(BError::Codes::OK);
    }

    int32_t InvokeGetFileHandleRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        std::string fileName = "1.tar";
        TestManager tm("GetFileHand_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        reply.WriteFileDescriptor(fd);
        return BError(BError::Codes::OK);
    }

    UniqueFd GetFileHandle(const std::string &fileName) override
    {
        return UniqueFd(-1);
    };
    ErrCode HandleClear() override
    {
        return BError(BError::Codes::OK);
    };
    ErrCode HandleBackup() override
    {
        return BError(BError::Codes::OK);
    };
    ErrCode PublishFile(const std::string &fileName) override
    {
        return BError(BError::Codes::OK);
    };
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_EXTENSION_MOCK_H