/*
 * 版权所有 (c) 华为技术有限公司 2022
 * 说明：
 * 备份服务同一时间只接受一个唯一的会话。在会话期间，服务只与一个备份应用通信
 * 且只响应备份或恢复接口中的一种。
 */

#ifndef OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H
#define OHOS_FILEMGMT_BACKUP_SVC_SESSION_MANAGER_H

#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "b_file_info.h"
#include "i_service_reverse.h"
#include "module_ipc/svc_backup_connection.h"
#include "svc_death_recipient.h"

namespace OHOS::FileManagement::Backup {
struct BackupExtInfo {
    bool receExtManageJson {false};
    bool receExtAppDone {false};
    std::string backupExtName;
    sptr<SvcBackupConnection> backUpConnection;
    std::set<std::string> fileNameInfo;
};

class Service;
class SvcSessionManager final {
public:
    struct Impl {
        uint32_t clientToken {0};
        IServiceReverse::Scenario scenario {IServiceReverse::Scenario::UNDEFINED};
        std::map<BundleName, BackupExtInfo> backupExtNameMap;
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
    void Active(Impl newImpl);

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
    void VerifyBundleName(std::string &bundleName);

    /**
     * @brief 获取IServiceReverse
     *
     * @return sptr<IServiceReverse> 返回clientProxy
     * @throw BError::Codes::SA_REFUSED_ACT 调用者不是会话所有者
     */
    sptr<IServiceReverse> GetServiceReverseProxy();

    /**
     * @brief 获取Scenario
     *
     * @return IServiceReverse::Scenario 返回scenario
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     */
    IServiceReverse::Scenario GetScenario();

    /**
     * @brief 更新backupExtNameMap并判断是否完成分发
     *
     * @param bundleName 客户端信息
     * @param fileName 文件名称
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     * @return true 分发已完成
     * @return false 分发未完成
     */
    bool OnBunleFileReady(const std::string &bundleName, const std::string &fileName = "");

    /**
     * @brief 设置backup manage.json 信息
     *
     * @param bundleName 客户端信息
     * @param fd manage.json 文件描述符
     * @return UniqueFd 返回manage.json 文件描述符
     * @throw BError::Codes::SA_INVAL_ARG 获取异常
     */
    UniqueFd OnBunleExtManageInfo(const std::string &bundleName, UniqueFd fd);

    /**
     * @brief Remove backup extension info
     *
     * @param bundleName 应用名称
     */
    void RemoveExtInfo(const std::string &bundleName);

    /**
     * @brief get backupExtName info
     *
     * @param extNameVec
     */
    void GetBackupExtNameVec(std::vector<std::pair<std::string, std::string>> &extNameVec);

    /**
     * @brief get extension connection info
     *
     * @param bundleName
     * @return wptr<SvcBackupConnection>
     */
    wptr<SvcBackupConnection> GetExtConnection(const BundleName &bundleName);

    /**
     * @brief HiDumper dump info
     *
     * @param fd 对端dump句柄
     * @param args 服务参数
     */
    void DumpInfo(const int fd, const std::vector<std::u16string> &args);

private:
    /**
     * @brief 校验BundleName和ability type 并补全Impl.backupExtNameMap信息
     *
     * @param backupExtNameMap 客户端信息
     * @throw BError::Codes::SA_INVAL_ARG 客户端信息异常
     * @throw BError::Codes::SA_BROKEN_IPC
     */
    void GetBundleExtNames(std::map<BundleName, BackupExtInfo> &backupExtNameMap);

    /**
     * @brief 初始化 extension backUpConnection
     *
     * @param backupExtNameMap
     */
    void InitExtConn(std::map<BundleName, BackupExtInfo> &backupExtNameMap);

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
