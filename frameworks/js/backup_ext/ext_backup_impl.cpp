/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "native_engine/native_engine.h"

extern const char _binary_backup_extension_ability_js_start[];
extern const char _binary_backup_extension_ability_js_end[];
extern const char _binary_backup_extension_ability_abc_start[];
extern const char _binary_backup_extension_ability_abc_end[];

extern "C" __attribute__((constructor)) void NAPI_application_BackupExtensionAbility_AutoRegister()
{
    auto moduleManager = NativeModuleManager::GetInstance();
    NativeModule newModuleInfo = {
        .name = "application.BackupExtensionAbility",
        .fileName = "application/libbackupextensionability_napi.so/BackupExtensionAbility.js",
    };

    moduleManager->Register(&newModuleInfo);
}

extern "C" __attribute__((visibility("default"))) void
    NAPI_application_BackupExtensionAbility_GetJSCode(const char **buf, int *bufLen)
{
    if (buf != nullptr) {
        *buf = _binary_backup_extension_ability_js_start;
    }

    if (bufLen != nullptr) {
        *bufLen = _binary_backup_extension_ability_js_end - _binary_backup_extension_ability_js_start;
    }
}

extern "C" __attribute__((visibility("default"))) void
    NAPI_application_BackupExtensionAbility_GetABCCode(const char **buf, int *buflen)
{
    if (buf != nullptr) {
        *buf = _binary_backup_extension_ability_abc_start;
    }
    if (buflen != nullptr) {
        *buflen = _binary_backup_extension_ability_abc_end - _binary_backup_extension_ability_abc_start;
    }
}