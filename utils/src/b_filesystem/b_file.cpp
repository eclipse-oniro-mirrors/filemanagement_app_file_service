/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_filesystem/b_file.h"

#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

unique_ptr<char[]> BFile::ReadFile(const UniqueFd &fd)
{
    if (lseek(fd, 0, SEEK_SET) == -1) {
        HILOGE("Failed to seek to the file front because of %{public}s", strerror(errno));
        throw std::system_error(errno, std::generic_category());
    }

    struct stat stat = {};
    if (fstat(fd, &stat) == -1) {
        HILOGE("Failed to get the file status because of %{public}s", strerror(errno));
        throw std::system_error(errno, std::generic_category());
    }
    off_t fileSize = stat.st_size;
    if (fileSize == 0) {
        HILOGE("Try to deserialize an empty file");
        return std::make_unique<char[]>(1);
    }

    auto buf = std::make_unique<char[]>(fileSize + 1);
    if (read(fd, buf.get(), fileSize) == -1) {
        HILOGE("Failed to read from the JsonFile because of %{public}s", strerror(errno));
        throw std::system_error(errno, std::generic_category());
    }
    return buf;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS