/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "file_uri.h"

#include <unistd.h>

#include "uri.h"

#include "common_func.h"
#include "log.h"

using namespace std;
namespace OHOS {
namespace AppFileService {
namespace ModuleFileUri {
const std::string PATH_SHARE = "/data/storage/el2/share";
const std::string MODE_RW = "/rw/";
const std::string MODE_R = "/r/";
const std::string FILE_SCHEME_PREFIX = "file://";
string FileUri::GetName()
{
    string sandboxPath = uri_.GetPath();
    size_t posLast = sandboxPath.find_last_of("/");
    if (posLast == string::npos) {
        return sandboxPath;
    }

    if (posLast == sandboxPath.size()) {
        return "";
    }

    return sandboxPath.substr(posLast + 1);
}

string FileUri::GetPath()
{
    string sandboxPath = uri_.GetPath();
    string realPath = sandboxPath;
    string providerBundleName = uri_.GetAuthority();
    string targetBundleName = CommonFunc::GetSelfBundleName();
    if (CommonFunc::CheckPublicDirPath(realPath) ||
       ((targetBundleName != providerBundleName) && (providerBundleName != ""))) {
        realPath = PATH_SHARE + MODE_RW + providerBundleName + sandboxPath;
        if (access(realPath.c_str(), F_OK) != 0) {
            realPath = PATH_SHARE + MODE_R + providerBundleName + sandboxPath;
        }
    }

    return realPath;
}

string FileUri::ToString()
{
    return uri_.ToString();
}

FileUri::FileUri(const string &uriOrPath): uri_(
    (uriOrPath.find(FILE_SCHEME_PREFIX) == 0) ? uriOrPath : CommonFunc::GetUriFromPath(uriOrPath)
)
{}
}
}  // namespace AppFileService
}  // namespace OHOS