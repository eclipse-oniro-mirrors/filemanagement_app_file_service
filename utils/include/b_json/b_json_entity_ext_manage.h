/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H

#include <set>
#include <string>

#include "b_json/b_json_cached_entity.h"
#include "json/json.h"

namespace OHOS::FileManagement::Backup {
class BJsonEntityExtManage {
public:
    /**
     * @brief 构造方法，具备T(Json::Value&)能力的构造函数
     *
     * @param Json对象引用
     */
    BJsonEntityExtManage(Json::Value &obj);

    /**
     * @brief 设置索引文件
     *
     * @param info std::set<filename>
     */
    void SetExtManage(const std::set<std::string> &info) const;

    /**
     * @brief 获取索引文件
     *
     * @return std::set<std::string>
     */
    std::set<std::string> GetExtManage() const;

private:
    Json::Value &obj_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H