/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H
#define OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H

#include <map>
#include <set>
#include <string>
#include <sys/stat.h>

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
     * @param info std::map<string, pair<string, struct stat>>
     */
    void SetExtManage(const map<string, pair<string, struct stat>> &info) const;

    /**
     * @brief 获取索引文件
     *
     * @return std::set<std::string>
     */
    std::set<std::string> GetExtManage() const;

    /**
     * @brief 获取索引文件及详细信息
     *
     * @return map<string, pair<string, struct stat>>
     */
    std::map<std::string, std::pair<std::string, struct stat>> GetExtManageInfo() const;

    /**
     * @brief Set the hard link Information
     *
     * @param origin 原始文件名
     * @param hardLinks 硬链接文件名
     * @return true 设置成功
     * @return false 设置失败
     */
    bool SetHardLinkInfo(const std::string origin, const std::set<std::string> hardLinks);

    /**
     * @brief Get the hard link Information
     *
     * @param origin 原始文件名
     * @return const 硬链接集合
     */
    const std::set<std::string> GetHardLinkInfo(const string origin);

private:
    Json::Value &obj_;
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_B_JSON_ENTITY_EXT_MANAGE_H