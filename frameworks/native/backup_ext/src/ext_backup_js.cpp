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

#include "ext_backup_js.h"

#include <cstdio>
#include <memory>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "bundle_mgr_client.h"
#include "ext_backup_context_js.h"
#include "js_extension_context.h"
#include "js_native_api.h"
#include "js_native_api_types.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_remote_object.h"
#include "unique_fd.h"

#include "b_error/b_error.h"
#include "b_error/b_excep_utils.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_extension_config.h"
#include "b_resources/b_constants.h"
#include "ext_extension.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
constexpr size_t ARGC_ONE = 1;

static string GetSrcPath(const AppExecFwk::AbilityInfo &info)
{
    using AbilityRuntime::Extension;
    stringstream ss;

    // API9(stage model) 中通过 $(module)$(name)/$(srcEntrance/(.*$)/(.abc)) 获取自定义插件路径
    if (!info.srcEntrance.empty()) {
        ss << info.moduleName << '/' << string(info.srcEntrance, 0, info.srcEntrance.rfind(".")) << ".abc";
        return ss.str();
    }
    return "";
}

static napi_status DealNapiStrValue(napi_env env, const napi_value napi_StrValue, std::string &result)
{
    HILOGI("Start DealNapiStrValue");
    std::string buffer = "";
    size_t bufferSize = 0;
    napi_status status = napi_ok;
    status = napi_get_value_string_utf8(env, napi_StrValue, nullptr, -1, &bufferSize);
    if (status != napi_ok) {
        HILOGE("Can not get buffer size");
        return status;
    }
    buffer.reserve(bufferSize + 1);
    buffer.resize(bufferSize);
    if (bufferSize > 0) {
        status = napi_get_value_string_utf8(env, napi_StrValue, buffer.data(), bufferSize + 1, &bufferSize);
        if (status != napi_ok) {
            HILOGE("Can not get buffer value");
            return status;
        }
    }
    result = buffer;
    return status;
}

static napi_value PromiseCallback(napi_env env, napi_callback_info info)
{
    HILOGI("Promise callback.");
    void *data = nullptr;
    if (napi_get_cb_info(env, info, nullptr, 0, nullptr, &data) != napi_ok) {
        HILOGE("Failed to get callback info.");
        return nullptr;
    }
    auto *callbackInfo = static_cast<CallbackInfo *>(data);
    if (callbackInfo == nullptr) {
        HILOGE("CallbackInfo is nullptr");
        return nullptr;
    }
    callbackInfo->callback();
    data = nullptr;
    return nullptr;
}

static napi_value PromiseCallbackEx(napi_env env, napi_callback_info info)
{
    HILOGI("PromiseEx callback.");
    void *data = nullptr;
    std::string str;
    size_t argc = 1;
    napi_value argv = {nullptr};
    NAPI_CALL_NO_THROW(napi_get_cb_info(env, info, &argc, &argv, nullptr, &data), nullptr);
    auto *callbackInfoEx = static_cast<CallbackInfoEx *>(data);
    if (callbackInfoEx == nullptr) {
        HILOGE("CallbackInfo is nullPtr");
        return nullptr;
    }
    DealNapiStrValue(env, argv, str);
    callbackInfoEx->callbackParam(str);
    data = nullptr;
    return nullptr;
}

static bool CheckPromise(napi_env env, napi_value value)
{
    if (value == nullptr) {
        HILOGE("CheckPromise, result is null, no need to call promise.");
        return false;
    }
    bool isPromise = false;
    if (napi_is_promise(env, value, &isPromise) != napi_ok) {
        HILOGE("CheckPromise, result is not promise, no need to call promise.");
        return false;
    }
    return isPromise;
}

