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

#include "untar_file.h"

#include <utime.h>

#include "directory_ex.h"
#include "filemgmt_libhilog.h"
#include "securec.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
const int32_t PATH_MAX_LEN = 4096;
const int32_t OCTAL = 8;

static bool IsEmptyBlock(const char *p)
{
    if (p != nullptr) {
        return (p[0] == '\0');
    }
    return true;
}

// 八进制字符串转十进制数字
static off_t ParseOctalStr(const string &octalStr, size_t destLen)
{
    off_t ret = 0;
    string::const_iterator it = octalStr.begin();

    while (it != octalStr.end() && (*it < '0' || *it > '7') && destLen > 0) {
        ++it;
        --destLen;
    }

    while (it != octalStr.end() && *it >= '0' && *it <= '7' && destLen > 0) {
        ret *= OCTAL;
        ret += *it - '0';
        ++it;
        --destLen;
    }

    return ret;
}

static bool ForceCreateDirectoryWithMode(const string& path, mode_t mode)
{
    string::size_type index = 0;
    do {
        index = path.find('/', index + 1);
        string subPath = (index == string::npos) ? path : path.substr(0, index);
        if (access(subPath.c_str(), F_OK) != 0) {
            if (mkdir(subPath.c_str(), mode) != 0) {
                return false;
            }
        }
    } while (index != string::npos);
    return access(path.c_str(), F_OK) == 0;
}

static void RTrimNull(std::string &s)
{
    auto iter = std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return ch != '\0'; });
    s.erase(iter.base(), s.end());
}

static int ReadLongName(FileStatInfo &info, FILE *tarFilePtr_, off_t tarFileSize_)
{
    size_t nameLen = static_cast<size_t>(tarFileSize_);
    if (nameLen <= PATH_MAX_LEN) {
        string tempName("");
        tempName.resize(nameLen);
        size_t read = fread(&(tempName[0]), sizeof(char), nameLen, tarFilePtr_);
        if (read < nameLen) {
            HILOGE("Failed to fread longName of %{private}s", info.fullPath.c_str());
            return -1;
        }
        info.longName = tempName;
    } else {
        HILOGE("longName of %{private}s exceed PATH_MAX_LEN", info.fullPath.c_str());
    }

    return 0;
}

UntarFile &UntarFile::GetInstance()
{
    static UntarFile instance;
    return instance;
}

int UntarFile::UnPacket(const string &tarFile, const string &rootPath)
{
    tarFilePtr_ = fopen(tarFile.c_str(), "rb");
    if (tarFilePtr_ == nullptr) {
        HILOGE("Failed to open tar file %{public}s, err = %{public}d", tarFile.c_str(), errno);
        return errno;
    }

    if (ParseTarFile(rootPath) != 0) {
        HILOGE("Failed to parse tar file");
    }

    fclose(tarFilePtr_);
    tarFilePtr_ = nullptr;

    return 0;
}

int UntarFile::IncrementalUnPacket(const string &tarFile, const string &rootPath,
                                   const unordered_map<string, struct ReportFileInfo> &includes)
{
    includes_ = includes;
    tarFilePtr_ = fopen(tarFile.c_str(), "rb");
    if (tarFilePtr_ == nullptr) {
        HILOGE("Failed to open tar file %{public}s, err = %{public}d", tarFile.c_str(), errno);
        return errno;
    }

    if (ParseIncrementalTarFile(rootPath) != 0) {
        HILOGE("Failed to parse tar file");
    }

    fclose(tarFilePtr_);
    tarFilePtr_ = nullptr;

    return 0;
}

