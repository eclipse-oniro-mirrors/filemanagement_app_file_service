/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include "ext_extension.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_json/b_json_cached_entity.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "b_tarball/b_tarball_factory.h"
#include "bundle_mgr_client.h"
#include "directory_ex.h"
#include "errors.h"
#include "filemgmt_libhilog.h"
#include "ipc_skeleton.h"
#include "service_proxy.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

void BackupExtExtension::VerifyCaller()
{
    HILOGI("begin");
    uint32_t tokenCaller = IPCSkeleton::GetCallingTokenID();
    int tokenType = Security::AccessToken::AccessTokenKit::GetTokenType(tokenCaller);
    if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        throw BError(BError::Codes::EXT_BROKEN_IPC, "Calling tokenType is error");
    } else if (tokenType == Security::AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        if (IPCSkeleton::GetCallingUid() != BConstants::BACKUP_UID) {
            throw BError(BError::Codes::EXT_BROKEN_IPC, "Calling uid is invalid");
        }
    }
}

UniqueFd BackupExtExtension::GetFileHandle(const string &fileName)
{
    if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
        return UniqueFd(-1);
    }

    HILOGI("begin fileName = %{public}s", fileName.data());
    VerifyCaller();
    if (extension_ == nullptr) {
        HILOGE("%{public}s end failed.", __func__);
        return UniqueFd(-1);
    }

    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
        stringstream ss;
        ss << "Failed to create folder backup. ";
        ss << std::generic_category().message(errno);
        throw BError(BError::Codes::EXT_INVAL_ARG, ss.str());
    }

    string tarName = path + fileName;
    return UniqueFd(open(tarName.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
}

ErrCode BackupExtExtension::HandleClear()
{
    HILOGI("begin clear");
    try {
        if (extension_->GetExtensionAction() == BConstants::ExtensionAction::INVALID) {
            return EPERM;
        }
        VerifyCaller();

        string backupCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BAKCUP);
        string restoreCache = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);

        if (!ForceRemoveDirectory(backupCache)) {
            HILOGI("Failed to delete the backup cache %{public}s", backupCache.c_str());
        }

        if (!ForceRemoveDirectory(restoreCache)) {
            HILOGI("Failed to delete the restore cache %{public}s", restoreCache.c_str());
        }

        tars_.clear();
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
        return EPERM;
    } catch (...) {
        HILOGE("");
        return EPERM;
    }

    return ERR_OK;
}

ErrCode IndexFileReady(const string &pkgName, sptr<IService> proxy)
{
    set<string> info;
    info.insert(pkgName);
    string indexFile = string(BConstants::PATH_BUNDLE_BACKUP_HOME)
                           .append(BConstants::SA_BUNDLE_BACKUP_BAKCUP)
                           .append(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(
        UniqueFd(open(indexFile.data(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR)));
    auto cache = cachedEntity.Structuralize();
    cache.SetExtManage(info);
    cachedEntity.Persist();
    close(cachedEntity.GetFd().Release());

    ErrCode ret =
        proxy->AppFileReady(string(BConstants::EXT_BACKUP_MANAGE), UniqueFd(open(indexFile.data(), O_RDONLY)));
    if (SUCCEEDED(ret)) {
        HILOGI("The application is packaged successfully, index json file name is %{public}s", indexFile.c_str());
    } else {
        HILOGI(
            "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
            "%{public}d",
            ret);
    }
    return ret;
}

ErrCode TarFileReady(const string &tarName, const string &pkgName, sptr<IService> proxy)
{
    UniqueFd fd(open(tarName.data(), O_RDONLY));
    if (fd < 0) {
        throw BError(BError::Codes::EXT_INVAL_ARG, std::generic_category().message(errno));
    }

    ErrCode ret = proxy->AppFileReady(pkgName, std::move(fd));
    if (SUCCEEDED(ret)) {
        HILOGI("The application is packaged successfully, package name is %{public}s", pkgName.c_str());
    } else {
        HILOGI(
            "The application is packaged successfully but the AppFileReady interface fails to be invoked: "
            "%{public}d",
            ret);
    }
    return ret;
}

ErrCode BackupExtExtension::PublishFile(const string &fileName)
{
    HILOGI("begin publish file. fileName is %{public}s", fileName.data());
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            return BError(-EPERM);
        }
        VerifyCaller();

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string tarName = path + fileName;
        if (find(tars_.begin(), tars_.end(), fileName) != tars_.end() || access(tarName.data(), F_OK) != 0) {
            return BError(-EPERM);
        }
        tars_.push_back(fileName);

        // 异步执行解压操作
        if (extension_->AllowToBackupRestore()) {
            AsyncTaskRestore(fileName);
        }

        return ERR_OK;
    } catch (const BError &e) {
        return e.GetCode();
    } catch (const exception &e) {
        HILOGE("Catched an unexpected low-level exception %{public}s", e.what());
        return BError(-EPERM);
    } catch (...) {
        HILOGE("Unexpected exception");
        return BError(-EPERM);
    }
}

ErrCode BackupExtExtension::HandleBackup()
{
    string usrConfig = extension_->GetUsrConfig();
    if (!usrConfig.empty()) {
        AsyncTaskBackup(usrConfig);
        return 0;
    }

    return BError(-EPERM);
}

