/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_filesystem/b_file.h"

#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "b_error/b_error.h"
#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
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
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS