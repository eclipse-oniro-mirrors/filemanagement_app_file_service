/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef OHOS_FILEMGMT_BACKUP_DFX_INFO_H
#define OHOS_FILEMGMT_BACKUP_DFX_INFO_H
 
#include <cstdint>
#include <string>
 
#include "parcel.h"
 
namespace OHOS::FileManagement::Backup {

struct DfxInfo : public OHOS::Parcelable {
    int64_t uniqId {0};
    uint32_t extConnectSpend {0};
    std::string bundleName;
    int32_t broadcastInfo {-1}; // -1=无广播配置, 0=发送失败, 1=发送成功
    int64_t lastIncrementalTime {0}; // 增量备份上次时间
 
    bool Marshalling(OHOS::Parcel &parcel) const override
    {
        return parcel.WriteInt64(uniqId)
            && parcel.WriteUint32(extConnectSpend)
            && parcel.WriteString(bundleName)
            && parcel.WriteInt32(broadcastInfo)
            && parcel.WriteInt64(lastIncrementalTime);
    }
 
    static DfxInfo* Unmarshalling(OHOS::Parcel &parcel)
    {
        auto info = new (std::nothrow) DfxInfo();
        if (info == nullptr) {
            return nullptr;
        }
        if (!parcel.ReadInt64(info->uniqId)
            || !parcel.ReadUint32(info->extConnectSpend)
            || !parcel.ReadString(info->bundleName)
            || !parcel.ReadInt32(info->broadcastInfo)
            || !parcel.ReadInt64(info->lastIncrementalTime)) {
            delete info;
            return nullptr;
        }
        return info;
    }
};
} // namespace OHOS::FileManagement::Backup
#endif // OHOS_FILEMGMT_BACKUP_DFX_INFO_H