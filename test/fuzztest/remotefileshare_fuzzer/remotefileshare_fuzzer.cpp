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

#include "remotefileshare_fuzzer.h"

#include <cstdint>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <climits>

#include "securec.h"

namespace OHOS {
namespace AppFileService {
namespace ModuleRemoteFileShare {
namespace {
    constexpr int HMDFS_CID_SIZE = 64;
    constexpr unsigned HMDFS_IOC = 0xf2;
    const char* SHARE_PATH = "/data/storage/el2/distributedfiles/.share";
}

#define HMDFS_IOC_SET_SHARE_PATH    _IOW(HMDFS_IOC, 1, struct hmdfs_share_control)

struct hmdfs_share_control {
    int src_fd;
    char cid[HMDFS_CID_SIZE];
};

bool ShareFilePathIoctlFdAndCidFuzzTest(const uint8_t* data, size_t size)
{
    struct hmdfs_share_control sc;
    int32_t ret = 0;
    int32_t dirFd;

    if (size == 0) {
        return false;
    }

    if (access(SHARE_PATH, F_OK) != 0) {
        ret = mkdir(SHARE_PATH, S_IRWXU | S_IRWXG | S_IXOTH);
        if (ret < 0) {
            return false;
        }
    }

    char realPath[PATH_MAX] = {0};
    if (!realpath(SHARE_PATH, realPath)) {
        return false;
    }
    dirFd = open(realPath, O_RDONLY);
    if (dirFd < 0) {
        return false;
    }

    const char* cid = reinterpret_cast<const char*>(data);
    sc.src_fd = size;
    if (memcpy_s(sc.cid, HMDFS_CID_SIZE, cid, size) != 0) {
        close(dirFd);
        return false;
    }

    ret = ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &sc);
    if (ret < 0) {
        close(dirFd);
        return false;
    }

    return true;
}

bool ShareFilePathIoctlCidFuzzTest(const uint8_t* data, size_t size)
{
    struct hmdfs_share_control sc;
    int32_t ret = 0;
    int32_t dirFd;
    int32_t srcFd;

    if (size == 0) {
        return false;
    }

    if (access(SHARE_PATH, F_OK) != 0) {
        ret = mkdir(SHARE_PATH, S_IRWXU | S_IRWXG | S_IXOTH);
        if (ret < 0) {
            return false;
        }
    }

    char realPath[PATH_MAX] = {0};
    if (!realpath(SHARE_PATH, realPath)) {
        return false;
    }
    dirFd = open(realPath, O_RDONLY);
    if (dirFd < 0) {
        return false;
    }

    const char* srcPath = "/data/service/el2/100/hmdfs/non_account/data/com.ohos.camera";
    srcFd = open(srcPath, O_RDONLY);
    if (srcFd < 0) {
        close(dirFd);
        return false;
    }
    sc.src_fd = size;
    const char* cid = reinterpret_cast<const char*>(data);
    if (memcpy_s(sc.cid, HMDFS_CID_SIZE, cid, size) != 0) {
        close(dirFd);
        close(srcFd);
        return false;
    }

    ret = ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &sc);
    if (ret < 0) {
        close(dirFd);
        close(srcFd);
        return false;
    }

    close(srcFd);
    return true;
}

bool ShareFilePathIoctlFdFuzzTest(const uint8_t* data, size_t size)
{
    struct hmdfs_share_control sc;
    int32_t ret = 0;
    int32_t dirFd;

    if (size == 0) {
        return false;
    }

    if (access(SHARE_PATH, F_OK) != 0) {
        ret = mkdir(SHARE_PATH, S_IRWXU | S_IRWXG | S_IXOTH);
        if (ret < 0) {
            return false;
        }
    }

    char realPath[PATH_MAX] = {0};
    if (!realpath(SHARE_PATH, realPath)) {
        return false;
    }

    dirFd = open(realPath, O_RDONLY);
    if (dirFd < 0) {
        return false;
    }

    const char* cid = "remoteShareFileFuzzTestCidxxx";
    if (memcpy_s(sc.cid, HMDFS_CID_SIZE, cid, strlen(cid)) != 0) {
        close(dirFd);
        return false;
    }
    sc.src_fd = size;

    ret = ioctl(dirFd, HMDFS_IOC_SET_SHARE_PATH, &sc);
    if (ret < 0) {
        close(dirFd);
        return false;
    }

    return true;
}
} // namespace ModuleRemoteFileShare
} // namespace AppFileService
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AppFileService::ModuleRemoteFileShare::ShareFilePathIoctlFdFuzzTest(data, size);
    OHOS::AppFileService::ModuleRemoteFileShare::ShareFilePathIoctlCidFuzzTest(data, size);
    OHOS::AppFileService::ModuleRemoteFileShare::ShareFilePathIoctlFdAndCidFuzzTest(data, size);
    return 0;
}
