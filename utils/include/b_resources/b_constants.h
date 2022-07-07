/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#ifndef OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H
#define OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H

#include <array>
#include <string_view>
#include <unistd.h>

namespace OHOS::FileManagement::Backup::BConstants {
static inline const char *EXTENSION_ACTION_PARA = "extensionAction";
enum class ExtensionAction {
    INVALID = 0,
    BACKUP = 1,
    RESTORE = 2,
};

constexpr int SPAN_USERID_UID = 20000000;
constexpr int SYSTEM_UID = 1000;

// 应用备份数据暂存路径。
static inline std::string_view SA_BUNDLE_BACKUP_DIR = "/data/service/el2/100/backup/bundles/";
static inline std::string_view SA_BUNDLE_BACKUP_BAKCUP = "/backup/";
static inline std::string_view SA_BUNDLE_BACKUP_RESTORE = "/restore/";
static inline std::string_view SA_BUNDLE_BACKUP_ROOT_DIR = "/data/service/el2/100/backup/backup_sa/";
static inline std::string_view SA_BUNDLE_BACKUP_TMP_DIR = "/tmp/";
static inline std::string_view BACKUP_TOOL_RECEIVE_DIR = "/data/backup/received/";
static inline std::string_view PATH_BUNDLE_BACKUP_HOME = "/data/storage/el2/backup";

// 应用默认备份的目录，其均为相对根路径的路径。为避免模糊匹配，务必以斜线为结尾。
static inline std::array<std::string_view, 6> PATHES_TO_BACKUP = {
    "data/storage/el2/database/",
    "data/storage/el2/base/files/",
    "data/storage/el2/base/preferences/",
    "data/storage/el2/base/haps/*/database/",
    "data/storage/el2/base/haps/*/base/files/",
    "data/storage/el2/base/haps/*/base/preferences/",
};
} // namespace OHOS::FileManagement::Backup::BConstants

#endif // OHOS_FILEMGMT_BACKUP_B_CONSTANTS_H