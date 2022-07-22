/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include "ext_extension.h"

#include <cstdint>
#include <sstream>

#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BackupExtExtension::VerifyCaller()
{
    HILOGI("begin");
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        throw BError(BError::Codes::EXT_BROKEN_IPC, "Calling tokenType is error");
    } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        if (IPCSkeleton::GetCallingUid() != BConstants::BACKUP_UID) {
            throw BError(BError::Codes::EXT_BROKEN_IPC, "Calling uid is invalid");
        }
    }
}

UniqueFd BackupExtExtension::GetFileHandle(string &fileName)
{
    HILOGI("begin fileName = %{public}s", fileName.data());
    VerifyCaller();
    if (extension_ == nullptr) {
        HILOGE("%{public}s end failed.", __func__);
        return UniqueFd(-1);
    }
    UniqueFd fd = extension_->GetFileHandle(fileName);
    return fd;
}

ErrCode BackupExtExtension::HandleClear()
{
    HILOGI("begin clear");
    VerifyCaller();
    if (extension_ == nullptr) {
        HILOGE("%{public}s end failed.", __func__);
        return EPERM;
    }
    ErrCode ret = extension_->HandleClear();
    return ret;
}

ErrCode BackupExtExtension::PublishFile(string &fileName)
{
    HILOGI("begin fileName = %{public}s", fileName.data());
    return ERR_OK;
}

ErrCode BackupExtExtension::HandleBackup()
{
    HILOGI("begin");
    return ERR_OK;
}
} // namespace OHOS::FileManagement::Backup