/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_H
#define OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_H

#include "extension_context.h"

namespace OHOS::FileManagement::Backup {
class ExtBackupContext : public AbilityRuntime::ExtensionContext {
public:
    ExtBackupContext() {};
    virtual ~ExtBackupContext() = default;

    static inline size_t CONTEXT_TYPE_ID = std::hash<const char *> {}("extBackupContext");

protected:
    bool IsContext(size_t contextTypeId) override
    {
        return contextTypeId == CONTEXT_TYPE_ID || ExtensionContext::IsContext(contextTypeId);
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_EXT_BACKUP_CONTEXT_H