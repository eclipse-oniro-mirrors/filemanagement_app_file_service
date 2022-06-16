/*
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：本文件原则上要捕获所有异常，防止异常扩散到异常不安全的模块
 */
#include "module_ipc/service.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <regex>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include "ability_manager_client.h"
#include "ability_util.h"
#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "b_process/b_multiuser.h"
#include "b_resources/b_constants.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
#include "parameter.h"
#include "system_ability_definition.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, false);

void Service::OnStart()
{
    bool res = SystemAbility::Publish(sptr(this));
    HILOGI("End, res = %{public}d", res);

    string tmpPath = string(SA_ROOT_DIR) + string(SA_TMP_DIR);
    if (access(tmpPath.data(), F_OK) == 0 && !ForceRemoveDirectory(tmpPath.data())) {
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Failed to clear folder tmp");
    }
    if (access(tmpPath.data(), F_OK) != 0 && mkdir(tmpPath.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Failed to create folder tmp");
    }
}

void Service::OnStop()
{
    HILOGI("Called");
}

UniqueFd Service::GetLocalCapabilities()
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        struct statfs fsInfo = {};
        if (statfs(SA_ROOT_DIR, &fsInfo) == -1) {
            throw BError(errno);
        }

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(UniqueFd(open(SA_ROOT_DIR, O_TMPFILE | O_RDWR, 0600)));
        auto cache = cachedEntity.Structuralize();
        cache.SetFreeDiskSpace(fsInfo.f_bfree);
        cache.SetOSFullName(GetOSFullName());
        cache.SetDeviceType(GetDeviceType());
        cachedEntity.Persist();

        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        return UniqueFd(-e.GetCode());
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    session_.Deactive(obj, force);
}

string Service::VerifyCallerAndGetCallerName()
{
    try {
        uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
        int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
        if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
            Security::AccessToken::HapTokenInfo hapTokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetHapTokenInfo(tokenCaller, hapTokenInfo) != 0) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Get hap token info failed");
            }
            session_.VerifyBundleName(hapTokenInfo.bundleName);
            // REM: 校验ability type
            return hapTokenInfo.bundleName;
        } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
            if (IPCSkeleton::GetCallingUid() != 1000) { // REM: uid整改后调整
                throw BError(BError::Codes::SA_BROKEN_IPC, "Calling uid is invalid");
            }

            Security::AccessToken::NativeTokenInfo tokenInfo;
            if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenCaller, tokenInfo) != 0) {
                throw BError(BError::Codes::SA_INVAL_ARG, "Get native token info failed");
            }
            if (tokenInfo.processName != "backup_tool" && tokenInfo.processName != "hdcd") { // REM: hdcd整改后删除
                throw BError(BError::Codes::SA_INVAL_ARG, "Process name is invalid");
            }
            return "simulate";
        } else {
            throw BError(BError::Codes::SA_INVAL_ARG, string("Invalid token type ").append(to_string(tokenType)));
        }
    } catch (const BError &e) {
        return "";
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return "";
    } catch (...) {
        HILOGE("Unexpected exception");
        return "";
    }
}

[[maybe_unused]] static void LaunchBackupExtension(int userId, string bunldeName, BConstants::ExtensionAction action)
{
    string backupExtName = [bunldeName, userId]() {
        AppExecFwk::BundleInfo bundleInfo;
        auto bms = AAFwk::AbilityUtil::GetBundleManager();
        if (!bms) {
            throw BError(BError::Codes::SA_BROKEN_IPC, "Broken BMS");
        }
        if (!bms->GetBundleInfo(bunldeName, AppExecFwk::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, userId)) {
            string pendingMsg = string("Failed to get the info of bundle ").append(bunldeName);
            throw BError(BError::Codes::SA_BROKEN_IPC, pendingMsg);
        }
        for (auto &&ext : bundleInfo.extensionInfos) {
            if (ext.type == AppExecFwk::ExtensionAbilityType::BACKUP) {
                return ext.name;
            }
        }
        string pendingMsg = string("Bundle ").append(bunldeName).append(" need to instantiate a backup ext");
        throw BError(BError::Codes::SA_INVAL_ARG, pendingMsg);
    }();

    AAFwk::Want want;
    want.SetElementName(bunldeName, backupExtName);
    want.SetParam(BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));

    const int default_request_code = -1;
    AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, default_request_code, userId);
    HILOGI("Started %{public}s[%{public}d]'s %{public}s with %{public}s set to %{public}d", bunldeName.c_str(), userId,
           backupExtName.c_str(), BConstants::EXTENSION_ACTION_PARA, static_cast<int>(action));
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames)
{
    try {
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::RESTORE,
            .bundlesToProcess = bundleNames,
            .clientProxy = remote,
        });

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote,
                                   UniqueFd fd,
                                   const std::vector<BundleName> &bundleNames)
{
    try {
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::BACKUP,
            .bundlesToProcess = bundleNames,
            .clientProxy = remote,
        });

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        uint64_t size = cache.GetFreeDiskSpace();
        if (size == 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Invalid field FreeDiskSpace or unsufficient space");
        }

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

