/*
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 注意：
 *     - 注意点1：本文件原则上只处理与IPC无关的业务逻辑
 *     - 注意点2：本文件原则上要捕获所有异常，防止异常扩散到异常不安全的模块
 */
#include "service.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include "b_error/b_error.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json_entity_caps.h"
#include "filemgmt_libhilog.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

REGISTER_SYSTEM_ABILITY_BY_ID(Service, FILEMANAGEMENT_BACKUP_SERVICE_SA_ID, true);

void Service::OnStart()
{
    HILOGI("Begin Service");
    bool res = SystemAbility::Publish(this);
    HILOGI("End Service, res = %{public}d", res);
}

void Service::OnStop()
{
    HILOGI("Done Service");
}

int32_t Service::EchoServer(const string &echoStr)
{
    HILOGI("Service::EchoServer %{public}s", echoStr.c_str());
    return echoStr.length();
}

void Service::DumpObj(const ComplexObject &obj)
{
    HILOGI("field1 = %{public}d, field2 = %{public}d", obj.field1_, obj.field2_);
}

int32_t Service::GetLocalCapabilities()
{
    try {
        struct statfs fsInfo = {};
        if (statfs(SA_ROOT_DIR, &fsInfo) == -1) {
            throw BError(errno);
        }

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(UniqueFd(open(SA_ROOT_DIR, O_TMPFILE | O_RDWR, 0600)));
        auto cache = cachedEntity.Structuralize();
        cache.SetFreeDiskSpace(fsInfo.f_bfree);
        cachedEntity.Persist();

        return cachedEntity.GetFd().Release();
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %s", e.what());
        return -EPERM;
    }
}

int32_t Service::InitRestoreSession(std::vector<AppId> apps)
{
    try {
        if (apps.empty()) {
            throw BError(BError::Codes::SA_INVAL_ARG, "No app was selected");
        }

        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    }
}

int32_t Service::InitBackupSession(UniqueFd fd, std::vector<AppId> apps)
{
    try {
        if (apps.empty()) {
            throw BError(BError::Codes::SA_INVAL_ARG, "No app was selected");
        }

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(move(fd));
        auto cache = cachedEntity.Structuralize();
        uint64_t size = cache.GetFreeDiskSpace();
        if (size == 0) {
            throw BError(BError::Codes::SA_INVAL_ARG,
                         "Field FreeDiskSpace is invalid or there's no enough space left on disk");
        }
        HILOGI("Check field FreeDiskSpace size %{public}llu", size);
        return BError(BError::Codes::OK);
    } catch (const BError &e) {
        return e.GetCode();
    }
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS
