/**
 * 版权所有 (c) 华为技术有限公司 2022
 *
 * 本部件处理错误的原则：
 *     原则1：使用异常表示错误，但只有无法处理的问题才算得上是错误，否则只是普通的边界分支；
 *     原则2：仅在模块内部使用异常，而在界面层Catch所有异常，从而防止异常扩散；
 *     原则3：在注释里通过throw关键字注明可能抛出的异常，通报使用风险。
 */
#ifndef OHOS_FILEMGMT_BACKUP_B_ERROR_H
#define OHOS_FILEMGMT_BACKUP_B_ERROR_H

#include <map>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

#include "errors.h"

/**
 * @brief 黄区编译器升级CLANG-10后即支持
 *
 */
#if __has_builtin(__builtin_FILE) && __has_builtin(__builtin_LINE) && __has_builtin(__builtin_FUNCTION)
#define DEFINE_SOURCE_LOCATION                                              \
    int lineNo = __builtin_LINE(), const char *fileName = __builtin_FILE(), \
        const char *functionName = __builtin_FUNCTION()
#else
#define DEFINE_SOURCE_LOCATION int lineNo = -1, const char *fileName = "NA", const char *functionName = "NA"
#endif

namespace OHOS {
namespace FileManagement {
namespace Backup {
class BError : public std::exception {
public:
    /**
     * @brief 错误码，新增错误码时需要同步补充默认错误信息
     *
     */
    enum class Codes : ErrCode {
        // 0 无错误
        OK = 0x0,

        // 1~999 标准平台错误

        // 0x1000~0x1999 backup_utils错误
        UTILS_INVAL_JSON_ENTITY = 0x1000,
        UTILS_INVAL_FILE_HANDLE = 0x1001,
        UTILS_INVAL_TARBALL_ARG = 0x1002,
        UTILS_INVAL_PROCESS_ARG = 0x1003,

        // 0x2000~0x2999 backup_tool错误

        // 0x3000~0x3999 backup_sa错误
        SA_INVAL_ARG = 0x3000,
        SA_BROKEN_IPC = 0x3001,
        SA_REFUSED_ACT = 0x3002,

        // 0x4000~0x4999 backup_SDK错误
        SDK_INVAL_ARG = 0x4000,
        SDK_BROKEN_IPC = 0x4001,
        SDK_MIXED_SCENARIO = 0x4002,
    };

public:
    /**
     * @brief 返回OHOS标准错误码
     *
     * @return int 标注错误码
     */
    int GetCode() const
    {
        if (code_ == Codes::OK) {
            return 0;
        } else {
            return -1 * (static_cast<int>(code_) | ErrCodeOffset(SUBSYS_FILEMANAGEMENT, codeSubsystem_));
        }
    }

    /**
     * @brief 返回原始错误码
     *
     * @return Codes 原始错误码
     */
    Codes GetRawCode() const
    {
        return code_;
    }

    /**
     * @brief 返回错误信息
     *
     * @return const char* 错误信息
     */
    const char *what() const noexcept override
    {
        return msg_.c_str();
    }

public:
    /**
     * @brief 重载bool操作符，判断当前错误是否是错误
     *
     * @return true 是错误
     * @return false 不是错误
     */
    explicit operator bool() const
    {
        return code_ != Codes::OK;
    }

    /**
     * @brief 返回OHOS标准错误码
     *
     * @return int 标准错误码
     */
    operator int() const
    {
        return GetCode();
    }

public:
    /**
     * @brief 构造错误对象
     *
     * @param code 备份系统标准错误码，取自本类中的Codes
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    BError(Codes code = Codes::OK, DEFINE_SOURCE_LOCATION) : code_(code)
    {
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {mpErrToMsg_.at(code_)});
    }

    /**
     * @brief 构造错误对象
     *
     * @param code 备份系统标准错误码，取自本类中的Codes
     * @param extraMsg 追加的详细错误信息
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    BError(Codes code, const std::string_view &extraMsg, DEFINE_SOURCE_LOCATION) : code_(code)
    {
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {mpErrToMsg_.at(code_), extraMsg});
    }

    /**
     * @brief 构造错误对象
     *
     * @param stdErrno 失败的LIBC调用通过errno返回的错误码
     * @param lineNo 构造错误对象的行号（不要自己填写）
     * @param fileName 构造错误对象的文件（不要自己填写）
     * @param functionName 构造错误对象的函数（不要自己填写）
     */
    BError(int stdErrno, DEFINE_SOURCE_LOCATION) : code_ {stdErrno}
    {
        std::string rawMsg = std::generic_category().message(stdErrno);
        msg_ = WrapMessageWithExtraInfos(fileName, lineNo, functionName, code_, {rawMsg});
    }

private:
    static inline const std::map<Codes, std::string_view> mpErrToMsg_ = {
        {Codes::OK, "No error"},
        {Codes::UTILS_INVAL_JSON_ENTITY, "Json utils operated on an invalid file"},
        {Codes::UTILS_INVAL_FILE_HANDLE, "File utils received an invalid file handle"},
        {Codes::UTILS_INVAL_TARBALL_ARG, "Tarball utils received an invalid argument"},
        {Codes::UTILS_INVAL_PROCESS_ARG, "Process utils received an invalid argument"},
        {Codes::SA_INVAL_ARG, "SA received invalid arguments"},
        {Codes::SA_BROKEN_IPC, "SA failed to issue a IPC"},
        {Codes::SA_REFUSED_ACT, "SA refuse to act"},
        {Codes::SDK_INVAL_ARG, "SDK received invalid arguments"},
        {Codes::SDK_BROKEN_IPC, "SDK failed to do IPC"},
        {Codes::SDK_MIXED_SCENARIO, "SDK involed backup/restore when doing the contrary"},
    };

private:
    const int codeSubsystem_ {1};
    Codes code_ {Codes::OK};
    std::string msg_;

private:
    /**
     * @brief 生成如下格式的打印信息 → [文件名:行号->函数名] 默认错误信息. 补充错误信息
     *
     * @param fileName 构造错误对象的文件
     * @param lineNo 构造错误对象的行号
     * @param functionName 构造错误对象的函数
     * @param msgs 所有待追加的错误信息
     * @return std::string 打印信息
     */
    std::string WrapMessageWithExtraInfos(const char *fileName,
                                          int lineNo,
                                          const char *functionName,
                                          Codes code,
                                          const std::vector<std::string_view> &msgs) const;
};
} // namespace Backup
} // namespace FileManagement
} // namespace OHOS

#endif // OHOS_FILEMGMT_BACKUP_B_ERROR_H