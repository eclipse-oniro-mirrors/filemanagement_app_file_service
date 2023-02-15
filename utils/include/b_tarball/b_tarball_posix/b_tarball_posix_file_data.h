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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H

#include <sys/stat.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixFileData {
public:
    void Publish(const UniqueFd &inFile, const UniqueFd &outFile);

public:
    BTarballPosixFileData() = default;
    explicit BTarballPosixFileData(struct stat &statInfo);
    ~BTarballPosixFileData() = default;

private:
    struct stat stat_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_FILE_DATA_H