static bool CallPromise(AbilityRuntime::JsRuntime &jsRuntime, napi_value result, CallbackInfo *callbackInfo)
{
    AbilityRuntime::HandleScope handleScope(jsRuntime);
    auto env = jsRuntime.GetNapiEnv();
    napi_value method = nullptr;
    if (napi_get_named_property(env, result, "then", &method) != napi_ok) {
        HILOGI("CallPromise, Failed to get method then");
        return false;
    }
    bool isCallable = false;
    if (napi_is_callable(env, method, &isCallable) != napi_ok) {
        HILOGI("CallPromise, Failed to check method then is callable");
        return false;
    }
    if (!isCallable) {
        HILOGI("CallPromise, property then is not callable.");
        return false;
    }
    napi_value ret;
    napi_create_function(env, "promiseCallback", strlen("promiseCallback"), PromiseCallback, callbackInfo, &ret);
    napi_value argv[1] = {ret};
    napi_call_function(env, result, method, 1, argv, nullptr);
    return true;
}

static bool CallPromiseEx(AbilityRuntime::JsRuntime &jsRuntime, napi_value result, CallbackInfoEx *callbackInfoEx)
{
    AbilityRuntime::HandleScope handleScope(jsRuntime);
    auto env = jsRuntime.GetNapiEnv();
    napi_value method = nullptr;
    if (napi_get_named_property(env, result, "then", &method) != napi_ok) {
        HILOGI("CallPromise, Failed to get method then");
        return false;
    }
    bool isCallable = false;
    if (napi_is_callable(env, method, &isCallable) != napi_ok) {
        HILOGI("CallPromise, Failed to check method then is callable");
        return false;
    }
    if (!isCallable) {
        HILOGI("CallPromise, property then is not callable.");
        return false;
    }
    napi_value ret;
    napi_create_function(env, "promiseCallbackEx", strlen("promiseCallbackEx"), PromiseCallbackEx, callbackInfoEx,
        &ret);
    napi_value argv[1] = {ret};
    napi_call_function(env, result, method, 1, argv, nullptr);
    return true;
}

static bool CallPromiseEx(AbilityRuntime::JsRuntime &jsRuntime, napi_value result,
    CallbackInfoBackup *callbackInfoBackup)
{
    AbilityRuntime::HandleScope handleScope(jsRuntime);
    auto env = jsRuntime.GetNapiEnv();
    napi_value method = nullptr;
    if (napi_get_named_property(env, result, "then", &method) != napi_ok) {
        HILOGI("CallPromise, Failed to get method then");
        return false;
    }
    bool isCallable = false;
    if (napi_is_callable(env, method, &isCallable) != napi_ok) {
        HILOGI("CallPromise, Failed to check method then is callable");
        return false;
    }
    if (!isCallable) {
        HILOGI("CallPromise, property then is not callable.");
        return false;
    }
    napi_value ret;
    napi_create_function(env, "promiseCallbackEx", strlen("promiseCallbackEx"), PromiseCallbackEx, callbackInfoBackup,
        &ret);
    napi_value argv[1] = {ret};
    napi_call_function(env, result, method, 1, argv, nullptr);
    return true;
}

