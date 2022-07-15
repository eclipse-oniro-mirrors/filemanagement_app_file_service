/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "ext_backup_js.h"

#include <cstdio>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_usr_config.h"
#include "b_resources/b_constants.h"
#include "b_tarball/b_tarball_factory.h"
#include "bundle_mgr_client.h"
#include "directory_ex.h"
#include "ext_extension.h"
#include "filemgmt_libhilog.h"
#include "js_runtime_utils.h"
#include "service_proxy.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void ExtBackupJs::OnStart(const AAFwk::Want &want)
{
    HILOGI("Boot 9# BackupExtensionAbility(JS) was started");
    Extension::OnStart(want);
}

static string GetSrcPath(const AppExecFwk::AbilityInfo &info)
{
    using AbilityRuntime::Extension;
    stringstream ss;

    // API9(stage model) 中通过 $(module)$(name)/$(srcEntrance/(.*$)/(.abc)) 获取自定义插件路径
    if (!info.srcEntrance.empty()) {
        ss << info.moduleName << '/' << string(info.srcEntrance, 0, info.srcEntrance.rfind(".")) << ".abc";
        return ss.str();
    }

    // REM: 否则?????
    return "";
}

void ExtBackupJs::Init(const shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
                       const shared_ptr<AppExecFwk::OHOSApplication> &application,
                       shared_ptr<AppExecFwk::AbilityHandler> &handler,
                       const sptr<IRemoteObject> &token)
{
    HILOGI("Boot 6# Init the BackupExtensionAbility(JS)");
    try {
        ExtBackup::Init(record, application, handler, token);

        if (!abilityInfo_) {
            throw BError(BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid abilityInfo_");
        }

        // 获取应用扩展的 BackupExtensionAbility 的路径
        const AppExecFwk::AbilityInfo &info = *abilityInfo_;
        string bundleName = info.bundleName;
        string moduleName(info.moduleName + "::" + info.name);
        string modulePath = GetSrcPath(info);
        int moduleType = static_cast<int>(info.type);
        HILOGI("Boot 10# Try to load %{public}s's %{public}s(type %{public}d) from %{public}s", bundleName.c_str(),
               moduleName.c_str(), moduleType, modulePath.c_str());

        // 加载用户扩展 BackupExtensionAbility 到 JS 引擎，并将之暂存在 jsObj_ 中。注意，允许加载失败，往后执行默认逻辑
        AbilityRuntime::HandleScope handleScope(jsRuntime_);
        jsObj_ = jsRuntime_.LoadModule(moduleName, modulePath);
        if (jsObj_) {
            HILOGI("Wow! Here's a custsom BackupExtensionAbility");
        } else {
            HILOGW("Oops! There's no custom BackupExtensionAbility");
        }
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    }
}

tuple<ErrCode, NativeValue *> ExtBackupJs::CallObjectMethod(string_view name, const vector<NativeValue *> &argv)
{
    HILOGI("Call %{public}s", name.data());

    if (!jsObj_) {
        return {BError(BError::Codes::EXT_BROKEN_FRAMEWORK, "Invalid jsObj_").GetCode(), nullptr};
    }

    AbilityRuntime::HandleScope handleScope(jsRuntime_);

    NativeValue *value = jsObj_->Get();
    NativeObject *obj = AbilityRuntime::ConvertNativeValueTo<NativeObject>(value);
    if (!obj) {
        return {BError(BError::Codes::EXT_INVAL_ARG, "The custom BackupAbilityExtension is required to be an object")
                    .GetCode(),
                nullptr};
    }

    NativeValue *method = obj->GetProperty(name.data());
    if (!method || method->TypeOf() != NATIVE_FUNCTION) {
        return {BError(BError::Codes::EXT_INVAL_ARG, string(name).append(" is required to be a function")).GetCode(),
                nullptr};
    }

    auto ret = jsRuntime_.GetNativeEngine().CallFunction(value, method, argv.data(), argv.size());
    if (!ret) {
        return {BError(BError::Codes::EXT_INVAL_ARG, string(name).append(" raised an exception")).GetCode(), nullptr};
    }
    return {BError(BError::Codes::OK).GetCode(), ret};
}

int ExtBackupJs::HandleBackup(const BJsonEntityUsrConfig &usrConfig)
{
    HILOGI("Do backup");

    try {
        (void)CallObjectMethod("onBackup");

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BAKCUP);
        if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
            stringstream ss;
            ss << "Failed to create folder backup. ";
            ss << std::generic_category().message(errno);
            throw BError(BError::Codes::SA_BROKEN_ROOT_DIR, ss.str());
        }
        // REM: 打包（处理includeDir, excludeDir），反馈，退出进程
        string pkgName = "1.tar";
        string tarName = path.append("/").append(pkgName);
        string root = "/";

        vector<string> includes = usrConfig.GetIncludes();
        vector<string> excludes = usrConfig.GetExcludes();

        auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
        (tarballFunc->tar)(root, {includes.begin(), includes.end()}, {excludes.begin(), excludes.end()});
        if (chmod(tarName.data(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0) {
            throw BError(errno);
        }

        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }

        UniqueFd fd(open(tarName.data(), O_RDONLY));
        if (fd < 0) {
            throw BError(BError::Codes::EXT_INVAL_ARG, std::generic_category().message(errno));
        }

        ErrCode ret = proxy->AppFileReady(pkgName, move(fd));
        if (SUCCEEDED(ret)) {
            HILOGI("The application is packaged successfully, package name is %{public}s", pkgName.c_str());
        } else {
            HILOGI(
                "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
                "%{public}d",
                ret);
        }
        return ret;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("");
        return EPERM;
    }
}

