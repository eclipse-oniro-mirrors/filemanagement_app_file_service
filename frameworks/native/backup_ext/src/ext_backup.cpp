/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "ext_backup.h"
#include "ext_backup_js.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
void ExtBackup::Init(const shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
                     const shared_ptr<AbilityRuntime::OHOSApplication> &application,
                     shared_ptr<AbilityRuntime::AbilityHandler> &handler,
                     const sptr<IRemoteObject> &token)
{
    HILOGI("Boot 7# Init the BackupExtensionAbility(Base)");
    AbilityRuntime::ExtensionBase<AbilityRuntime::ExtensionContext>::Init(record, application, handler, token);
}

ExtBackup *ExtBackup::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Boot 4# Create as an BackupExtensionAbility(Base)");
    if (!runtime) {
        return new ExtBackup();
    }

    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS:
            return ExtBackupJs::Create(runtime);

        default:
            return new ExtBackup();
    }
}
} // namespace OHOS::FileManagement::Backup