/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#ifndef MOCK_SERVICE_REVERSE_MOCK_H
#define MOCK_SERVICE_REVERSE_MOCK_H

#include <gmock/gmock.h>

#include "b_error/b_error.h"
#include "i_service_reverse.h"
#include "iremote_stub.h"

namespace OHOS::FileManagement::Backup {
class ServiceReverseMock : public IRemoteStub<IServiceReverse> {
public:
    int code_ = 0;
    ServiceReverseMock() : code_(0) {}
    virtual ~ServiceReverseMock() {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));

    int32_t InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return BError(BError::Codes::OK);
    }
    virtual void BackupOnFileReady(std::string bundleName, std::string fileName, int fd) override {}
    virtual void BackupOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    virtual void BackupOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    virtual void BackupOnAllBundlesFinished(int32_t errCode) override {}

    virtual void RestoreOnBundleStarted(int32_t errCode, std::string bundleName) override {}
    virtual void RestoreOnBundleFinished(int32_t errCode, std::string bundleName) override {}
    virtual void RestoreOnAllBundlesFinished(int32_t errCode) override {}
    virtual void RestoreOnFileReady(std::string bundleName, std::string fileName, int fd) override {}
};
} // namespace OHOS::FileManagement::Backup
#endif // MOCK_SERVICE_MOCK_H