void ExtBackupJs::Init(const shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                       const shared_ptr<AppExecFwk::OHOSApplication> &application,
                       shared_ptr<AppExecFwk::AbilityHandler> &handler,
                       const sptr<IRemoteObject> &token)
{
    HILOGI("Init the BackupExtensionAbility(JS)");
    try {
        ExtBackup::Init(record, application, handler, token);
        BExcepUltils::BAssert(abilityInfo_, BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
        // 获取应用扩展的 BackupExtensionAbility 的路径
        const AppExecFwk::AbilityInfo &info = *abilityInfo_;
        string bundleName = info.bundleName;
        string moduleName(info.moduleName + "::" + info.name);
        string modulePath = GetSrcPath(info);
        int moduleType = static_cast<int>(info.type);
        HILOGI("Try to load %{public}s's %{public}s(type %{public}d) from %{public}s", bundleName.c_str(),
               moduleName.c_str(), moduleType, modulePath.c_str());

        // 加载用户扩展 BackupExtensionAbility 到 JS 引擎，并将之暂存在 jsObj_ 中。注意，允许加载失败，往后执行默认逻辑
        AbilityRuntime::HandleScope handleScope(jsRuntime_);
        jsObj_ = jsRuntime_.LoadModule(moduleName, modulePath, info.hapPath,
                                       abilityInfo_->compileMode == AbilityRuntime::CompileMode::ES_MODULE);
        if (jsObj_ == nullptr) {
            HILOGW("Oops! There's no custom BackupExtensionAbility");
            return;
        }
        HILOGI("Wow! Here's a custsom BackupExtensionAbility");
        ExportJsContext();
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    }
}

napi_value AttachBackupExtensionContext(napi_env env, void *value, void *)
{
    HILOGI("AttachBackupExtensionContext");
    if (value == nullptr || env == nullptr) {
        HILOG_WARN("invalid parameter.");
        return nullptr;
    }
    auto ptr = reinterpret_cast<std::weak_ptr<ExtBackupContext> *>(value)->lock();
    if (ptr == nullptr) {
        HILOGE("invalid context.");
        return nullptr;
    }
    auto object = CreateExtBackupJsContext(env, ptr);
    if (object == nullptr) {
        HILOGE("Failed to get js backup extension context");
        return nullptr;
    }
    auto contextRef =
        AbilityRuntime::JsRuntime::LoadSystemModuleByEngine(env, "application.BackupExtensionContext", &object, 1);
    napi_value contextObj = contextRef->GetNapiValue();
    napi_coerce_to_native_binding_object(env, contextObj, AbilityRuntime::DetachCallbackFunc,
                                         AttachBackupExtensionContext, value, nullptr);

    auto workContext = new (std::nothrow) std::weak_ptr<ExtBackupContext>(ptr);
    if (workContext == nullptr) {
        HILOGE("Failed to get backup extension context");
        return nullptr;
    }
    napi_wrap(
        env, contextObj, workContext,
        [](napi_env, void *data, void *) {
            HILOG_DEBUG("Finalizer for weak_ptr base context is called");
            delete static_cast<std::weak_ptr<ExtBackupContext> *>(data);
        },
        nullptr, nullptr);
    return contextObj;
}

void ExtBackupJs::ExportJsContext(void)
{
    auto env = jsRuntime_.GetNapiEnv();
    napi_value obj = jsObj_->GetNapiValue();
    if (obj == nullptr) {
        HILOGE("Failed to get BackupExtAbility object");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        HILOGE("Failed to get context");
        return;
    }

    HILOGI("CreateBackupExtAbilityContext");
    napi_value contextObj = CreateExtBackupJsContext(env, context);
    auto contextRef = jsRuntime_.LoadSystemModule("application.BackupExtensionContext", &contextObj, ARGC_ONE);
    if (!contextRef) {
        HILOGE("context is nullptr");
        return;
    }
    contextObj = contextRef->GetNapiValue();
    HILOGI("Bind context");
    context->Bind(jsRuntime_, contextRef.release());
    napi_set_named_property(env, obj, "context", contextObj);

    auto workContext = new (std::nothrow) std::weak_ptr<ExtBackupContext>(context);

    napi_coerce_to_native_binding_object(env, contextObj, AbilityRuntime::DetachCallbackFunc,
                                         AttachBackupExtensionContext, workContext, nullptr);
    HILOGI("Set backup extension ability context pointer is nullptr: %{public}d", context.get() == nullptr);
    napi_wrap(
        env, contextObj, workContext,
        [](napi_env, void *data, void *) {
            HILOG_DEBUG("Finalizer for weak_ptr base context is called");
            delete static_cast<std::weak_ptr<ExtBackupContext> *>(data);
        },
        nullptr, nullptr);
}

[[maybe_unused]] tuple<ErrCode, napi_value> ExtBackupJs::CallObjectMethod(string_view name,
                                                                          const vector<napi_value> &argv)
{
    HILOGI("Call %{public}s", name.data());
    return {BError(BError::Codes::OK).GetCode(), nullptr};
}

ExtBackupJs *ExtBackupJs::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Create as an BackupExtensionAbility(JS)");
    return new ExtBackupJs(static_cast<AbilityRuntime::JsRuntime &>(*runtime));
}

