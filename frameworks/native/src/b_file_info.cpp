/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_file_info.h"
#include "filemgmt_libhilog.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

bool BFileInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(owner) || !parcel.WriteString(fileName) || !parcel.WriteUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

bool BFileInfo::ReadFromParcel(Parcel &parcel)
{
    if (!parcel.ReadString(owner) || !parcel.ReadString(fileName) || !parcel.ReadUint32(sn)) {
        HILOGE("Failed");
        return false;
    }
    return true;
}

BFileInfo *BFileInfo::Unmarshalling(Parcel &parcel)
{
    try {
        auto result = make_unique<BFileInfo>();
        if (!result->ReadFromParcel(parcel)) {
            return nullptr;
        }
        return result.release();
    } catch (const bad_alloc e) {
        HILOGE("Failed to unmarshall BFileInfo because of %{public}s", e.what());
    }
    return nullptr;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS