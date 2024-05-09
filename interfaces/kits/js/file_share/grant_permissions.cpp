/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "grant_permissions.h"

#include <string>
#include <vector>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "js_native_api.h"
#include "log.h"
#include "n_napi.h"
#include "parameter.h"
#include "tokenid_kit.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleFileShare {
using namespace OHOS::FileManagement::LibN;
using namespace std;

namespace {
const std::string FILE_ACCESS_PERMISSION = "ohos.permission.FILE_ACCESS_PERSIST";
const std::string FULL_MOUNT_ENABLE_PARAMETER = "const.filemanager.full_mount.enable";

static bool CheckPermission(const string &permission)
{
    Security::AccessToken::AccessTokenID tokenCaller = IPCSkeleton::GetCallingTokenID();
    return Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenCaller, permission) ==
           Security::AccessToken::PermissionState::PERMISSION_GRANTED;
}

static bool CheckFileManagerFullMountEnable()
{
    char value[] = "false";
    int retSystem = GetParameter(FULL_MOUNT_ENABLE_PARAMETER.c_str(), "false", value, sizeof(value));
    if (retSystem > 0 && !strcmp(value, "true")) {
        LOGE("The full mount enable parameter is true");
        return true;
    }
    LOGD("The full mount enable parameter is false");
    return false;
}
} // namespace

static napi_value GetErrData(napi_env env, deque<struct PolicyErrorResult> &errorResults)
{
    napi_value res = nullptr;
    napi_status status = napi_create_array(env, &res);
    if (status != napi_ok) {
        LOGE("Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (auto &iter : errorResults) {
        NVal obj = NVal::CreateObject(env);
        obj.AddProp("uri", NVal::CreateUTF8String(env, iter.uri).val_);
        obj.AddProp("code", NVal::CreateInt32(env, iter.code).val_);
        obj.AddProp("message", NVal::CreateUTF8String(env, iter.message).val_);
        status = napi_set_element(env, res, index++, obj.val_);
        if (status != napi_ok) {
            LOGE("Failed to set element on data");
            return nullptr;
        }
    }
    return res;
}

static napi_value GetResultData(napi_env env, const vector<bool> &results)
{
    napi_value res = nullptr;
    napi_status status = napi_create_array(env, &res);
    if (status != napi_ok) {
        LOGE("Failed to create array");
        return nullptr;
    }
    size_t index = 0;
    for (const auto &iter : results) {
        napi_value value;
        napi_get_boolean(env, iter, &value);
        status = napi_set_element(env, res, index++, value);
        if (status != napi_ok) {
            LOGE("Failed to set element on data");
            return nullptr;
        }
    }
    return res;
}

static napi_status GetUriPoliciesArg(napi_env env, napi_value agrv, std::vector<UriPolicyInfo> &uriPolicies)
{
    uint32_t count;
    napi_status status = napi_get_array_length(env, agrv, &count);
    if (status != napi_ok) {
        LOGE("get array length failed");
        return status;
    }
    if (count > MAX_ARRAY_SIZE) {
        LOGE("The length of the array is extra-long");
        return napi_invalid_arg;
    }
    for (uint32_t i = 0; i < count; i++) {
        napi_handle_scope scope;
        status = napi_open_handle_scope(env, &scope);
        if (status != napi_ok) {
            return status;
        }
        napi_value object;
        status = napi_get_element(env, agrv, i, &object);
        if (status != napi_ok) {
            LOGE("get element failed");
            return status;
        }
        napi_value uriValue;
        napi_value modeValue;
        status = napi_get_named_property(env, object, "uri", &uriValue);
        if (status != napi_ok) {
            LOGE("get named property failed");
            return status;
        }
        status = napi_get_named_property(env, object, "operationMode", &modeValue);
        if (status != napi_ok) {
            LOGE("get named property failed");
            return status;
        }
        auto [succStr, str, ignore] = NVal(env, uriValue).ToUTF8String();
        auto [succMode, mode] = NVal(env, modeValue).ToUint32();
        if (!succStr || !succMode) {
            LOGE("the argument error");
            return napi_invalid_arg;
        }
        UriPolicyInfo uriPolicy {.uri = str.get(), .mode = mode};
        uriPolicies.emplace_back(uriPolicy);
        status = napi_close_handle_scope(env, scope);
        if (status != napi_ok) {
            return status;
        }
    }
    return napi_ok;
}

napi_value PersistPermission(napi_env env, napi_callback_info info)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this api");
        NError(E_DEVICENOTSUPPORT).ThrowErr(env);
        return nullptr;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        LOGE("PersistPermission has not ohos permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("PersistPermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::PersistPermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "persist_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value RevokePermission(napi_env env, napi_callback_info info)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this api");
        NError(E_DEVICENOTSUPPORT).ThrowErr(env);
        return nullptr;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        LOGE("RevokePermission has not ohos permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("RevokePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::RevokePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "revoke_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value ActivatePermission(napi_env env, napi_callback_info info)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this api");
        NError(E_DEVICENOTSUPPORT).ThrowErr(env);
        return nullptr;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        LOGE("PersistPermission has not ohos permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("ActivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::ActivatePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "activate_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value DeactivatePermission(napi_env env, napi_callback_info info)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this api");
        NError(E_DEVICENOTSUPPORT).ThrowErr(env);
        return nullptr;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        LOGE("PersistPermission has not ohos permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("DeactivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyErrorArgs> arg = make_shared<PolicyErrorArgs>();
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::DeactivatePermission(uriPolicies, arg->errorResults);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (err) {
            if (arg->errNo == EPERM) {
                napi_value data = err.GetNapiErr(env);
                napi_set_named_property(env, data, FILEIO_TAG_ERR_DATA.c_str(), GetErrData(env, arg->errorResults));
                return NVal(env, data);
            }
            return {env, err.GetNapiErr(env)};
        }
        return NVal::CreateUndefined(env);
    };
    const string procedureName = "deactivate_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

napi_value CheckPersistentPermission(napi_env env, napi_callback_info info)
{
    if (!CheckFileManagerFullMountEnable()) {
        LOGE("The device doesn't support this api");
        NError(E_DEVICENOTSUPPORT).ThrowErr(env);
        return nullptr;
    }
    if (!CheckPermission(FILE_ACCESS_PERMISSION)) {
        LOGE("PersistPermission has not ohos permission!");
        NError(E_PERMISSION).ThrowErr(env);
        return nullptr;
    }
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ONE)) {
        LOGE("ActivatePermission Number of arguments unmatched");
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    std::vector<UriPolicyInfo> uriPolicies;
    if (GetUriPoliciesArg(env, funcArg[NARG_POS::FIRST], uriPolicies) != napi_ok) {
        NError(E_PARAMS).ThrowErr(env);
        return nullptr;
    }
    shared_ptr<PolicyInfoResultArgs> arg = make_shared<PolicyInfoResultArgs>();
    auto cbExec = [uriPolicies, arg]() -> NError {
        arg->errNo = FilePermission::CheckPersistentPermission(uriPolicies, arg->resultData);
        return NError(arg->errNo);
    };
    auto cbCompl = [arg](napi_env env, NError err) -> NVal {
        if (arg->errNo != 0) {
            return {env, err.GetNapiErr(env)};
        }
        return {env, GetResultData(env, arg->resultData)};
    };
    const string procedureName = "check_persist_permission";
    NVal thisVar(env, funcArg.GetThisVar());
    return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbCompl).val_;
}

} // namespace ModuleFileShare
} // namespace AppFileService
} // namespace OHOS