tuple<ErrCode, TmpFileSN, UniqueFd> Service::GetFileOnServiceEnd()
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);

        TmpFileSN tmpFileSN = seed++;
        string tmpPath = string(SA_ROOT_DIR) + string(SA_TMP_DIR) + to_string(tmpFileSN);
        if (access(tmpPath.data(), F_OK) == 0) {
            // 约束服务启动时清空临时目录，且生成的临时文件名必不重复
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Tmp file to create is existed");
        }
        UniqueFd fd(open(tmpPath.data(), O_RDWR | O_CREAT, 0600));
        if (fd < 0) {
            stringstream ss;
            ss << "Failed to open tmpPath " << errno;
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }

        return {ERR_OK, tmpFileSN, move(fd)};
    } catch (const BError &e) {
        return {e.GetCode(), -1, UniqueFd(-1)};
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return {BError(-EPERM), -1, UniqueFd(-1)};
    } catch (...) {
        HILOGE("Unexpected exception");
        return {BError(-EPERM), -1, UniqueFd(-1)};
    }
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    try {
        session_.VerifyCaller(IPCSkeleton::GetCallingTokenID(), IServiceReverse::Scenario::RESTORE);
        session_.VerifyBundleName(fileInfo.owner);

        if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }

        string tmpPath = string(SA_ROOT_DIR) + string(SA_TMP_DIR);
        UniqueFd dfdTmp(open(tmpPath.data(), O_RDONLY));
        if (dfdTmp < 0) {
            stringstream ss;
            ss << "Failed to open tmpPath " << errno;
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }

        string path = string(SA_ROOT_DIR) + fileInfo.owner + string("/");
        if (access(path.data(), F_OK) != 0 && mkdir(path.data(), S_IRWXU) != 0) {
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Failed to create folder");
        }

        UniqueFd dfdNew(open(path.data(), O_RDONLY));
        if (dfdNew < 0) {
            stringstream ss;
            ss << "Failed to open path " << errno;
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }

        string tmpFile = to_string(fileInfo.sn);
        if (renameat(dfdTmp, tmpFile.data(), dfdNew, fileInfo.fileName.data()) == -1) {
            stringstream ss;
            ss << "Failed to rename file " << errno;
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::AppFileReady(const string &fileName)
{
    try {
        string callerName = VerifyCallerAndGetCallerName();
        if (!regex_match(fileName, regex("^[0-9a-zA-Z_]+$"))) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
        }

        string path = string(SA_TEST_DIR) + fileName;
        UniqueFd fd(open(path.data(), O_RDWR, 0600));
        if (fd < 0) {
            stringstream ss;
            ss << "Failed to open path " << errno;
            throw BError(BError::Codes::SA_INVAL_ARG, ss.str());
        }

        struct stat fileStat = {};
        if (fstat(fd, &fileStat) != 0) {
            throw BError(BError::Codes::SA_INVAL_ARG, "Fail to get file stat");
        }
        if (fileStat.st_gid != SA_GID && fileStat.st_gid != 1000) { // REM: uid整改后删除
            throw BError(BError::Codes::SA_INVAL_ARG, "Gid is not in the whitelist");
        }

        auto proxy = session_.GetServiceReverseProxy();
        proxy->BackupOnFileReady(callerName, fileName, move(fd));

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode Service::AppDone(ErrCode errCode)
{
    try {
        string callerName = VerifyCallerAndGetCallerName();
        auto proxy = session_.GetServiceReverseProxy();
        proxy->BackupOnSubTaskFinished(errCode, callerName);

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}
} // namespace OHOS::FileManagement::Backup