void UntarFile::HandleTarBuffer(const string &buff, const string &name, FileStatInfo &info)
{
    info.mode = static_cast<mode_t>(ParseOctalStr(&buff[0] + TMODE_BASE, TMODE_LEN));
    info.uid = static_cast<uid_t>(ParseOctalStr(&buff[0] + TUID_BASE, TUID_LEN));
    info.gid = static_cast<gid_t>(ParseOctalStr(&buff[0] + TGID_BASE, TGID_LEN));
    info.mtime = ParseOctalStr(&buff[0] + TMTIME_BASE, MTIME_LEN);

    tarFileSize_ = ParseOctalStr(&buff[0] + TSIZE_BASE, TSIZE_LEN);
    tarFileBlockCnt_ = (tarFileSize_ + BLOCK_SIZE - 1) / BLOCK_SIZE;
    pos_ = ftello(tarFilePtr_);

    string realName = name;
    if (!info.longName.empty()) {
        realName = info.longName;
        info.longName.clear();
    }
    if (realName.length() > 0 && realName[0] == '/') {
        info.fullPath = realName.substr(1, realName.length() - 1);
        return;
    }
    info.fullPath = realName;
}

int UntarFile::ParseTarFile(const string &rootPath)
{
    // re-parse tar header
    rootPath_ = rootPath;
    char buff[BLOCK_SIZE] = {0};
    FileStatInfo info {};

    // tarFileSize
    int ret = fseeko(tarFilePtr_, 0L, SEEK_END);
    if (ret != 0) {
        HILOGE("Failed to fseeko tarFileSize SEEK_SET, err = %{public}d", errno);
    }
    tarFileSize_ = ftello(tarFilePtr_);
    // reback file to begin
    ret = fseeko(tarFilePtr_, 0L, SEEK_SET);
    if (ret != 0) {
        HILOGE("Failed to fseeko reback SEEK_SET, err = %{public}d", errno);
    }

    while (1) {
        readCnt_ = fread(buff, 1, BLOCK_SIZE, tarFilePtr_);
        if (readCnt_ < BLOCK_SIZE) {
            HILOGE("Parsing tar file completed, read data count is less then block size.");
            return 0;
        }
        TarHeader *header = reinterpret_cast<TarHeader *>(buff);
        // two empty continuous block indicate end of file
        if (IsEmptyBlock(buff) && header->typeFlag != GNUTYPE_LONGNAME) {
            char tailBuff[BLOCK_SIZE] = {0};
            size_t tailRead = fread(tailBuff, 1, BLOCK_SIZE, tarFilePtr_);
            if (tailRead == BLOCK_SIZE && IsEmptyBlock(tailBuff)) {
                HILOGE("Parsing tar file completed, tailBuff is empty.");
                return 0;
            }
        }
        // check header
        if (!IsValidTarBlock(*header)) {
            // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
            if (ftello(tarFilePtr_) > (tarFileSize_ + READ_BUFF_SIZE) || !IsEmptyBlock(buff)) {
                HILOGE("Invalid tar file format");
                ret = ERR_FORMAT;
            }
            HILOGE("invalid tar block");
            return ret;
        }
        HandleTarBuffer(string(buff, BLOCK_SIZE), header->name, info);
        ParseFileByTypeFlag(header->typeFlag, info);
    }

    return ret;
}

int UntarFile::ParseIncrementalTarFile(const string &rootPath)
{
    // re-parse tar header
    rootPath_ = rootPath;
    char buff[BLOCK_SIZE] = {0};
    FileStatInfo info {};

    // tarFileSize
    int ret = fseeko(tarFilePtr_, 0L, SEEK_END);
    if (ret != 0) {
        HILOGE("Failed to fseeko tarFile SEEK_END, err = %{public}d", errno);
        return ret;
    }
    tarFileSize_ = ftello(tarFilePtr_);
    // reback file to begin
    if ((ret = fseeko(tarFilePtr_, 0L, SEEK_SET)) != 0) {
        HILOGE("Failed to fseeko tarFile SEEK_SET, err = %{public}d", errno);
        return ret;
    }

    do {
        readCnt_ = fread(buff, 1, BLOCK_SIZE, tarFilePtr_);
        if (readCnt_ < BLOCK_SIZE) {
            HILOGE("Parsing tar file completed, read data count is less then block size.");
            return 0;
        }
        TarHeader *header = reinterpret_cast<TarHeader *>(buff);
        // two empty continuous block indicate end of file
        if (IsEmptyBlock(buff) && header->typeFlag != GNUTYPE_LONGNAME) {
            char tailBuff[BLOCK_SIZE] = {0};
            size_t tailRead = fread(tailBuff, 1, BLOCK_SIZE, tarFilePtr_);
            if (tailRead == BLOCK_SIZE && IsEmptyBlock(tailBuff)) {
                HILOGE("Parsing tar file completed, tailBuff is empty.");
                return 0;
            }
        }
        // check header
        if (!IsValidTarBlock(*header)) {
            // when split unpack, ftell size is over than file really size [0,READ_BUFF_SIZE]
            if (ftello(tarFilePtr_) > (tarFileSize_ + READ_BUFF_SIZE) || !IsEmptyBlock(buff)) {
                HILOGE("Invalid tar file format");
                ret = ERR_FORMAT;
            }
            return ret;
        }
        HandleTarBuffer(string(buff, BLOCK_SIZE), header->name, info);
        if ((ret = ParseIncrementalFileByTypeFlag(header->typeFlag, info)) != 0) {
            HILOGE("Failed to parse incremental file by type flag");
            return ret;
        }
    } while (readCnt_ >= BLOCK_SIZE);

    return ret;
}

