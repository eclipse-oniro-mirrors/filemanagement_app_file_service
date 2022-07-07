/*
 * 版权所有 (c) 华为技术有限公司 2022
 */
#include <cerrno>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <mutex>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <vector>

#include "b_error/b_error.h"
#include "b_filesystem/b_file.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "directory_ex.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class RstoreSession {
public:
    void UpdateBundleCountAndTryNotifty()
    {
        lock_guard<mutex> lk(lock_);
        cnt_--;
        if (cnt_ == 0) {
            ready_ = true;
            cv_.notify_all();
        }
    }
    void SetTaskNumber(uint32_t count)
    {
        cnt_ = count;
    }
    void Wait()
    {
        unique_lock<mutex> lk(lock_);
        cv_.wait(lk, [&] { return ready_; });
    }

    unique_ptr<BSessionRestore> session_ = {};
private:
    uint32_t cnt_ = -1;
    mutable condition_variable cv_;
    mutex lock_;
    bool ready_ = false;
};

static string GenHelpMsg()
{
    return "the functionality of the restore api. Arg list:\n"
           "path_cap_file bundleName1 bundleName2...";
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
}

static void OnBundleFinished(shared_ptr<RstoreSession> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    ctx->UpdateBundleCountAndTryNotifty();
}

static void OnAllBundlesFinished(ErrCode err)
{
    if (err == 0) {
        printf("Restore successful\n");
    } else {
        printf("Failed to Unplanned Abort error: %d\n", err);
    }
}

static void OnBackupServiceDied()
{
    printf("backupServiceDied\n");
}

static void RestoreApp(shared_ptr<RstoreSession> restore, vector<BundleName> &bundleNames)
{
    if (!restore || !restore->session_) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    for (auto &bundleName : bundleNames) {
        string path = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + bundleName;
        if (access(path.data(), F_OK) != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
        }
        vector<string> filePaths;
        GetDirFiles(path, filePaths);
        if (filePaths.size() == 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "error path or empty dir");
        }
        for (auto &filePath : filePaths) {
            const auto [errCode, tmpFileSN, RemoteFd] = restore->session_->GetFileOnServiceEnd();
            if (errCode != 0 || RemoteFd < 0) {
                throw BError(BError::Codes::TOOL_INVAL_ARG, "GetFileOnServiceEnd error");
            }
            printf("errCode = %d tmpFileSN = %d RemoteFd = %d\n", errCode, tmpFileSN, RemoteFd.Get());
            UniqueFd fdLocal(open(filePath.data(), O_RDWR));
            if (fdLocal < 0) {
                throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
            }
            struct stat stat = {};
            if (fstat(fdLocal, &stat) == -1) {
                throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
            }
            if (sendfile(RemoteFd, fdLocal, nullptr, stat.st_size) == -1) {
                throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
            }
            string fileName = filePath.substr(filePath.rfind("/") + 1);
            int ret = restore->session_->PublishFile(BFileInfo(bundleName, fileName, tmpFileSN));
            if (ret != 0) {
                throw BError(BError::Codes::TOOL_INVAL_ARG, "PublishFile error");
            }
        }
    }
}

static int32_t Init(string_view pathCapFile, ToolsOp::CRefVStrView args)
{
    std::vector<BundleName> bundleNames;
    for (auto &&bundleName : args) {
        bundleNames.emplace_back(bundleName.data());
    }
    auto ctx = make_shared<RstoreSession>();
    ctx->session_ = BSessionRestore::Init(
        bundleNames, BSessionRestore::Callbacks {
                        .onBundleStarted = OnBundleStarted,
                        .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
                        .onAllBundlesFinished = OnAllBundlesFinished,
                        .onBackupServiceDied = OnBackupServiceDied,
                     });
    if (ctx->session_ == nullptr) {
        printf("Failed to init restore");
        return -EPERM;
    }
    ctx->SetTaskNumber(bundleNames.size());
    UniqueFd fdRemote(ctx->session_->GetLocalCapabilities());
    if (fdRemote < 0) {
        printf("Failed to receive fd");
        return fdRemote;
    }
    if (lseek(fdRemote, 0, SEEK_SET) == -1) {
        fprintf(stderr, "Failed to lseek. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    struct stat stat = {};
    if (fstat(fdRemote, &stat) == -1) {
        fprintf(stderr, "Failed to fstat. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    UniqueFd fdLocal(open(pathCapFile.data(), O_WRONLY | O_CREAT, S_IRWXU));
    if (fdLocal < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    if (sendfile(fdLocal, fdRemote, nullptr, stat.st_size) == -1) {
        fprintf(stderr, "Failed to Copy file. error: %d %s\n", errno, strerror(errno));
        return -errno;
    }
    RestoreApp(ctx, bundleNames);
    int ret = ctx->session_->Start();
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "restore start error");
    }
    ctx->Wait();
    return 0;
}

static int Exec(ToolsOp::CRefVStrView args)
{
    if (args.empty()) {
        fprintf(stderr, "Please input the name of API to restore\n");
        return -EINVAL;
    }
    std::vector<string_view> argsWithoutHead(args.begin() + 1, args.end());
    return Init(args.front(), argsWithoutHead);
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"restore"},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup