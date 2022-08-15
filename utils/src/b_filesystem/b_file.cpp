/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_filesystem/b_file.h"

#include <cstring>
#include <fcntl.h>
#include <libgen.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<char[]> BFile::ReadFile(const UniqueFd &fd)
{
    if (lseek(fd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }

    struct stat stat = {};
    if (fstat(fd, &stat) == -1) {
        throw BError(errno);
    }
    off_t fileSize = stat.st_size;
    if (fileSize == 0) {
        HILOGI("Deserialized an empty file");
        return std::make_unique<char[]>(1);
    }

    auto buf = std::make_unique<char[]>(fileSize + 1);
    if (read(fd, buf.get(), fileSize) == -1) {
        throw BError(errno);
    }
    return buf;
}

void BFile::SendFile(int out_fd, int in_fd)
{
    long offset = 0;
    long ret = 0;
    if (lseek(out_fd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }
    if (lseek(in_fd, 0, SEEK_SET) == -1) {
        throw BError(errno);
    }
    struct stat stat = {};
    if (fstat(in_fd, &stat) == -1) {
        throw BError(errno);
    }
    while ((ret = sendfile(out_fd, in_fd, &offset, stat.st_size)) > 0)
        ;
    if (ret == -1) {
        throw BError(errno);
    }
}

void BFile::Write(const UniqueFd &fd, const string &str)
{
    int ret = pwrite(fd, str.c_str(), str.length(), 0);
    if (ret == -1) {
        throw BError(errno);
    }
    if (ftruncate(fd, ret) == -1) {
        throw BError(errno);
    }
}

bool BFile::CopyFile(const string &from, const string &to)
{
    if (from == to) {
        return true;
    }

    try {
        auto resolvedPath = make_unique<char[]>(PATH_MAX);
        if (!realpath(from.data(), resolvedPath.get())) {
            HILOGI("failed to real path the file %{public}s", from.c_str());
            return false;
        }
        string oldPath(resolvedPath.get());
        UniqueFd fdFrom(open(oldPath.data(), O_RDONLY));
        if (fdFrom == -1) {
            HILOGI("failed to open the file %{public}s", from.c_str());
            throw BError(errno);
        }

        string dir = to;
        if (mkdir(dirname(dir.data()), S_IRWXU) && errno != EEXIST) {
            HILOGI("failed to access or make directory %{public}s", dir.c_str());
            throw BError(errno);
        }
        UniqueFd fdTo(open(to.data(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR));
        if (fdTo == -1) {
            HILOGI("failed to open the file %{public}s", to.c_str());
            throw BError(errno);
        }

        SendFile(fdTo, fdFrom);
        return true;
    } catch (const BError &e) {
        HILOGE("%{public}s", e.what());
    } catch (const exception &e) {
        HILOGE("%{public}s", e.what());
    } catch (...) {
        HILOGE("");
    }
    return false;
}
} // namespace OHOS::FileManagement::Backup