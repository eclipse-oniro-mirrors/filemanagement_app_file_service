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

#include "b_error/b_error.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_caps.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
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
        cachedEntity.Persist();

        return move(cachedEntity.GetFd());
    } catch (const BError &e) {
        return UniqueFd(-1);
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return UniqueFd(-EPERM);
    }
}

void Service::StopAll(const wptr<IRemoteObject> &obj, bool force)
{
    session_.Deactive(obj, force);
}

ErrCode Service::InitRestoreSession(sptr<IServiceReverse> remote, std::vector<AppId> apps)
{
    try {
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::RESTORE,
            .appsToOperate = std::move(apps),
            .clientProxy = remote,
        });

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        StopAll(nullptr, true);
        return e.GetCode();
    }
}

ErrCode Service::InitBackupSession(sptr<IServiceReverse> remote, UniqueFd fd, std::vector<AppId> apps)
{
    try {
        session_.Active({
            .clientToken = IPCSkeleton::GetCallingTokenID(),
            .scenario = IServiceReverse::Scenario::BACKUP,
            .appsToOperate = std::move(apps),
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
    }
}

tuple<ErrCode, TmpFileSN, UniqueFd> Service::GetFileOnServiceEnd()
{
    TmpFileSN tmpFileSN = seed++;
    string tmpPath = string(SA_ROOT_DIR) + string(SA_TMP_DIR) + to_string(tmpFileSN);
    if (access(tmpPath.data(), F_OK) == 0) {
        //约束服务启动时清空临时目录，且生成的临时文件名必不重复
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Tmp file to create is existed");
    }
    UniqueFd fd(open(tmpPath.data(), O_RDWR | O_CREAT, 0600));
    if (fd < 0) {
        stringstream ss;
        ss << "Failed to open " << errno;
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
    }
    return {SUBSYS_COMMON, tmpFileSN, move(fd)};
}

ErrCode Service::PublishFile(const BFileInfo &fileInfo)
{
    HILOGE("Begin");
    if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z]+$"))) {
        throw BError(BError::Codes::SA_INVAL_ARG, "Filename is not alphanumeric");
    }

    string tmpPath = string(SA_ROOT_DIR) + string(SA_TMP_DIR);
    UniqueFd dfdTmp(open(tmpPath.data(), O_RDONLY));
    if (dfdTmp < 0) {
        stringstream ss;
        ss << "Failed to open " << errno;
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
    }

    string path = string(SA_ROOT_DIR) + fileInfo.owner + string("/");
    if (access(path.data(), F_OK) != 0 && mkdir(path.data(), S_IRWXU) != 0) {
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, "Failed to create folder");
    }

    UniqueFd dfdNew(open(path.data(), O_RDONLY));
    if (dfdNew < 0) {
        stringstream ss;
        ss << "Failed to open " << errno;
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
    }

    string tmpFile = to_string(fileInfo.sn);
    if (renameat(dfdTmp, tmpFile.data(), dfdNew, fileInfo.fileName.data()) == -1) {
        stringstream ss;
        ss << "Failed to rename " << errno;
        throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
    }

    return BError(BError::Codes::OK);
}
} // namespace OHOS::FileManagement::Backup