ErrCode ExtBackupJs::OnBackup(function<void()> callback)
{
    HILOGI("BackupExtensionAbility(JS) OnBackup.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onBackup interface.");
    callbackInfo_ = std::make_shared<CallbackInfo>(callback);
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfo {callbackInfo_}](napi_env env,
                                                                             napi_value result) -> bool {
        if (!CheckPromise(env, result)) {
            callbackInfo->callback();
            return true;
        }
        HILOGI("CheckPromise(JS) OnBackup ok.");
        return CallPromise(*jsRuntime, result, callbackInfo.get());
    };

    auto errCode = CallJsMethod("onBackup", jsRuntime_, jsObj_.get(), {}, retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
    }
    return errCode;
}

ErrCode ExtBackupJs::OnRestore(function<void()> callback, std::function<void(const std::string)> callbackEx,
    std::function<void()> callbackExAppDone)
{
    HILOGI("BackupExtensionAbility(JS) OnRestore.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onRestore interface.");
    needCallOnRestore_.store(false);
    callbackInfo_ = std::make_shared<CallbackInfo>(callback);
    callbackInfoEx_ = std::make_shared<CallbackInfoEx>(callbackEx, callbackExAppDone);
    return CallJSRestoreEx();
}

ErrCode ExtBackupJs::OnRestore(function<void()> callback)
{
    HILOGI("Start execute BackupExtensionAbility(JS) OnRestore.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the onRestore interface.");
    auto argParser = [appVersionCode(appVersionCode_),
        appVersionStr(appVersionStr_)](napi_env env, vector<napi_value> &argv) -> bool {
        napi_value objValue = nullptr;
        napi_create_object(env, &objValue);
        napi_set_named_property(env, objValue, "code", AbilityRuntime::CreateJsValue(env, appVersionCode));
        napi_set_named_property(env, objValue, "name", AbilityRuntime::CreateJsValue(env, appVersionStr.c_str()));
        argv.push_back(objValue);
        return true;
    };
    callbackInfo_ = std::make_shared<CallbackInfo>(callback);
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfo {callbackInfo_}](napi_env env, napi_value result) -> bool {
        if (!CheckPromise(env, result)) {
            HILOGI("CheckPromise false");
            callbackInfo->callback();
            return true;
        }
        HILOGI("CheckPromise Js Method onRestore ok.");
        return CallPromise(*jsRuntime, result, callbackInfo.get());
    };
    auto errCode = CallJsMethod("onRestore", jsRuntime_, jsObj_.get(), argParser, retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ExtBackupJs::CallJSRestoreEx()
{
    HILOGI("Start call app js method onRestoreEx");
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfoEx {callbackInfoEx_}](napi_env envir, napi_value result) ->
        bool {
        if (!CheckPromise(envir, result)) {
            HILOGI("CheckPromise onRestoreEx false");
            std::string str;
            DealNapiStrValue(envir, result, str);
            callbackInfoEx->callbackParam(str);
            return true;
        }
        HILOGI("CheckPromise onRestoreEx ok");
        return CallPromiseEx(*jsRuntime, result, callbackInfoEx.get());
    };
    auto errCode = CallJsMethod("onRestoreEx", jsRuntime_, jsObj_.get(), ParseRestoreExInfo(), retParser);
    if (errCode != ERR_OK) {
        HILOGE("Call onRestoreEx error");
        return errCode;
    }
    if (!needCallOnRestore_.load()) {
        if (callbackInfoEx_) {
            HILOGI("Will call app done");
            callbackInfoEx_->callbackAppDone();
        }
        HILOGI("Call Js method onRestoreEx done");
        return ERR_OK;
    }
    return CallJSRestore();
}

