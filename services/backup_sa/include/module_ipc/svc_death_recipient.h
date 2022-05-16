/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_DEATH_RECIPIENT_H
#define OHOS_FILEMGMT_BACKUP_SVC_DEATH_RECIPIENT_H

#include <functional>

#include "iremote_object.h"

namespace OHOS::FileManagement::Backup {
class SvcDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    void OnRemoteDied(const wptr<IRemoteObject> &object) override
    {
        functor_(object);
    };

public:
    SvcDeathRecipient(std::function<void(const wptr<IRemoteObject> &)> functor) : functor_(functor) {};

private:
    std::function<void(const wptr<IRemoteObject> &)> functor_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_DEATH_RECIPIENT_H