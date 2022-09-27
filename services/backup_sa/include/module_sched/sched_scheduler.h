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

#ifndef OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
#define OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H

#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "b_resources/b_constants.h"
#include "iremote_broker.h"
#include "thread_pool.h"

namespace OHOS::FileManagement::Backup {
class Service;

class SchedScheduler final {
public:
    /**
     * @brief 给workqueue下发请求任务
     *
     */
    void Sched(const std::string &bundleName);

    /**
     * @brief 暂存真实文件请求
     *
     */
    void QueueGetFileRequest(const std::string &bundleName, const std::string &fileName);

    /**
     * @brief 激活请求
     *
     */
    void WorkQueue(const std::function<void()> &task);

    /**
     * @brief 暂存启动extension请求
     *
     * @param bundleName 应用名称
     * @param backupExtName
     */
    void QueueSetExtBundleName(const std::string &bundleName, const std::string &backupExtName);

    /**
     * @brief 给workqueue下发连接extension任务
     *
     */
    void SchedConn();

    /**
     * @brief 之前extension 备份恢复流程
     *
     * @param bundleName 应用名称
     */
    void ExtStart(std::string bundleName);

    /**
     * @brief 更新当前启动中的extension表
     *
     * @param bundleName 应用名称
     */
    void RemoveExtConn(const std::string &bundleName);

public:
    explicit SchedScheduler(wptr<Service> reversePtr) : reversePtr_(reversePtr)
    {
        threadPool_.Start(BConstants::SA_THREAD_POOL_COUNT);
    }
    ~SchedScheduler()
    {
        threadPool_.Stop();
    }

private:
    mutable std::shared_mutex lock_;
    mutable std::shared_mutex extLock_;
    OHOS::ThreadPool threadPool_;

    std::vector<std::tuple<std::string, std::string>> getFileRequests_;
    std::vector<std::tuple<std::string, std::string>> extBundleName_;
    std::set<std::string> extStartName_;

    wptr<Service> reversePtr_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
