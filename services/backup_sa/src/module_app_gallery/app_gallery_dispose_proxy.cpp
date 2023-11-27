/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "module_app_gallery/app_gallery_dispose_proxy.h"

#include <string>

#include "message_parcel.h"
#include "want.h"

#include "module_app_gallery/app_gallery_service_connection.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

mutex AppGalleryDisposeProxy::instanceLock_;
mutex AppGalleryDisposeProxy::conditionMutex_;

string AppGalleryDisposeProxy::abilityName = "AppFoundationService";
sptr<IRemoteObject> AppGalleryDisposeProxy::appRemoteObj_;
condition_variable AppGalleryDisposeProxy::conditionVal_;
mutex AppGalleryDisposeProxy::appRemoteObjLock_;
mutex AppGalleryDisposeProxy::connectMutex;
sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::appGalleryDisposeProxyInstance_;

AppGalleryDisposeProxy::AppGalleryDisposeProxy()
{
    HILOGI("AppGalleryDisposeProxy construct");
}

AppGalleryDisposeProxy::~AppGalleryDisposeProxy()
{
    if (appGalleryDisposeProxyInstance_ != nullptr) {
        delete appGalleryDisposeProxyInstance_;
        appGalleryDisposeProxyInstance_ = nullptr;
    }
}

sptr<AppGalleryDisposeProxy> AppGalleryDisposeProxy::GetInstance()
{
    if (appGalleryDisposeProxyInstance_ == nullptr) {
        lock_guard<mutex> autoLock(instanceLock_);
        if (appGalleryDisposeProxyInstance_ == nullptr) {
            appGalleryDisposeProxyInstance_ = new AppGalleryDisposeProxy;
        }
    }

    return appGalleryDisposeProxyInstance_;
}

DisposeErr AppGalleryDisposeProxy::StartBackup(const std::string &bundleName)
{
    HILOGI("StartBackup, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::START_BACKUP);
}

DisposeErr AppGalleryDisposeProxy::EndBackup(const std::string &bundleName)
{
    HILOGI("EndBackup, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::END_BACKUP);
}

DisposeErr AppGalleryDisposeProxy::StartRestore(const std::string &bundleName)
{
    HILOGI("StartRestore, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::START_RESTORE);
}

DisposeErr AppGalleryDisposeProxy::EndRestore(const std::string &bundleName)
{
    HILOGI("EndRestore, app %{public}s", bundleName.c_str());
    return DoDispose(bundleName, DisposeOperation::END_RESTORE);
}

DisposeErr AppGalleryDisposeProxy::DoDispose(const std::string &bundleName, DisposeOperation disposeOperation)
{
    HILOGI("DoDispose, app %{public}s, operation %{public}d", bundleName.c_str(), disposeOperation);
    if (!ConnectExtAbility<AppGalleryDisposeProxy>() || appRemoteObj_ == nullptr) {
        HILOGI("Can not connect to %{public}s", bundleName.c_str());
        return DisposeErr::CONN_FAIL;
    }

    MessageParcel data;
    if (!data.WriteString16(Str8ToStr16(bundleName))) {
        HILOGI("write ownerInfo and bundleName failed");
        return DisposeErr::IPC_FAIL;
    }

    if (!data.WriteRemoteObject(this)) {
        HILOGI("write RemoteObject failed");
        return DisposeErr::IPC_FAIL;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    int32_t ret = appRemoteObj_->SendRequest(static_cast<int>(disposeOperation), data, reply, option);
    if (ret != ERR_NONE) {
        HILOGI("SendRequest error, code=%{public}d, bundleName=%{public}s", ret, bundleName.c_str());
        return DisposeErr::REQUEST_FAIL;
    }

    HILOGI("SendRequest success, dispose=%{public}d, bundleName=%{public}s", disposeOperation, bundleName.c_str());
    return DisposeErr::OK;
}

} // namespace OHOS::FileManagement::Backup