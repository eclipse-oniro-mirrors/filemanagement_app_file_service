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

#ifndef OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_HEADER_H
#define OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_HEADER_H

#include "b_tarball_posix_extended_data.h"
#include "b_tarball_posix_header.h"
#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
class BTarballPosixExtendedHeader {
public:
    void Publish(const UniqueFd &outFile);

public:
    BTarballPosixExtendedHeader() = default;
    BTarballPosixExtendedHeader(BTarballPosixExtendedData &extData);
    ~BTarballPosixExtendedHeader() = default;

private:
    BTarballPosixHeader header_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_TARBALL_POSIX_EXTENDED_HEADER_H