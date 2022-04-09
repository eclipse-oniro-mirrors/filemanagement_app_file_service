/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "complex_object.h"

#include <memory>

namespace OHOS {
namespace FileManagement {
namespace Backup {
using namespace std;

bool ComplexObject::Marshalling(Parcel &parcel) const
{
    parcel.WriteInt32(field1_);
    parcel.WriteInt32(field2_);
    return true;
}

unique_ptr<ComplexObject> ComplexObject::Unmarshalling(Parcel &parcel)
{
    auto obj = make_unique<ComplexObject>();
    obj->field2_ = parcel.ReadInt32();
    obj->field1_ = parcel.ReadInt32();
    return obj;
}
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS