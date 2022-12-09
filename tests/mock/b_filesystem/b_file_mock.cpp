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

#include "b_filesystem/b_file.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unique_fd.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

unique_ptr<char[]> BFile::ReadFile(const UniqueFd &fd)
{
    auto buf = make_unique<char[]>(1);
    return buf;
}

void BFile::SendFile(int outFd, int inFd) {}

void BFile::Write(const UniqueFd &fd, const string &str) {}

bool BFile::CopyFile(const string &from, const string &to)
{
    return true;
}
} // namespace OHOS::FileManagement::Backup