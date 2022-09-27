/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_FILEMGMT_BACKUP_SERVICE_H
#define OHOS_FILEMGMT_BACKUP_SERVICE_H

#include <cstdint>
#include <mutex>

#include "i_service_reverse.h"
#include "iremote_stub.h"
#include "module_sched/sched_scheduler.h"
#include "service_stub.h"
#include "svc_session_manager.h"
#include "system_ability.h"

namespace OHOS::FileManagement::Backup {
class Service final : public SystemAbility, public ServiceStub, protected NoCopyable {
    DECLARE_SYSTEM_ABILITY(Service);

    // 以下都是IPC接口
public:
    ErrCode InitRestoreSession(sptr<IServiceReverse> remote, const std::vector<BundleName> &bundleNames) override;
    ErrCode InitBackupSession(sptr<IServiceReverse> remote,
                              UniqueFd fd,
                              const std::vector<BundleName> &bundleNames) override;
    ErrCode Start() override;
    UniqueFd GetLocalCapabilities() override;
    ErrCode PublishFile(const BFileInfo &fileInfo) override;
    ErrCode AppFileReady(const std::string &fileName, UniqueFd fd) override;
    ErrCode AppDone(ErrCode errCode) override;
    ErrCode GetExtFileName(std::string &bundleName, std::string &fileName) override;

    // 以下都是非IPC接口
public:
    void OnStart() override;
    void OnStop() override;
    void StopAll(const wptr<IRemoteObject> &obj, bool force = false);
    int Dump(int fd, const std::vector<std::u16string> &args) override;

    /**
     * @brief 启动 backup extension
     *
     * @param bundleName
     * @param backupExtName
     * @return ErrCode
     */
    ErrCode LaunchBackupExtension(const BundleName &bundleName, const std::string &backupExtName);

    /**
     * @brief backup extension died
     *
     * @param bundleName 应用名称
     */
    void OnBackupExtensionDied(const std::string &&bundleName, ErrCode ret);

    /**
     * @brief 获取 ExtConnect 连接状态
     *
     * @param bundleName 应用名称
     * @return true connect ok
     * @return false connect failed
     */
    bool TryExtConnect(const std::string &bundleName);

    /**
     * @brief 执行backup extension 备份恢复流程
     *
     * @param bundleName 应用名称
     */
    void ExtStart(const std::string &bundleName);

    /**
     * @brief Get the File Handle object
     *
     * @param bundleName 应用名称
     * @param fileName 文件名称
     */
    void GetFileHandle(const std::string &bundleName, const std::string &fileName);

public:
    explicit Service(int32_t saID, bool runOnCreate = false)
        : SystemAbility(saID, runOnCreate), session_(wptr(this)) {};
    ~Service() override = default;

private:
    std::string VerifyCallerAndGetCallerName();

private:
    static sptr<Service> instance_;
    static std::mutex instanceLock_;
    static inline std::atomic<uint32_t> seed {1};

    SvcSessionManager session_;
    std::unique_ptr<SchedScheduler> sched_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SERVICE_H