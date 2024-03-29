/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_FILEMGMT_BACKUP_I_SERVICE_IPC_INTERFACE_CODE_H
#define OHOS_FILEMGMT_BACKUP_I_SERVICE_IPC_INTERFACE_CODE_H

/*SAID: 5203*/
namespace OHOS::FileManagement::Backup {
enum class IServiceInterfaceCode {
    SERVICE_CMD_INIT_RESTORE_SESSION,
    SERVICE_CMD_INIT_BACKUP_SESSION,
    SERVICE_CMD_GET_LOCAL_CAPABILITIES,
    SERVICE_CMD_PUBLISH_FILE,
    SERVICE_CMD_APP_FILE_READY,
    SERVICE_CMD_APP_DONE,
    SERVICE_CMD_RESULT_REPORT,
    SERVICE_CMD_START,
    SERVICE_CMD_GET_FILE_NAME,
    SERVICE_CMD_APPEND_BUNDLES_RESTORE_SESSION,
    SERVICE_CMD_APPEND_BUNDLES_BACKUP_SESSION,
    SERVICE_CMD_FINISH,
    SERVICE_CMD_RELSEASE_SESSION,
    SERVICE_CMD_GET_LOCAL_CAPABILITIES_INCREMENTAL,
    SERVICE_CMD_INIT_INCREMENTAL_BACKUP_SESSION,
    SERVICE_CMD_APPEND_BUNDLES_INCREMENTAL_BACKUP_SESSION,
    SERVICE_CMD_PUBLISH_INCREMENTAL_FILE,
    SERVICE_CMD_APP_INCREMENTAL_FILE_READY,
    SERVICE_CMD_APP_INCREMENTAL_DONE,
    SERVICE_CMD_GET_INCREMENTAL_FILE_NAME,
    SERVICE_CMD_GET_BACKUP_INFO,
};
} // namespace OHOS::FileManagement::Backup

#endif // OHOS_FILEMGMT_BACKUP_I_SERVICE_IPC_INTERFACE_CODE_H