void UntarFile::ParseFileByTypeFlag(char typeFlag, FileStatInfo &info)
{
    HILOGD("untar file: %{public}s, rootPath: %{public}s", info.fullPath.c_str(), rootPath_.c_str());
    switch (typeFlag) {
        case REGTYPE:
        case AREGTYPE:
            info.fullPath = GenRealPath(rootPath_, info.fullPath);
            ParseRegularFile(info, typeFlag);
            break;
        case SYMTYPE:
            break;
        case DIRTYPE:
            info.fullPath = GenRealPath(rootPath_, info.fullPath);
            CreateDir(info.fullPath, info.mode);
            break;
        case GNUTYPE_LONGNAME: {
            ReadLongName(info, tarFilePtr_, tarFileSize_);
            fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET);
            break;
        }
        default: {
            fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR);
            break;
        }
    }
}

int UntarFile::ParseIncrementalFileByTypeFlag(char typeFlag, FileStatInfo &info)
{
    HILOGD("untar file: %{public}s, rootPath: %{public}s", info.fullPath.c_str(), rootPath_.c_str());
    string tmpFullPath = info.fullPath;
    RTrimNull(tmpFullPath);
    switch (typeFlag) {
        case REGTYPE:
        case AREGTYPE:
            if (!includes_.empty() && includes_.find(tmpFullPath) == includes_.end()) { // not in includes
                if (fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET) != 0) {
                    HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
                    return -1;
                }
                break;
            }
            info.fullPath = GenRealPath(rootPath_, info.fullPath);
            ParseRegularFile(info, typeFlag);
            break;
        case SYMTYPE:
            break;
        case DIRTYPE:
            info.fullPath = GenRealPath(rootPath_, info.fullPath);
            CreateDir(info.fullPath, info.mode);
            break;
        case GNUTYPE_LONGNAME: {
            int ret = ReadLongName(info, tarFilePtr_, tarFileSize_);
            if (ret != 0) {
                return ret;
            }
            if (fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET) != 0) {
                HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
                return -1;
            }
            break;
        }
        default: {
            if (fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR) != 0) {
                HILOGE("Failed to fseeko of %{private}s, err = %{public}d", info.fullPath.c_str(), errno);
                return -1;
            }
            break;
        }
    }

    return 0;
}

