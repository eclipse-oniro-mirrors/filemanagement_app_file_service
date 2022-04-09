/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include <cerrno>
#include <cstring>
#include <unistd.h>

#include "filemgmt_libhilog.h"
#include "i_service.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "unique_fd.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

static void Foo()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    HILOGE("Get samgr %{public}p", samgr.GetRefPtr());

    auto remote = samgr->GetSystemAbility(FILEMANAGEMENT_BACKUP_SERVICE_SA_ID);
    HILOGE("Get remote %{public}p", remote.GetRefPtr());

    auto proxy = iface_cast<Backup::IService>(remote);
    HILOGE("Get proxy %{public}p", proxy.GetRefPtr());

    proxy->EchoServer("hello, world");
    proxy->DumpObj({12, 34});

    UniqueFd fd(proxy->GetFd());
    const string str = "hello, world";
    if (write(fd.Get(), str.c_str(), str.length()) == -1) {
        HILOGI("write error %{public}d %{public}s", errno, strerror(errno));
    }
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

int main(int argc, char const *argv[])
{
    OHOS::FileManagement::Backup::Foo();

    return 0;
}