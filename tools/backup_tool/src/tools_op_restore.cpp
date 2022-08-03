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
#include <regex>
#include <set>
#include <string>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <tuple>
#include <unistd.h>
#include <vector>

#include "b_error/b_error.h"
#include "b_filesystem/b_dir.h"
#include "b_filesystem/b_file.h"
#include "b_json/b_json_entity_ext_manage.h"
#include "b_resources/b_constants.h"
#include "backup_kit_inner.h"
#include "base/hiviewdfx/hitrace/interfaces/native/innerkits/include/hitrace_meter/hitrace_meter.h"
#include "errors.h"
#include "service_proxy.h"
#include "tools_op.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

class Session {
public:
    void UpdateBundleSendFiles(const BundleName &bundleName, const std::string &fileName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].sendFile.insert(fileName);
    }

    void UpdateBundleSentFiles(const BundleName &bundleName, const std::string &fileName)
    {
        lock_guard<mutex> lk(lock_);
        bundleStatusMap_[bundleName].sentFile.insert(fileName);
        TryClearBundleOfMap(bundleName);
    }

    void TryNotify(bool flag = false)
    {
        if (flag == true) {
            ready_ = true;
            cv_.notify_all();
        } else if (bundleStatusMap_.size() == 0 && cnt_ == 0) {
            ready_ = true;
            cv_.notify_all();
        }
    }

    void UpdateBundleFinishedCount()
    {
        lock_guard<mutex> lk(lock_);
        cnt_--;
    }

    void SetBundleFinishedCount(uint32_t cnt)
    {
        cnt_ = cnt;
    }

    void Wait()
    {
        unique_lock<mutex> lk(lock_);
        cv_.wait(lk, [&] { return ready_; });
    }

    unique_ptr<BSessionRestore> session_ = {};

private:
    struct BundleStatus {
        set<string> sendFile;
        set<string> sentFile;
    };

    void TryClearBundleOfMap(const BundleName &bundleName)
    {
        if (bundleStatusMap_[bundleName].sendFile == bundleStatusMap_[bundleName].sentFile) {
            bundleStatusMap_.erase(bundleName);
        }
    }

    map<string, BundleStatus> bundleStatusMap_;
    mutable condition_variable cv_;
    mutex lock_;
    bool ready_ = false;
    uint32_t cnt_;
};

static string GenHelpMsg()
{
    return "\t\tThis operation helps to restore application data.\n"
           "\t\t--pathCapFile\t\t This parameter should be the path of the capability file.\n"
           "\t\t--bundle\t\t This parameter is bundleName.";
}

static void OnFileReady(shared_ptr<Session> ctx, const BFileInfo &fileInfo, UniqueFd fd)
{
    printf("FileReady owner = %s, fileName = %s, sn = %u, fd = %d\n", fileInfo.owner.c_str(), fileInfo.fileName.c_str(),
           fileInfo.sn, fd.Get());
    if (!regex_match(fileInfo.fileName, regex("^[0-9a-zA-Z_.]+$"))) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "Filename is not alphanumeric");
    }
    string tmpPath = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + fileInfo.owner + "/" + fileInfo.fileName;
    if (access(tmpPath.data(), F_OK) != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    UniqueFd fdLocal(open(tmpPath.data(), O_RDONLY));
    if (fdLocal < 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    BFile::SendFile(fd, fdLocal);
    int ret = ctx->session_->PublishFile(fileInfo);
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "PublishFile error");
    }
    ctx->UpdateBundleSentFiles(fileInfo.owner, fileInfo.fileName);
    ctx->TryNotify();
}

static void OnBundleStarted(ErrCode err, const BundleName name)
{
    printf("BundleStarted errCode = %d, BundleName = %s\n", err, name.c_str());
}

static void OnBundleFinished(shared_ptr<Session> ctx, ErrCode err, const BundleName name)
{
    printf("BundleFinished errCode = %d, BundleName = %s\n", err, name.c_str());
    ctx->UpdateBundleFinishedCount();
    ctx->TryNotify();
}

static void OnAllBundlesFinished(ErrCode err)
{
    if (err == 0) {
        printf("Restore successful\n");
    } else {
        printf("Failed to Unplanned Abort error: %d\n", err);
    }
}

static void OnBackupServiceDied(shared_ptr<Session> ctx)
{
    printf("backupServiceDied\n");
    ctx->TryNotify(true);
}

