/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H
#define OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H

/**
 * @file b_cwd_guard.h
 * @brief 异常安全的临时变更目录方法
 *
 */

#include <string_view>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BGuardCwd final : protected NoCopyable {
public:
    /**
     * @brief 构造器，其中会把当前目录变更为目标目录
     *
     * @param tgtDir 目标目录
     */
    BGuardCwd(std::string_view tgtDir);

    /**
     * @brief 析构器，其中会把目标目录恢复为当前目录
     *
     */
    ~BGuardCwd();

private:
    BGuardCwd() = delete;
    char *pwd_ {nullptr};
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_GUARD_CWD_H