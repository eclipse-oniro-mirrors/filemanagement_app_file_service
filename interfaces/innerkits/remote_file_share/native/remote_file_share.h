/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef REMOTE_FILE_SHARE_H
#define REMOTE_FILE_SHARE_H

#include <string>

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
namespace {
    const std::string SHARE_ALL_DEVICE = "0";
}
class RemoteFileShare {
public:
    RemoteFileShare() {}
    static int CreateSharePath(const int &fd, std::string &sharePath,
                        const int &userId, const std::string &deviceId = SHARE_ALL_DEVICE);
    ~RemoteFileShare() {}
};
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS

#endif