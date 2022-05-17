/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H
#define OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H

/**
 * @file b_guard_signal.h
 * @brief 异常安全的临时调整信号处理程序方法
 *
 */

#include <signal.h>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BGuardSignal final : protected NoCopyable {
public:
    /**
     * @brief 构造器，其中会把给定信号的处理程序重置为默认值
     *
     * @param sig 给定信号
     */
    BGuardSignal(int sig);

    /**
     * @brief 析构器，其中会还原给定信号的处理程序
     *
     */
    ~BGuardSignal();

private:
    BGuardSignal() = delete;
    sighandler_t prevHandler_ {nullptr};
    int sig_ {-1};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_GUARD_SIGNAL_H