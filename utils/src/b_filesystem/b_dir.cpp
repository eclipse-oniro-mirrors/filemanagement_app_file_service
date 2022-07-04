/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_filesystem/b_dir.h"

#include <dirent.h>
#include <functional>

#include "b_error/b_error.h"
#include "directory_ex.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

tuple<ErrCode, vector<string>> BDir::GetDirFiles(const string &path)
{
    vector<string> files;
    unique_ptr<DIR, function<void(DIR *)>> dir = {opendir(path.c_str()), closedir};
    if (!dir) {
        HILOGE("Invalid directory path: %{private}s", path.c_str());
        return {BError(errno).GetCode(), files};
    }

    struct dirent *ptr = nullptr;
    while (!(ptr = readdir(dir.get()))) {
        // current dir OR parent dir
        if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0)) {
            continue;
        } else if (ptr->d_type == DT_DIR) {
            continue;
        } else {
            files.push_back(IncludeTrailingPathDelimiter(path) + string(ptr->d_name));
        }
    }

    return {BError(BError::Codes::OK).GetCode(), files};
}
} // namespace OHOS::FileManagement::Backup