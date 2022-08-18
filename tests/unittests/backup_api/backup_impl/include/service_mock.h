/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#ifndef MOCK_SERVICE_MOCK_H
#define MOCK_SERVICE_MOCK_H

#include <fcntl.h>
#include <gmock/gmock.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "i_service.h"
#include "iremote_stub.h"
#include "test_manager.h"

namespace OHOS::FileManagement::Backup {
class ServiceMock : public IRemoteStub<IService> {
public:
    int code_ = 0;
    ServiceMock() : code_(0) {}
    virtual ~ServiceMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return BError(BError::Codes::OK);
    }

    int32_t InvokeGetLocalSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        std::string fileName = "1.tar";
        TestManager tm("GetLocalCap_GetFd_0100");
        std::string filePath = tm.GetRootDirCurTest().append(fileName);
        UniqueFd fd(open(filePath.data(), O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR));
        reply.WriteFileDescriptor(fd);
        return BError(BError::Codes::OK);
    }

    virtual ErrCode InitRestoreSession(sptr<IServiceReverse> remote,
                                       const std::vector<BundleName> &bundleNames) override
    {
        return BError(BError::Codes::OK);
    }

    virtual ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                                      UniqueFd fd,
                                      const std::vector<BundleName> &bundleNames) override
    {
        return BError(BError::Codes::OK);
    }

    virtual ErrCode Start() override
    {
        return BError(BError::Codes::OK);
    }

    virtual UniqueFd GetLocalCapabilities() override
    {
        return UniqueFd(-1);
    }

    virtual ErrCode PublishFile(const BFileInfo &fileInfo) override
    {
        return BError(BError::Codes::OK);
    }

    virtual ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) override
    {
        return BError(BError::Codes::OK);
    }

    virtual ErrCode AppDone(ErrCode errCode) override
    {
        return BError(BError::Codes::OK);
    }

    virtual ErrCode GetExtFileName(std::string &bundleName, std::string &fileName) override
    {
        return BError(BError::Codes::OK);
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_MOCK_H