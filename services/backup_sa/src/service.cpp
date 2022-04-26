/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

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
            HILOGI("Failed to statfs because of %{public}s", strerror(errno));
            throw system_error(errno, std::generic_category());
        }

        BJsonCachedEntity<BJsonEntityCaps> cachedEntity(UniqueFd(open(SA_ROOT_DIR, O_TMPFILE | O_RDWR, 0600)));
        auto cache = cachedEntity.Structuralize();
        cache.SetFreeDiskSpace(fsInfo.f_bfree);
        cachedEntity.Persist();

        return cachedEntity.GetFd().Release();
    } catch (const system_error &e) {
        return -1 * e.code().value();
    } catch (const exception &e) {
        return -EPERM;;
    }
}

int32_t Service::InitRestoreSession(std::vector<AppId> apps)
{
    if (apps.empty()) {
        HILOGE("Invalid argument: No app was selected");
        return -EINVAL;
    }
    return ERR_NONE;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS