/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_DIRECT_EX_MOCK_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_DIRECT_EX_MOCK_H

#include <gmock/gmock.h>

#include <cstdio>

#include "directory_ex.h"

namespace OHOS {
namespace AppFileService {
class DirectoryFunc {
public:
    virtual ~DirectoryFunc() = default;
    virtual bool ForceRemoveDirectoryBMS(const std::string&) = 0;
public:
    static inline std::shared_ptr<DirectoryFunc> directoryFunc_ = nullptr;
};

class DirectoryFuncMock : public DirectoryFunc {
public:
    MOCK_METHOD(bool, ForceRemoveDirectoryBMS, (const std::string&));
};
} // namespace AppFileService
} // namespace OHOS

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_LIBRARY_FUNC_MOCK_H