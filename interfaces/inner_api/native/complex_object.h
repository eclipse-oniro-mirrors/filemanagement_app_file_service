/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_COMPLEX_OBJECT_H
#define OHOS_FILEMGMT_BACKUP_COMPLEX_OBJECT_H

#include <memory>

#include "parcel.h"

namespace OHOS {
namespace FileManagement {
namespace Backup {
class ComplexObject final : public Parcelable {
public:
    ComplexObject() {}
    ComplexObject(int field1, int field2) : field1_(field1), field2_(field2) {}
    ~ComplexObject() {}

    int32_t field1_{0};
    int32_t field2_{0};

    bool Marshalling(Parcel &parcel) const override;
    static std::unique_ptr<ComplexObject> Unmarshalling(Parcel &parcel);
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_COMPLEX_OBJECT_H