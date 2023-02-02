/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef LOG_H
#define LOG_H
#include "hilog/log.h"

namespace OHOS {
namespace AppFileService {
const unsigned int APP_LOG_DOMAIN = 0xD004313;
const char APP_LOG_TAG[] = "AppFileService";
static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, APP_LOG_DOMAIN, APP_LOG_TAG};

#define PRINT_LOG(Level, fmt, ...) \
    OHOS::HiviewDFX::HiLog::Level(OHOS::AppFileService::LOG_LABEL, "[%{public}s:%{public}d] " fmt, \
                            __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define LOGI(fmt, ...) PRINT_LOG(Info, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) PRINT_LOG(Warn, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) PRINT_LOG(Error, fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) PRINT_LOG(Fatal, fmt, ##__VA_ARGS__)
} // namespace AppFileService
} // namespace OHOS

#endif // LOG_H