ErrCode ExtBackupJs::CallJSRestore()
{
    HILOGI("Start call app js method onRestore");
    auto retParser = [jsRuntime {&jsRuntime_}, callbackInfo {callbackInfo_}](napi_env env, napi_value result) -> bool {
        if (!CheckPromise(env, result)) {
            HILOGI("onRestore, CheckPromise false");
            callbackInfo->callback();
            return true;
        }
        HILOGI("CheckPromise Js Method onRestore ok.");
        return CallPromise(*jsRuntime, result, callbackInfo.get());
    };
    auto errCode = CallJsMethod("onRestore", jsRuntime_, jsObj_.get(), ParseRestoreInfo(), retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ExtBackupJs::GetBackupInfo(std::function<void(const std::string)> callback)
{
    HILOGI("BackupExtensionAbility(JS) GetBackupInfo begin.");
    BExcepUltils::BAssert(jsObj_, BError::Codes::EXT_BROKEN_FRAMEWORK,
                          "The app does not provide the GetBackupInfo interface.");
    callbackInfoBackup_ = std::make_shared<CallbackInfoBackup>(callback);
    auto retParser = [jsRuntime {&jsRuntime_}, callBackInfo {callbackInfoBackup_}](napi_env env,
                                                                             napi_value result) -> bool {
        if (!CheckPromise(env, result)) {
            size_t strLen = 0;
            napi_status status = napi_get_value_string_utf8(env, result, nullptr, -1, &strLen);
            if (status != napi_ok) {
                return false;
            }
            size_t bufLen = strLen + 1;
            unique_ptr<char[]> str = make_unique<char[]>(bufLen);
            status = napi_get_value_string_utf8(env, result, str.get(), bufLen, &strLen);
            callBackInfo->callbackParam(str.get());
            return true;
        }
        HILOGI("BackupExtensionAbulity(JS) GetBackupInfo ok.");
        return CallPromiseEx(*jsRuntime, result, callBackInfo.get());
    };

    auto errCode = CallJsMethod("getBackupInfo", jsRuntime_, jsObj_.get(), {}, retParser);
    if (errCode != ERR_OK) {
        HILOGE("CallJsMethod error, code:%{public}d.", errCode);
    }
    HILOGI("BackupExtensionAbulity(JS) GetBackupInfo end.");
    return errCode;
}

static int DoCallJsMethod(CallJsParam *param)
{
    AbilityRuntime::JsRuntime *jsRuntime = param->jsRuntime;
    if (jsRuntime == nullptr) {
        HILOGE("failed to get jsRuntime.");
        return EINVAL;
    }
    AbilityRuntime::HandleEscape handleEscape(*jsRuntime);
    auto env = jsRuntime->GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        HILOGE("scope is nullptr");
        return EINVAL;
    }
    vector<napi_value> argv = {};
    if (param->argParser != nullptr) {
        if (!param->argParser(env, argv)) {
            HILOGE("failed to get params.");
            napi_close_handle_scope(env, scope);
            return EINVAL;
        }
    }
    napi_value value = param->jsObj->GetNapiValue();
    if (value == nullptr) {
        HILOGE("failed to get napi value object.");
        napi_close_handle_scope(env, scope);
        return EINVAL;
    }
    napi_status status;
    napi_value method;
    status = napi_get_named_property(env, value, param->funcName.c_str(), &method);
    if (status != napi_ok || param->retParser == nullptr) {
        HILOGE("ResultValueParser must not null.");
        napi_close_handle_scope(env, scope);
        return EINVAL;
    }
    napi_value result;
    napi_call_function(env, value, method, argv.size(), argv.data(), &result);
    if (!param->retParser(env, handleEscape.Escape(result))) {
        HILOGI("Parser js result fail.");
        napi_close_handle_scope(env, scope);
        return EINVAL;
    }
    napi_close_handle_scope(env, scope);
    return ERR_OK;
}

int ExtBackupJs::CallJsMethod(const std::string &funcName,
                              AbilityRuntime::JsRuntime &jsRuntime,
                              NativeReference *jsObj,
                              InputArgsParser argParser,
                              ResultValueParser retParser)
{
    uv_loop_s *loop = nullptr;
    napi_status status = napi_get_uv_event_loop(jsRuntime.GetNapiEnv(), &loop);
    if (status != napi_ok) {
        HILOGE("failed to get uv event loop.");
        return EINVAL;
    }
    auto param = std::make_shared<CallJsParam>(funcName, &jsRuntime, jsObj, argParser, retParser);
    BExcepUltils::BAssert(param, BError::Codes::EXT_BROKEN_FRAMEWORK, "failed to new param.");

    auto work = std::make_shared<uv_work_t>();
    BExcepUltils::BAssert(work, BError::Codes::EXT_BROKEN_FRAMEWORK, "failed to new uv_work_t.");

    work->data = reinterpret_cast<void *>(param.get());
    HILOGI("Will execute current js method");
    int ret = uv_queue_work(
        loop, work.get(), [](uv_work_t *work) {},
        [](uv_work_t *work, int status) {
            CallJsParam *param = reinterpret_cast<CallJsParam *>(work->data);
            do {
                if (param == nullptr) {
                    HILOGE("failed to get CallJsParam.");
                    break;
                }
                HILOGI("Start call current js method");
                if (DoCallJsMethod(param) != ERR_OK) {
                    HILOGE("failed to call DoCallJsMethod.");
                }
            } while (false);
            param->backupOperateCondition.notify_one();
        });
    if (ret != 0) {
        HILOGE("failed to exec uv_queue_work.");
        return EINVAL;
    }
    HILOGI("Wait execute current js method");
    std::unique_lock<std::mutex> lock(param->backupOperateMutex);
    param->backupOperateCondition.wait(lock);
    return ERR_OK;
}

std::function<bool(napi_env env, std::vector<napi_value> &argv)> ExtBackupJs::ParseRestoreExInfo()
{
    auto onRestoreExFun = [appVersionCode(appVersionCode_), appVersionStr(appVersionStr_),
        restoreRetInfo(restoreRetInfo_)](napi_env env, vector<napi_value> &argv) -> bool {
        napi_value objValue = nullptr;
        napi_value restoreRetValue = nullptr;
        napi_create_object(env, &objValue);
        napi_create_object(env, &restoreRetValue);
        napi_set_named_property(env, objValue, "code", AbilityRuntime::CreateJsValue(env, appVersionCode));
        napi_set_named_property(env, objValue, "name", AbilityRuntime::CreateJsValue(env, appVersionStr.c_str()));
        napi_create_string_utf8(env, restoreRetInfo.c_str(), restoreRetInfo.size(), &restoreRetValue);
        argv.push_back(objValue);
        argv.push_back(restoreRetValue);
        return true;
    };
    return onRestoreExFun;
}

std::function<bool(napi_env env, std::vector<napi_value> &argv)> ExtBackupJs::ParseRestoreInfo()
{
    auto onRestoreFun = [appVersionCode(appVersionCode_), appVersionStr(appVersionStr_)](napi_env env,
        vector<napi_value> &argv) -> bool {
        napi_value objValue = nullptr;
        napi_create_object(env, &objValue);
        napi_set_named_property(env, objValue, "code", AbilityRuntime::CreateJsValue(env, appVersionCode));
        napi_set_named_property(env, objValue, "name", AbilityRuntime::CreateJsValue(env, appVersionStr.c_str()));
        argv.push_back(objValue);
        return true;
    };
    return onRestoreFun;
}

ErrCode ExtBackupJs::CallExtRestore(std::string result)
{
    HILOGI("Start CallExtRestore, result is: %{public}s", result.c_str());
    if (result.size() == 0) {
        needCallOnRestore_.store(true);
    } else {
        needCallOnRestore_.store(false);
    }
    HILOGI("End CallExtRestore");
    return ERR_OK;
}
} // namespace OHOS::FileManagement::Backup
