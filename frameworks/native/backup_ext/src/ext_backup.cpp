/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "ext_backup.h"

#include <algorithm>
#include <cstdio>
#include <sstream>

#include <sys/stat.h>
#include <sys/types.h>

#include "bundle_mgr_client.h"
#include "unique_fd.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "ext_backup_js.h"
#include "ext_extension.h"
#include "filemgmt_libhilog.h"
#include "i_service.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

CreatorFunc ExtBackup::creator_ = nullptr;
void ExtBackup::SetCreator(const CreatorFunc &creator)
{
    creator_ = creator;
}

void ExtBackup::Init(const shared_ptr<AbilityRuntime::AbilityLocalRecord> &record,
                     const shared_ptr<AbilityRuntime::OHOSApplication> &application,
                     shared_ptr<AbilityRuntime::AbilityHandler> &handler,
                     const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility(Base)");
    AbilityRuntime::ExtensionBase<ExtBackupContext>::Init(record, application, handler, token);
}

ExtBackup *ExtBackup::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    if (!runtime) {
        HILOGD("Create as BackupExtensionAbility(base)");
        return new ExtBackup();
    }

    if (creator_) {
        HILOGD("Create as BackupExtensionAbility(creater)");
        return creator_(runtime);
    }

    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS:
            HILOGD("Create as BackupExtensionAbility(JS)");
            return ExtBackupJs::Create(runtime);

        default:
            HILOGD("Create as BackupExtensionAbility(base)");
            return new ExtBackup();
    }
}

void ExtBackup::OnStart(const AAFwk::Want &want)
{
    HILOGI("BackupExtensionAbility was started");
    Extension::OnStart(want);
}

void ExtBackup::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    HILOGI("BackupExtensionAbility was invoked. restart=%{public}d, startId=%{public}d", restart, startId);

    // REM: 处理返回结果 ret
    // REM: 通过杀死进程实现 Stop
}

string ExtBackup::GetUsrConfig() const
{
    vector<string> config;
    AppExecFwk::BundleMgrClient client;
    BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
    const AppExecFwk::AbilityInfo &info = *abilityInfo_;
    if (!client.GetProfileFromAbility(info, "ohos.extension.backup", config)) {
        throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to invoke the GetProfileFromAbility method.");
    }

    return config.empty() ? "" : config[0];
}

bool ExtBackup::AllowToBackupRestore()
{
    string usrConfig = GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (cache.GetAllowToBackupRestore() || WasFromSpecialVersion() || SpeicalVersionForCloneAndCloud()) {
        return true;
    }
    return false;
}

bool ExtBackup::UseFullBackupOnly(void) const
{
    string usrConfig = GetUsrConfig();
    BJsonCachedEntity<BJsonEntityExtensionConfig> cachedEntity(usrConfig);
    auto cache = cachedEntity.Structuralize();
    if (cache.GetFullBackupOnly()) {
        HILOGI("backup use fullBackupOnly.");
        return true;
    }
    HILOGI("backup not use fullBackupOnly.");
    return false;
}

BConstants::ExtensionAction ExtBackup::GetExtensionAction() const
{
    return extAction_;
}

BConstants::ExtensionAction ExtBackup::VerifyAndGetAction(const AAFwk::Want &want,
                                                          std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    string pendingMsg = "Received an empty ability. You must missed the init proc";
    BExcepUltils::BAssert(abilityInfo, BError::Codes::EXT_INVAL_ARG, pendingMsg);
    using namespace BConstants;
    ExtensionAction extAction {want.GetIntParam(EXTENSION_ACTION_PARA, static_cast<int>(ExtensionAction::INVALID))};
    if (extAction == ExtensionAction::INVALID) {
        int extActionInt = static_cast<int>(extAction);
        pendingMsg = string("Want must specify a valid action instead of ").append(to_string(extActionInt));
        throw BError(BError::Codes::EXT_INVAL_ARG, pendingMsg);
    }
    return extAction;
}

