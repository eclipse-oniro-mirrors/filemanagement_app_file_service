/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <string>

#include "b_error/b_error.h"
#include "b_resources/b_constants.h"
#include "filemgmt_libn.h"

#include "incremental_backup_data.h"
#include "parse_inc_info_from_js.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace LibN;
bool Parse::CheckDataList(const LibN::NVal &data)
{
    LibN::NVal name = data.GetProp(BConstants::BUNDLE_NAME);
    if (name.val_ == nullptr) {
        HILOGE("name.val is nullptr");
        return false;
    }
    auto [succ, str, ignore] = name.ToUTF8String();
    if (!succ) {
        HILOGE("convert name failed");
        return false;
    }

    LibN::NVal time = data.GetProp(BConstants::LAST_INCREMENTAL_TIME);
    if (time.val_ == nullptr) {
        HILOGE("time.val is nullptr");
        return false;
    }
    tie(succ, ignore) = time.ToInt64();
    if (!succ) {
        HILOGE("convert time failed");
        return false;
    }
    return true;
}

std::tuple<bool, std::vector<BIncrementalData>> Parse::ParseDataList(napi_env env, const napi_value& value)
{
    uint32_t size = 0;
    napi_status status = napi_get_array_length(env, value, &size);
    if (status != napi_ok) {
        HILOGE("Get array length failed!");
        return {false, {}};
    }
    if (size == 0) {
        HILOGI("array length is zero!");
        return {true, {}};
    }

    napi_value result;
    std::vector<BIncrementalData> backupData;
    for (uint32_t i = 0; i < size; i++) {
        status = napi_get_element(env, value, i, &result);
        if (status != napi_ok) {
            HILOGE("Get element failed! index is :%{public}u", i);
            return {false, {}};
        } else {
            NVal element(env, result);
            if (!CheckDataList(element)) {
                HILOGE("bundles are invalid!");
                return {false, {}};
            }
            IncrementalBackupData data(element);
            backupData.emplace_back(data.bundleName,
                                    data.lastIncrementalTime,
                                    data.manifestFd,
                                    data.parameters,
                                    data.priority);
        }
    }
    return {true, backupData};
}

bool Parse::VerifyAndParseParams(napi_env env, LibN::NFuncArg &funcArg,
                                 bool &isPreciseScan, std::vector<BIncrementalData> &bundleNames)
{
    if (!funcArg.InitArgs(NARG_CNT::TWO)) {
        HILOGE("Number of arguments unmatched.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Number of arguments unmatched.").GetCode()).ThrowErr(env);
        return false;
    }

    NVal jsBundleBool(env, funcArg[NARG_POS::FIRST]);
    bool succ;
    tie(succ, isPreciseScan) = jsBundleBool.ToBool();
    if (!succ) {
        HILOGE("First argument is not bool.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "Failed to get isPreciseScan.").GetCode()).ThrowErr(env);
        return false;
    }
    tie(succ, bundleNames) = ParseDataList(env, funcArg[NARG_POS::SECOND]);
    if (!succ) {
        HILOGE("bundles array invalid.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "bundles array invalid.").GetCode()).ThrowErr(env);
        return false;
    }
    if (bundleNames.empty()) {
        HILOGI("BundleName list is empty.");
        NError(BError(BError::Codes::SDK_INVAL_ARG, "BundleName list is empty.").GetCode()).ThrowErr(env);
        return false;
    }
    return true;
}
} // namespace OHOS::FileManagement::Backup