/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

class BackupExtensionAbility {
    onBackup() {
        console.log()
    }

    onRestore(versionBackupedBundle) {
        console.log(versionBackupedBundle)
    }
}

export default BackupExtensionAbility