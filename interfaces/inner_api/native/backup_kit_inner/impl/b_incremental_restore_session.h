/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_RESTORE_RESTORE_H
#define OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_RESTORE_RESTORE_H

#include <functional>
#include <memory>
#include <vector>

#include "b_file_info.h"
#include "b_incremental_data.h"
#include "errors.h"
#include "svc_death_recipient.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BIncrementalRestoreSession {
public:
    struct Callbacks {
        std::function<void(const BFileInfo &, UniqueFd, UniqueFd, ErrCode)> onFileReady; // 当备份服务有文件待发送时执行的回调
        std::function<void(ErrCode, const BundleName)> onBundleStarted; // 当启动某个应用的恢复流程结束时执行的回调函数
        std::function<void(ErrCode, const BundleName)>
            onBundleFinished; // 当某个应用的恢复流程结束或意外中止时执行的回调函数
        std::function<void(ErrCode)> onAllBundlesFinished; // 当整个恢复流程结束或意外中止时执行的回调函数
        std::function<void(const std::string)> onResultReport; // 某个应用恢复流程中自定义错误信息的上报的回调函数
        std::function<void()> onBackupServiceDied;         // 当备份服务意外死亡时执行的回调函数
    };

public:
    /**
     * @brief 获取一个用于控制恢复流程的会话
     *
     * @param callbacks 注册的回调函数
     * @return std::unique_ptr<BRestoreSession> 指向BRestoreSession的智能指针。失败时为空指针
     */
    static std::unique_ptr<BIncrementalRestoreSession> Init(Callbacks callbacks);

    /**
     * @brief 通知备份服务文件内容已就绪
     *
     * @param fileInfo 文件描述信息
     * @return ErrCode 规范错误码
     * @see GetFileHandle
     */
    ErrCode PublishFile(BFileInfo fileInfo);

    /**
     * @brief 通知备份服务SA文件内容已就绪
     *
     * @param fileInfo 文件描述信息
     * @param fd 文件描述符
     * @return ErrCode 规范错误码
     * @see GetFileHandle
     */
    ErrCode PublishSAFile(BFileInfo fileInfo, UniqueFd fd);

    /**
     * @brief 请求恢复流程所需的真实文件
     *
     * @param bundleName 应用名称
     * @param fileName   文件名称
     */
    ErrCode GetFileHandle(const std::string &bundleName, const std::string &fileName);

    /**
     * @brief 用于追加应用，现阶段仅支持在Start之前调用
     *
     * @param remoteCap 已打开的保存远端设备能力的Json文件。可使用GetLocalCapabilities方法获取
     * @param bundlesToRestore 待恢复的应用清单
     * @param detailInfos bundle对应的单双映射关系json串
     *
     * @return ErrCode 规范错误码
     */
    ErrCode AppendBundles(UniqueFd remoteCap, std::vector<BundleName> bundlesToRestore,
        std::vector<std::string> detailInfos);

    /**
     * @brief 用于追加应用，现阶段仅支持在Start之前调用
     *
     * @param remoteCap 已打开的保存远端设备能力的Json文件。可使用GetLocalCapabilities方法获取
     * @param bundlesToRestore 待恢复的应用清单
     *
     * @return ErrCode 规范错误码
     */
    ErrCode AppendBundles(UniqueFd remoteCap, std::vector<BundleName> bundlesToRestore);

    /**
     * @brief 用于结束服务
     *
     * @return ErrCode 规范错误码
     */
    ErrCode Release();

    /**
     * @brief 注册备份服务意外死亡时执行的回调函数
     *
     * @param functor 回调函数
     */
    void RegisterBackupServiceDied(std::function<void()> functor);

public:
    ~BIncrementalRestoreSession();

private:
    sptr<SvcDeathRecipient> deathRecipient_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_INCREMENTAL_RESTORE_RESTORE_H