int BackupExtExtension::HandleBackup(const BJsonEntityUsrConfig &usrConfig)
{
    HILOGI("Do backup");
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::BACKUP) {
            return EPERM;
        }

        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_BAKCUP);
        if (mkdir(path.data(), S_IRWXU) && errno != EEXIST) {
            stringstream ss;
            ss << "Failed to create folder backup. ";
            ss << std::generic_category().message(errno);
            throw BError(BError::Codes::EXT_INVAL_ARG, ss.str());
        }

        string pkgName = "1.tar";
        string tarName = path + pkgName;
        string root = "/";

        vector<string> includes = usrConfig.GetIncludes();
        vector<string> excludes = usrConfig.GetExcludes();

        // 打包
        auto tarballTar = BTarballFactory::Create("cmdline", tarName);
        (tarballTar->tar)(root, {includes.begin(), includes.end()}, {excludes.begin(), excludes.end()});
        if (chmod(tarName.data(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0) {
            throw BError(errno);
        }

        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            throw BError(BError::Codes::EXT_BROKEN_BACKUP_SA, std::generic_category().message(errno));
        }

        ErrCode ret = ERR_OK;
        ret = IndexFileReady(pkgName, proxy);
        ret = TarFileReady(tarName, pkgName, proxy);
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

int BackupExtExtension::HandleRestore(const string &fileName)
{
    HILOGI("Do restore");
    try {
        if (extension_->GetExtensionAction() != BConstants::ExtensionAction::RESTORE) {
            return EPERM;
        }
        // REM: 给定version
        // REM: 解压启动Extension时即挂载好的备份目录中的数据
        string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
        string tarName = path + fileName;

        if (ExtractFileExt(tarName) != "tar")
            return EPERM;

        auto tarballFunc = BTarballFactory::Create("cmdline", tarName);
        (tarballFunc->untar)("/");
        HILOGI("Application recovered successfully, package path is %{public}s", tarName.c_str());

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

void BackupExtExtension::AsyncTaskBackup(const string config)
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, config]() {
        auto ptr = obj.promote();
        if (!ptr) {
            HILOGI("ext_extension handle have been already released");
            return;
        }

        BJsonCachedEntity<BJsonEntityUsrConfig> cachedEntity(config);
        auto cache = cachedEntity.Structuralize();
        if (!cache.GetAllowToBackupRestore()) {
            HILOGI("Application does not allow backup or restore");
            return;
        }

        int ret = ptr->HandleBackup(cache);

        // REM: 处理返回结果 ret
        auto proxy = ServiceProxy::GetInstance();
        if (proxy == nullptr) {
            HILOGE("Failed to obtain the ServiceProxy handle");
            return;
        } else {
            proxy->AppDone(ret);
        }
    };

    // REM: 这里异步化了，需要做并发控制
    // 在往线程池中投入任务之前将需要的数据拷贝副本到参数中，保证不发生读写竞争，
    // 由于拷贝参数时尚运行在主线程中，故在参数拷贝过程中是线程安全的。
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (const BError &e) {
            HILOGE("%{public}s", e.what());
        } catch (const exception &e) {
            HILOGE("%{public}s", e.what());
        } catch (...) {
            HILOGE("");
        }
    });
}

bool IsAllFileReceived(vector<string> tars)
{
    // 是否已收到索引文件
    auto it = find(tars.begin(), tars.end(), string(BConstants::EXT_BACKUP_MANAGE));
    if (tars.end() == it) {
        return false;
    }

    // 获取索引文件内容
    string path = string(BConstants::PATH_BUNDLE_BACKUP_HOME).append(BConstants::SA_BUNDLE_BACKUP_RESTORE);
    string indexFile = path + string(BConstants::EXT_BACKUP_MANAGE);
    BJsonCachedEntity<BJsonEntityExtManage> cachedEntity(UniqueFd(open(indexFile.data(), O_RDONLY)));
    auto cache = cachedEntity.Structuralize();
    set<string> info = cache.GetExtManage();

    // 从数量上判断是否已经全部收到
    if (tars.size() <= info.size()) {
        return false;
    }

    // 逐个判断是否收到
    sort(tars.begin(), tars.end());
    if (includes(tars.begin(), tars.end(), info.begin(), info.end())) {
        return true;
    }
    return false;
}

void BackupExtExtension::AsyncTaskRestore(const string fileName)
{
    auto task = [obj {wptr<BackupExtExtension>(this)}, tars {tars_}, fileName]() {
        auto ptr = obj.promote();
        if (!ptr) {
            HILOGI("ext_extension handle have been already released");
            return;
        }

        if (fileName != string(BConstants::EXT_BACKUP_MANAGE)) {
            ptr->HandleRestore(fileName);
        }

        if (IsAllFileReceived(tars)) {
            // REM: 处理返回结果 ret
            auto proxy = ServiceProxy::GetInstance();
            if (proxy == nullptr) {
                HILOGE("Failed to obtain the ServiceProxy handle");
                return;
            } else {
                proxy->AppDone(ERR_OK);
            }
        }
    };

    // REM: 这里异步化了，需要做并发控制
    // 在往线程池中投入任务之前将需要的数据拷贝副本到参数中，保证不发生读写竞争，
    // 由于拷贝参数时尚运行在主线程中，故在参数拷贝过程中是线程安全的。
    threadPool_.AddTask([task]() {
        try {
            task();
        } catch (const BError &e) {
            HILOGE("%{public}s", e.what());
        } catch (const exception &e) {
            HILOGE("%{public}s", e.what());
        } catch (...) {
            HILOGE("");
        }
    });
}
} // namespace OHOS::FileManagement::Backup