int ExtBackupJs::HandleRestore()
{
    HILOGI("Do restore");

    try {
        // REM: 给定version
        // REM: 解压启动Extension时即挂载好的备份目录中的数据
        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);

        auto [errCode, files] = BDir::GetDirFiles(path);
        if (errCode) {
            throw BError(errCode);
        }
        if (files.empty()) {
            HILOGI("No .tar package is found. The directory is %{public}s", path.c_str());
        }

        for (auto &tarName : files) {
            if (ExtractFileExt(tarName) != "tar")
                continue;

            auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
            (tarballFunc->untar)("/");
            HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());
        }

        return ERR_OK;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("");
        return EPERM;
    }
}

static void Verify(const AAFwk::Want &want, std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo)
{
    if (!abilityInfo) {
        string pendingMsg = "Received an empty ability. You must missed the init proc";
        throw BError(BError::Codes::EXT_INVAL_ARG, pendingMsg);
    }

    // 发起者必须是备份服务
    // REM: 1089替换成宏，取消1000
    if (int uid = want.GetIntParam(AAFwk::Want::PARAM_RESV_CALLER_UID, -1); uid == -1 || (uid != 1089 && uid != 1000)) {
        string pendingMsg = string("Want must come from the backup sa instead of ").append(to_string(uid));
        throw BError(BError::Codes::EXT_INVAL_ARG, pendingMsg);
    }

    // 应用必须配置支持备份恢复
    // REM：读取配置文件，确认是否需要备份
}

void ExtBackupJs::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    HILOGI("Main 9# BackupExtensionAbility(JS) was invoked. restart=%{public}d, startId=%{public}d", restart, startId);
    int ret {EPERM};
    try {
        // JS 主线程是单线程的，所以这里无需做并发控制
        Verify(want, abilityInfo_);

        using namespace BConstants;
        ExtensionAction extAction {want.GetIntParam(EXTENSION_ACTION_PARA, static_cast<int>(ExtensionAction::INVALID))};
        if (extAction == ExtensionAction::INVALID) {
            int extActionInt = static_cast<int>(extAction);
            string pendingMsg = string("Want must specify a valid action instead of ").append(to_string(extActionInt));
            throw BError(BError::Codes::EXT_INVAL_ARG, pendingMsg);
        }

        vector<string> out;
        AppExecFwk::BundleMgrClient client;
        AppExecFwk::AbilityInfo &info = *abilityInfo_;
        if (!client.GetProfileFromAbility(info, "ohos.extension.backup", out)) {
            throw BError(BError::Codes::EXT_INVAL_ARG, "Failed to invoke the GetResConfigFile method.");
        }
        if (out.size()) {
            BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(out[0]);
            auto cache = cachedEntity.Structuralize();
            if (cache.GetAllowToBackupRestore()) {
                if (extAction == ExtensionAction::BACKUP) {
                    ret = HandleBackup(cache);
                } else if (extAction == ExtensionAction::RESTORE) {
                    ret = HandleRestore();
                } else if (extAction == ExtensionAction::CLEAR) {
                    HandleClear();
                    // 清理任务无需反馈结果，因此直接退出
                    return;
                }
            }
        }
    } catch (const BError &e) {
        ret = e.GetCode();
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    } catch (...) {
        HILOGE("");
    }
    // REM: 处理返回结果 ret
    auto proxy = ServiceProxy::GetInstance();
    if (proxy == nullptr) {
        HILOGE("Failed to obtain the ServiceProxy handle");
    } else {
        proxy->AppDone(ret);
    }
    // REM: 通过杀死进程实现 Stop
}

ExtBackupJs *ExtBackupJs::Create(const unique_ptr<AbilityRuntime::Runtime> &runtime)
{
    HILOGI("Boot 5# Create as an BackupExtensionAbility(JS)");
    return new ExtBackupJs(static_cast<AbilityRuntime::JsRuntime &>(*runtime));
}

ErrCode ExtBackupJs::HandleClear()
{
    HILOGI("Do clear");

    try {
        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BAKCUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);

        if (!ForceRemoveDirectory(backupCache)) {
            HILOGI("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectory(restoreCache)) {
            HILOGI("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }

        return ERR_OK;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("");
        return EPERM;
    }
}

sptr<IRemoteObject> ExtBackupJs::OnConnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin");
        Extension::OnConnect(want);

        auto remoteObject =
            sptr<BackupExtExtension>(new BackupExtExtension(std::static_pointer_cast<ExtBackupJs>(shared_from_this())));
        // REM:经过测试remoteObject返回为空 OnConnect也是连接成功的
        if (remoteObject == nullptr) {
            HILOGI("%{public}s No memory allocated for BackupExtExtension", __func__);
            return nullptr;
        }
        HILOGI("end");
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

void ExtBackupJs::OnDisconnect(const AAFwk::Want &want)
{
    try {
        HILOGI("begin");
        Extension::OnDisconnect(want);
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

UniqueFd ExtBackupJs::GetFileHandle(string &fileName)
{
    HILOGI("begin fileName = %{public}s", fileName.data());
    return UniqueFd(-1);
}
} // namespace OHOS::FileManagement::Backup