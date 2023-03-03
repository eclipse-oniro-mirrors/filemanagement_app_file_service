/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H

#include <string>
#include <string_view>
#include <vector>

#include "nocopyable.h"

namespace OHOS::FileManagement::Backup {
class BTarballCmdline final : protected NoCopyable {
public:
    void Tar(std::string_view root, std::vector<std::string_view> includes, std::vector<std::string_view> excludes);
    void Untar(std::string_view root);

public:
    BTarballCmdline(std::string_view tarballDir, std::string_view tarballName);

private:
    std::string tarballDir_;
    std::string tarballName_;
    std::string tarballPath_;
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_CMDLINE_H
