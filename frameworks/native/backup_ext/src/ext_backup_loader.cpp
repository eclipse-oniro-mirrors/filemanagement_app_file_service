/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "ext_backup_loader.h"

#include "ext_backup.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

ExtBackupLoader::ExtBackupLoader() = default;
ExtBackupLoader::~ExtBackupLoader() = default;

AbilityRuntime::Extension *ExtBackupLoader::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime) const
{
    HILOGI("Boot 3# Create as an BackupExtensionAbility(Loader)");
    return ExtBackup::Create(runtime);
}

map<string, string> ExtBackupLoader::GetParams()
{
    HILOGI("Boot 2# Register as an extension ability");
    return {
        // Type 即为 Extension 类型，定义在 ExtensionAbilityType 这一枚举类中。具体位置见 extension_ability_info.h
        {"type", "9"},
        {"name", "BackupExtensionAbility"},
    };
}

extern "C" __attribute__((visibility("default"))) void *OHOS_EXTENSION_GetExtensionModule()
{
    HILOGI("Boot 1# Load as a library");
    return &ExtBackupLoader::GetInstance();
}
} // namespace OHOS::FileManagement::Backup