static void RestoreApp(shared_ptr<Session> restore, vector<BundleName> &bundleNames)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "RestoreApp");
    if (!restore || !restore->session_) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
    }
    for (auto &bundleName : bundleNames) {
        if (!regex_match(bundleName, regex("^[0-9a-zA-Z_.]+$"))) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "bundleName is not alphanumeric");
        }
        string path = string(BConstants::BACKUP_TOOL_RECEIVE_DIR) + bundleName;
        if (access(path.data(), F_OK) != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, std::generic_category().message(errno));
        }
        const auto [err, filePaths] = BDir::GetDirFiles(path);
        if (err != 0) {
            throw BError(BError::Codes::TOOL_INVAL_ARG, "error path");
        }
        for (auto &filePath : filePaths) {
            string fileName = filePath.substr(filePath.rfind("/") + 1);
            restore->session_->GetExtFileName(bundleName, fileName);
            restore->UpdateBundleSendFiles(bundleName, fileName);
        }
    }
    FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
}

static int32_t Init(string pathCapFile, std::vector<string> bundles)
{
    StartTrace(HITRACE_TAG_FILEMANAGEMENT, "Init");
    std::vector<BundleName> bundleNames;
    for (auto &&bundleName : bundles) {
        bundleNames.emplace_back(bundleName.data());
    }
    auto ctx = make_shared<Session>();
    ctx->session_ = BSessionRestore::Init(
        bundleNames, BSessionRestore::Callbacks {
                        .onFileReady = bind(OnFileReady, ctx, placeholders::_1, placeholders::_2),
                        .onBundleStarted = OnBundleStarted,
                        .onBundleFinished = bind(OnBundleFinished, ctx, placeholders::_1, placeholders::_2),
                        .onAllBundlesFinished = OnAllBundlesFinished,
                        .onBackupServiceDied = bind(OnBackupServiceDied, ctx)
                    });
    if (ctx->session_ == nullptr) {
        printf("Failed to init restore");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -EPERM;
    }
    UniqueFd fdRemote(ctx->session_->GetLocalCapabilities());
    if (fdRemote < 0) {
        printf("Failed to receive fd");
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return fdRemote;
    }
    if (lseek(fdRemote, 0, SEEK_SET) == -1) {
        fprintf(stderr, "Failed to lseek. error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    struct stat stat = {};
    if (fstat(fdRemote, &stat) == -1) {
        fprintf(stderr, "Failed to fstat. error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    UniqueFd fdLocal(open(pathCapFile.data(), O_WRONLY | O_CREAT, S_IRWXU));
    if (fdLocal < 0) {
        fprintf(stderr, "Failed to open file. error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    if (sendfile(fdLocal, fdRemote, nullptr, stat.st_size) == -1) {
        fprintf(stderr, "Failed to Copy file. error: %d %s\n", errno, strerror(errno));
        FinishTrace(HITRACE_TAG_FILEMANAGEMENT);
        return -errno;
    }
    ctx->SetBundleFinishedCount(bundleNames.size());
    RestoreApp(ctx, bundleNames);
    int ret = ctx->session_->Start();
    if (ret != 0) {
        throw BError(BError::Codes::TOOL_INVAL_ARG, "restore start error");
    }
    ctx->Wait();
    return 0;
}

static int Exec(map<string, vector<string>> mapArgToVal)
{
    if (mapArgToVal.find("pathCapFile") == mapArgToVal.end() || mapArgToVal.find("bundles") == mapArgToVal.end()) {
        return -EPERM;
    }
    return Init(*(mapArgToVal["pathCapFile"].begin()), mapArgToVal["bundles"]);
}

/**
 * @brief The hack behind is that "variable with static storage duration has initialization or a destructor with side
 * effects; it shall not be eliminated even if it appears to be unused" -- point 2.[basic.stc.static].c++ draft
 *
 */
static bool g_autoRegHack = ToolsOp::Register(ToolsOp::Descriptor {
    .opName = {"restore"},
    .argList = {{
                    .paramName = "pathCapFile",
                    .repeatable = false,
                },
                {
                    .paramName = "bundles",
                    .repeatable = true,
                }},
    .funcGenHelpMsg = GenHelpMsg,
    .funcExec = Exec,
});
} // namespace OHOS::FileManagement::Backup