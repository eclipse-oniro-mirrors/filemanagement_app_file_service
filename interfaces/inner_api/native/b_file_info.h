/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
#define OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H

#include <memory>
#include <string>

#include "parcel.h"

namespace OHOS::FileManagement::Backup {
using AppId = std::string;
using TmpFileSN = uint32_t;

struct BFileInfo : public Parcelable {
    AppId owner;
    std::string fileName;
    TmpFileSN sn; // 用于服务零拷贝接收文件场景

    BFileInfo() = default;
    BFileInfo(std::string appId, std::string strFileNanme, TmpFileSN id)
        : owner(appId), fileName(strFileNanme), sn(id) {}
    ~BFileInfo() = default;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static BFileInfo *Unmarshalling(Parcel &parcel);
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_FILE_INFO_H
