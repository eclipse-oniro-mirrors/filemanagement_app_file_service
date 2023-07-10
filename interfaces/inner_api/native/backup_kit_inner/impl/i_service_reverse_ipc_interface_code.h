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

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_IPC_INTERFACE_CODE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_IPC_INTERFACE_CODE_H

/*SAID: 5203*/
namespace OHOS::FileManagement::Backup {
enum class IServiceReverseInterfaceCode {
    SERVICER_BACKUP_ON_FILE_READY,
    SERVICER_BACKUP_ON_SUB_TASK_STARTED,
    SERVICER_BACKUP_ON_SUB_TASK_FINISHED,
    SERVICER_BACKUP_ON_TASK_FINISHED,
    SERVICER_RESTORE_ON_SUB_TASK_STARTED,
    SERVICER_RESTORE_ON_SUB_TASK_FINISHED,
    SERVICER_RESTORE_ON_TASK_FINISHED,
    SERVICER_RESTORE_ON_FILE_READY,
};

} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_REVERSE_IPC_INTERFACE_CODE_H