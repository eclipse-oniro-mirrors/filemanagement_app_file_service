/*
 * 版权所有 (c) 华为技术有限公司 2022
 */

#include "b_tarball/b_tarball_posix/b_tarball_posix_extended_data.h"

#include <cstdio>

#include "b_resources/b_constants.h"

namespace OHOS::FileManagement::Backup {
using namespace std;

BTarballPosixExtendedData::BTarballPosixExtendedData(const string &pathName, const struct stat &statInfo)
{
    for (BConstants::EntryKey entryKey = BConstants::SUPER_LONG_PATH; entryKey <= BConstants::SUPER_LONG_SIZE;
         entryKey = static_cast<BConstants::EntryKey>(entryKey + 1)) {
        auto extEntryOptional = BTarballPosixExtendedEntry::TryToGetEntry(entryKey, pathName, statInfo);
        if (extEntryOptional != nullopt) {
            entries_.emplace_back(move(extEntryOptional.value()));
        }
    }

    for (BTarballPosixExtendedEntry &entry : entries_) {
        entriesSize_ += entry.GetEntrySize();
    }
}

size_t BTarballPosixExtendedData::GetEntriesSize()
{
    return entriesSize_;
}

void BTarballPosixExtendedData::Publish(const UniqueFd &outFile)
{
    for (BTarballPosixExtendedEntry &entry : entries_) {
        entry.Publish(outFile);
    }
    char block[BConstants::BLOCK_SIZE] {};
    size_t nBytesCompleted = BConstants::BLOCK_SIZE - entriesSize_ % BConstants::BLOCK_SIZE;
    if (nBytesCompleted < BConstants::BLOCK_SIZE) {
        if (write(outFile, block, nBytesCompleted) == -1) {
            printf("BTarballPosixExtendedData::Publish: write\n");
            throw exception();
        }
    }
}
} // namespace OHOS::FileManagement::Backup