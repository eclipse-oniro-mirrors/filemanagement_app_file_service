/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H
#define OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H

#include "ability_connect_callback_stub.h"
#include "i_extension.h"
#include "module_ipc/svc_death_recipient.h"
#include "module_sched/sched_scheduler.h"
#include "refbase.h"

namespace OHOS::FileManagement::Backup {
class SvcBackupConnection : public AAFwk::AbilityConnectionStub {
public:
    /**
     * @brief This method is called back to receive the connection result after an ability calls the
     * ConnectAbility method to connect it to an extension ability.
     *
     * @param element: Indicates information about the connected extension ability.
     * @param remote: Indicates the remote proxy object of the extension ability.
     * @param resultCode: Indicates the connection result code. The value 0 indicates a successful connection, and any
     * other value indicates a connection failure.
     */
    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                              const sptr<IRemoteObject> &remoteObject,
                              int resultCode) override;

    /**
     * @brief This method is called back to receive the disconnection result after the connected extension ability
     * crashes or is killed. If the extension ability exits unexpectedly, all its connections are disconnected, and
     * each ability previously connected to it will call onAbilityDisconnectDone.
     *
     * @param element: Indicates information about the disconnected extension ability.
     * @param resultCode: Indicates the disconnection result code. The value 0 indicates a successful disconnection,
     * and any other value indicates a disconnection failure.
     */
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

    /**
     * @brief connect remote ability of ExtBackup.
     */
    ErrCode ConnectBackupExtAbility(AAFwk::Want &want);

    /**
     * @brief disconnect remote ability of ExtBackup.
     */
    ErrCode DisconnectBackupExtAbility();

    /**
     * @brief check whether connected to remote extension ability.
     *
     * @return bool true if connected, otherwise false.
     */
    bool IsExtAbilityConnected();

    /**
     * @brief get the proxy of backup extension ability.
     *
     * @return the proxy of backup extension ability.
     */
    sptr<IExtension> GetBackupExtProxy();

public:
    SvcBackupConnection(std::function<void(const std::string &)> functor) : functor_(functor) {}
    virtual ~SvcBackupConnection() override {};

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    std::atomic<bool> isConnected_ = {false};
    std::atomic<bool> isConnectedDone_ = {false};
    sptr<IExtension> backupProxy_;
    std::function<void(const std::string &)> functor_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_BACKUP_CONNECTION_H