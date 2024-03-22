/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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
#ifndef FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_LOG_H
#define FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_LOG_H

#include "hilog/log.h"
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD004313
#undef  LOG_TAG
#define LOG_TAG "AppFileService"
namespace OHOS {
namespace AppFileService {

#define LOGI(fmt, ...) ((void)HILOG_IMPL(LOG_CORE, LOG_INFO, LOG_DOMAIN, LOG_TAG, \
    "[%{public}s:%{public}d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOGW(fmt, ...) ((void)HILOG_IMPL(LOG_CORE, LOG_WARN, LOG_DOMAIN, LOG_TAG, \
    "[%{public}s:%{public}d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOGD(fmt, ...) ((void)HILOG_IMPL(LOG_CORE, LOG_DEBUG, LOG_DOMAIN, LOG_TAG, \
    "[%{public}s:%{public}d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOGE(fmt, ...) ((void)HILOG_IMPL(LOG_CORE, LOG_ERROR, LOG_DOMAIN, LOG_TAG, \
    "[%{public}s:%{public}d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
#define LOGF(fmt, ...) ((void)HILOG_IMPL(LOG_CORE, LOG_FATAL, LOG_DOMAIN, LOG_TAG, \
    "[%{public}s:%{public}d] " fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__))
} // namespace AppFileService
} // namespace OHOS

#endif // FILEMANAGEMENT_APP_FILE_SERVICE_INTERFACES_INNERKITS_NATIVE_COMMON_LOG_H