void UntarFile::ParseRegularFile(FileStatInfo &info, char typeFlag)
{
    FILE *destFile = CreateFile(info.fullPath, info.mode, typeFlag);
    if (destFile != nullptr) {
        string destStr("");
        destStr.resize(READ_BUFF_SIZE);
        off_t remainSize = tarFileSize_;
        size_t readBuffSize = READ_BUFF_SIZE;
        while (remainSize > 0) {
            if (remainSize < READ_BUFF_SIZE) {
                readBuffSize = remainSize;
            }
            fread(&destStr[0], sizeof(char), readBuffSize, tarFilePtr_);
            fwrite(&destStr[0], sizeof(char), readBuffSize, destFile);
            remainSize -= readBuffSize;
        }
        fclose(destFile);
        chmod(info.fullPath.data(), info.mode);
        struct utimbuf times;
        struct stat attr;
        if (stat(info.fullPath.c_str(), &attr) != 0) {
            HILOGE("Failed to get stat of %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
            times.actime = info.mtime;
        } else {
            times.actime = attr.st_atime;
        }
        times.modtime = info.mtime;
        if (utime(info.fullPath.c_str(), &times) != 0) {
            HILOGE("Failed to set mtime of %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
        }
        // anyway, go to correct
        fseeko(tarFilePtr_, pos_ + tarFileBlockCnt_ * BLOCK_SIZE, SEEK_SET);
    } else {
        HILOGE("Failed to create file %{public}s, err = %{public}d", info.fullPath.c_str(), errno);
        fseeko(tarFilePtr_, tarFileBlockCnt_ * BLOCK_SIZE, SEEK_CUR);
    }
}

bool UntarFile::VerifyChecksum(TarHeader &header)
{
    vector<uint8_t> buffer {};
    buffer.resize(sizeof(header));
    buffer.assign(reinterpret_cast<uint8_t *>(&header), reinterpret_cast<uint8_t *>(&header) + sizeof(header));
    int sum = 0;
    for (uint32_t index = 0; index < BLOCK_SIZE; ++index) {
        if (index < CHKSUM_BASE || index > CHKSUM_BASE + CHKSUM_LEN - 1) {
            // Standard tar checksum adds unsigned bytes.
            sum += (buffer[index] & 0xFF);
        } else {
            sum += BLANK_SPACE;
        }
    }
    string strChksum;
    strChksum.assign(buffer.begin(), buffer.end());
    return (sum == ParseOctalStr(&strChksum[0] + CHKSUM_BASE, CHKSUM_LEN));
}

bool UntarFile::IsValidTarBlock(TarHeader &header)
{
    // check magic && checksum
    if (strncmp(header.magic, TMAGIC.c_str(), TMAGIC_LEN - 1) == 0 && VerifyChecksum(header)) {
        return true;
    }
    HILOGE("Invalid tar block");
    return false;
}

string UntarFile::GenRealPath(const string &rootPath, const string &realName)
{
    if (rootPath.empty() || realName.empty()) {
        return "";
    }
    string realPath(rootPath);
    size_t len = realPath.length();
    if (realPath[len - 1] == '/') {
        realPath = realPath.substr(0, len - 1);
    }
    realPath.append((realName[0] == '/') ? realName : ("/" + realName));
    if (realPath[0] == '/') {
        realPath = realPath.substr(1, realPath.length());
    }
    return realPath;
}

void UntarFile::CreateDir(string &path, mode_t mode)
{
    if (path.empty()) {
        return;
    }
    size_t len = path.length();
    if (path[len - 1] == '/') {
        path[len - 1] = '\0';
    }
    if (access(path.c_str(), F_OK) != 0) {
        HILOGE("directory does not exist, path:%{public}s, err = %{public}d", path.c_str(), errno);
        if (!ForceCreateDirectoryWithMode(path, mode)) {
            HILOGE("Failed to force create directory %{public}s, err = %{public}d", path.c_str(), errno);
        }
    }
}

FILE *UntarFile::CreateFile(string &filePath, mode_t mode, char fileType)
{
    FILE *f = fopen(filePath.c_str(), "wb+");
    if (f != nullptr) {
        return f;
    }

    uint32_t len = filePath.length();
    HILOGE("Failed to open file %{public}d, %{public}s, err = %{public}d", len, filePath.c_str(), errno);
    size_t pos = filePath.rfind('/');
    if (pos == string::npos) {
        return nullptr;
    }

    string path = filePath.substr(0, pos);
    if (ForceCreateDirectory(path)) {
        f = fopen(filePath.c_str(), "wb+");
        if (f == nullptr) {
            HILOGE("Failed to open file %{public}s, err = %{public}d", filePath.c_str(), errno);
        }
    }

    return f;
}

} // namespace OHOS::FileManagement::Backup