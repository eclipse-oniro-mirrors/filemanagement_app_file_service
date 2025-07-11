/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include <cJSON.h>

#include "cJsonMock.h"

CJSON_PUBLIC(cJSON *) cJSON_CreateArray(void)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_CreateArray();
}

CJSON_PUBLIC(cJSON *) cJSON_CreateObject(void)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_CreateObject();
}

CJSON_PUBLIC(char *) cJSON_Print(const cJSON *item)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_Print(item);
}

CJSON_PUBLIC(cJSON *) CJSONParse(const char *value)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_Parse(value);
}

CJSON_PUBLIC(cJSON *) CJSONGetObjectItem(const cJSON *const object, const char *const string)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_GetObjectItem(object, string);
}

CJSON_PUBLIC(void) CJSONDelete(cJSON *item)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_Delete(item);
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToObject(cJSON *object, const char *string, cJSON *item)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_AddItemToObject(object, string, item);
}

CJSON_PUBLIC(int) cJSON_GetArraySize(const cJSON *array)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_GetArraySize(array);
}

CJSON_PUBLIC(cJSON_bool) cJSON_AddItemToArray(cJSON *array, cJSON *item)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_AddItemToArray(array, item);
}

CJSON_PUBLIC(cJSON*) cJSON_AddStringToObject(cJSON * const object, const char * const name, const char * const string)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_AddStringToObject(object, name, string);
}

CJSON_PUBLIC(cJSON_bool) CJSONIsArray(const cJSON * const item)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_IsArray(item);
}

CJSON_PUBLIC(void) CJSONFree(void *object)
{
    return OHOS::FileManagement::Backup::CJson::cJsonPtr->cJSON_free(object);
}