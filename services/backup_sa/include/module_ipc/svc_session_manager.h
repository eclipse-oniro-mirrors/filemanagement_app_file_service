/*
 * 版权所有 (c) 华为技术有限公司 2022
 * 说明：
 * 备份服务同一时间只接受一个唯一的会话。在会话期间，服务只与一个备份应用通信
 * 且只响应备份或恢复接口中的一种。
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H

#include <shared_mutex>

#include "b_file_info.h"
#include "i_service_reverse.h"
#include "module_ipc/svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {
class Service;
class SvcSessionManager final {
public:
    struct Impl {
        uint32_t clientToken {0};
        IServiceReverse::Scenario scenario {IServiceReverse::Scenario::UNDEFINED};
        std::vector<BundleName> bundlesToProcess;
        sptr<IServiceReverse> clientProxy;
    };

public:
    /**
     * @brief 检验调用者是否是会话所有者，且当前命令是否与当前场景相匹配
     *
     * @param clientToken 调用者TOKEN
     * @param scen 给定场景
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     * @throw BError::Codes::SDK_MIXED_SCENARIO 调用者在备份/恢复场景使用了不匹配的函数
     */
    void VerifyCaller(uint32_t clientToken, IServiceReverse::Scenario scen) const;

    /**
     * @brief 激活会话
     *
     * @param impl 客户端信息
     */
    void Active(const Impl &impl);

    /**
     * @brief 关闭会话
     *
     * @param remoteInAction 尝试关闭会话的客户端代理。只有激活会话的客户端代理有权关闭会话
     * @param force 强制关闭
     */
    void Deactive(const wptr<IRemoteObject> &remoteInAction, bool force = false);

    /**
     * @brief 检验调用者给定的bundleName是否是有效的
     *
     * @param bundleName 调用者名称
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     */
    void VerifyBundleName(const std::string &bundleName);

public:
    /**
     * @brief Construct a new Svc Session object
     *
     * @param reversePtr 指向Service的反向指针，使用wptr避免循环引用
     */
    SvcSessionManager(wptr<Service> reversePtr) : reversePtr_(reversePtr) {}
    ~SvcSessionManager() = default;

private:
    mutable std::shared_mutex lock_;
    wptr<Service> reversePtr_;
    sptr<SvcDeathRecipient> deathRecipient_;
    Impl impl_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
