/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H

#include "extension_base.h"
#include "extension_context.h"
#include "runtime.h"

namespace OHOS::FileManagement::Backup {
class ExtBackup : public AbilityRuntime::ExtensionBase<AbilityRuntime::ExtensionContext>,
                  public std::enable_shared_from_this<ExtBackup> {
public:
    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
              const std::shared_ptr<AbilityRuntime::OHOSApplication> &application,
              std::shared_ptr<AbilityRuntime::AbilityHandler> &handler,
              const sptr<IRemoteObject> &token) override;

    /**
     * @brief Create Extension.
     *
     * @param runtime The runtime.
     * @return The ServiceExtension instance.
     */
    static ExtBackup *Create(const std::unique_ptr<AbilityRuntime::Runtime> &runtime);

public:
    ExtBackup() = default;
    ~ExtBackup() override = default;

private:
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_H