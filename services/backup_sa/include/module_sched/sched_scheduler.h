/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
#define OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H

#include <shared_mutex>
#include <string>
#include <vector>

#include "b_resources/b_constants.h"
#include "refbase.h"
#include "thread_pool.h"

namespace OHOS::FileManagement::Backup {
class SvcSessionManager;

class SchedScheduler {
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
    void WorkQueue(std::string bundleName, std::string fileName);

    /**
     * @brief Get the File Handle object
     *
     * @param bundleName
     * @param fileName
     */
    void GetFileHandle(const std::string &bundleName, const std::string &fileName);

public:
    explicit SchedScheduler(SvcSessionManager &session) : session_(session)
    {
        threadPool_.Start(BConstants::SA_THREAD_POOL_COUNT);
    }
    ~SchedScheduler()
    {
        threadPool_.Stop();
    }

private:
    mutable std::shared_mutex lock_;
    OHOS::ThreadPool threadPool_;
    std::vector<std::tuple<std::string, std::string>> getFileRequests_;
    SvcSessionManager &session_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_SCHED_SCHEDULER_H
