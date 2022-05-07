/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_error/b_error.h"

#include <sstream>
#include <sys/syscall.h>

#include "dfx_dump_catcher.h"
#include "filemgmt_libhilog.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

string BError::WrapMessageWithExtraInfos(const char *fileName,
                                         int lineNo,
                                         const char *functionName,
                                         Codes code,
                                         const vector<string_view> &msgs) const
{
    stringstream ss;
    ss << '[' << fileName << ':' << lineNo << " -> " << functionName << ']' << ' ';
    for (size_t i = 0; i < msgs.size(); ++i) {
        ss << msgs[i];
        if (i != msgs.size() - 1) {
            ss << ". ";
        }
    }

    if (code != Codes::OK) {
        string msg;
        HiviewDFX::DfxDumpCatcher().DumpCatch(getpid(), syscall(SYS_gettid), msg);
        ss << endl << msg;
    }

    string res = ss.str();
    HiviewDFX::HiLog::Error(FILEMGMT_LOG_LABEL, "%{public}s", res.c_str());
    return res;
}
} // namespace OHOS::FileManagement::Backup