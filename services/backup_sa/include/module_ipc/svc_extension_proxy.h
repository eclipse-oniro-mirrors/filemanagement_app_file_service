/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H
#define OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H

#include "i_extension.h"
#include "iremote_proxy.h"

namespace OHOS::FileManagement::Backup {
class SvcExtensionProxy : public IRemoteProxy<IExtension> {
public:
    UniqueFd GetFileHandle(const std::string &fileName) override;
    ErrCode HandleClear() override;
    ErrCode HandleBackup() override;
    ErrCode PublishFile(const std::string &fileName) override;

public:
    explicit SvcExtensionProxy(const sptr<IRemoteObject> &remote) : IRemoteProxy<IExtension>(remote) {}

private:
    static inline BrokerDelegator<SvcExtensionProxy> delegator_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_EXTENSION_PROXY_H