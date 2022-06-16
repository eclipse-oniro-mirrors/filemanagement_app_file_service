/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_LOADER_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_LOADER_H

#include "extension_module_loader.h"

namespace OHOS::FileManagement::Backup {
class ExtBackupLoader : public AbilityRuntime::ExtensionModuleLoader, public Singleton<ExtBackupLoader> {
    DECLARE_SINGLETON(ExtBackupLoader);

public:
    /**
     * @brief Create Extension.
     *
     * @param runtime The runtime.
     * @return The Extension instance.
     */
    AbilityRuntime::Extension *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime) const override;

    virtual std::map<std::string, std::string> GetParams() override;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_LOADER_H