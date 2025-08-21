/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "backup_session.h"
#include <array>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include "ani_utils.h"
#include "b_sa/b_sa_utils.h"
#include "filemgmt_libhilog.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "native_engine/native_engine.h"

namespace OHOS::FileManagement::Backup {
using namespace LibN;

namespace {
const char *BACKUP_SESSION_CLEANER_CLASS_NAME = "L@ohos/backup/transfer/backup/BackupSessionCleaner;";
constexpr int32_t E_OK = 0;
}

void BackupSession::Init(ani_env *aniEnv)
{
    HILOGD("Init BackupSession begin");
    if (aniEnv == nullptr) {
        HILOGE("aniEnv is null");
        return;
    }
    ani_class clsCleaner = AniUtils::GetAniClsByName(aniEnv, BACKUP_SESSION_CLEANER_CLASS_NAME);
    if (clsCleaner == nullptr) {
        return;
    }
    std::array cleanNativeFuncs = {
        ani_native_function {"clean", ":V", reinterpret_cast<void*>(BackupSessionCleaner::Clean)},
    };
    auto status = aniEnv->Class_BindNativeMethods(clsCleaner, cleanNativeFuncs.data(), cleanNativeFuncs.size());
    if (status != ANI_OK) {
        HILOGE("Class_BindNativeMethods failed status: %{public}d", status);
        return;
    }
    HILOGD("Init BackupSession end");
}

void BackupSessionCleaner::Clean(ani_env *aniEnv, ani_object object)
{
    ani_status ret = ANI_ERROR;
    ani_long session {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(object, "session", &session)) != ANI_OK) {
        HILOGE("get field session failed, ret:%{public}d", ret);
    } else {
        BackupSession* sessionPtr = reinterpret_cast<BackupSession*>(session);
        if (sessionPtr != nullptr) {
            delete sessionPtr;
        }
    }
    ani_long callbacks {};
    if ((ret = aniEnv->Object_GetPropertyByName_Long(object, "callbacks", &callbacks)) != ANI_OK) {
        HILOGE("get field callbacks failed, ret:%{public}d", ret);
    } else {
        GeneralCallbacks* callbackPtr = reinterpret_cast<GeneralCallbacks*>(callbacks);
        if (callbackPtr != nullptr) {
            delete callbackPtr;
        }
    }
}
}