ErrCode ExtBackup::GetParament(const AAFwk::Want &want)
{
    if (extAction_ == BConstants::ExtensionAction::RESTORE) {
        appVersionStr_ = want.GetStringParam(BConstants::EXTENSION_VERSION_NAME_PARA);
        appVersionCode_ = want.GetIntParam(BConstants::EXTENSION_VERSION_CODE_PARA, 0);
        restoreType_ = want.GetIntParam(BConstants::EXTENSION_RESTORE_TYPE_PARA, 0);

        HILOGI("Get version %{public}s type %{public}d from want when restore.", appVersionStr_.c_str(), restoreType_);
    }
    /* backup don't need parament. */
    return ERR_OK;
}

sptr<IRemoteObject> ExtBackup::OnConnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin connect");
        BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
        // 发起者必须是备份服务
        auto extAction = VerifyAndGetAction(want, abilityInfo_);
        if (extAction_ != BConstants::ExtensionAction::INVALID && extAction == BConstants::ExtensionAction::INVALID &&
            extAction_ != extAction) {
            HILOGE("Verification action failed.");
            return nullptr;
        }
        extAction_ = extAction;
        GetParament(want);
        // 应用必须配置支持备份恢复
        if (!AllowToBackupRestore()) {
            HILOGE("The application does not allow to backup and restore.");
            return nullptr;
        }

        Extension::OnConnect(want);

        auto remoteObject =
            sptr<BackupExtExtension>(new BackupExtExtension(std::static_pointer_cast<ExtBackup>(shared_from_this())));

        // 排除特殊场景
        if (!WasFromSpecialVersion() && !RestoreDataReady()) {
            remoteObject->ExtClear();
        }

        return remoteObject->AsObject();
    } catch (const BError &e) {
        return nullptr;
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return nullptr;
    } catch (...) {
        HILOGE("");
        return nullptr;
    }
}

void ExtBackup::OnDisconnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin disconnect");
        Extension::OnDisconnect(want);
        extAction_ = BConstants::ExtensionAction::INVALID;
        HILOGI("end");
    } catch (const BError &e) {
        return;
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return;
    } catch (...) {
        HILOGE("");
        return;
    }
}

bool ExtBackup::WasFromSpecialVersion(void)
{
    if (appVersionCode_ == BConstants::DEFAULT_VERSION_CODE && appVersionStr_ == BConstants::DEFAULT_VERSION_NAME) {
        return true;
    }
    return false;
}

bool ExtBackup::SpeicalVersionForCloneAndCloud(void)
{
    auto iter =
        find_if(BConstants::DEFAULT_VERSION_NAMES_VEC.begin(), BConstants::DEFAULT_VERSION_NAMES_VEC.end(),
                [appVersionStr {appVersionStr_}](const auto &versionName) { return versionName == appVersionStr; });
    if (appVersionCode_ == BConstants::DEFAULT_VERSION_CODE && iter != BConstants::DEFAULT_VERSION_NAMES_VEC.end()) {
        return true;
    }
    return false;
}

bool ExtBackup::RestoreDataReady()
{
    return restoreType_ == RestoreTypeEnum::RESTORE_DATA_READDY;
}

ErrCode ExtBackup::OnBackup(function<void()> callback)
{
    HILOGI("BackupExtensionAbility(base) OnBackup.");
    return ERR_OK;
}

ErrCode ExtBackup::OnRestore(function<void()> callback, std::function<void(const std::string)> callbackEx,
    std::function<void()> callbackExAppDone)
{
    HILOGI("BackupExtensionAbility(base) OnRestore with Ex.");
    return ERR_OK;
}

ErrCode ExtBackup::OnRestore(function<void()> callback)
{
    HILOGI("BackupExtensionAbility(base) OnRestore.");
    return ERR_OK;
}

ErrCode ExtBackup::GetBackupInfo(function<void(std::string)> callback)
{
    HILOGI("BackupExtensionAbility(base) GetBackupInfo.");
    return ERR_OK;
}

ErrCode ExtBackup::CallExtRestore(std::string result)
{
    HILOGI("BackupExtensionAbility(base) CallExtRestore.");
    return ERR_OK;
}

} // namespace OHOS::FileManagement::Backup
