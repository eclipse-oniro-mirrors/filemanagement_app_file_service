/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "service.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

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

int32_t Service::GetFd()
{
    int fd = open("/data/backup/tmp", O_RDWR | O_CREAT, 0600);
    if (fd < 0) {
        HILOGI("ServiceStub::CmdGeTtutorialFd %{public}d %{public}s", errno, strerror(errno));
    }
    return fd;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS