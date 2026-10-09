/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "b_error/b_error.h"
#include "file_ex.h"
#include "test_manager.h"
#include "untar_file.h"

namespace OHOS::FileManagement::Backup {
using namespace std;
using namespace testing;

static void ClearCache();

class UntarFileTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override {};

    static constexpr const char *TEST_DIR = "/data/local/tmp/untar_real_test";

    static void SetUpTestDir()
    {
        filesystem::remove_all(TEST_DIR);
        filesystem::create_directories(TEST_DIR);
    }

    static void TearDownTestDir()
    {
        filesystem::remove_all(TEST_DIR);
    }

    /**
     * Build a minimal tar file containing one regular file.
     * The tar has: 512-byte header + file data (padded to 512) + two 512-byte zero blocks.
     * Sets tarFilePtr_, tarFileSize_, pos_, tarFileBlockCnt_ on the UntarFile singleton
     * so that ParseRegularFile can read the file data portion.
     */
    static void BuildTarAndPrepare(const string &tarPath, const string &fileName,
                                   const string &content, off_t mtime, mode_t mode,
                                   const string &destPath)
    {
        TarHeader header;
        memset(&header, 0, sizeof(header));
        strncpy(header.name, fileName.c_str(), sizeof(header.name) - 1);
        snprintf(header.mode, sizeof(header.mode), "%07o", mode);
        snprintf(header.uid, sizeof(header.uid), "%07o", 0);
        snprintf(header.gid, sizeof(header.gid), "%07o", 0);
        snprintf(header.size, sizeof(header.size), "%011zo", content.size());
        snprintf(header.mtime, sizeof(header.mtime), "%011llo", static_cast<long long>(mtime));
        header.typeFlag = REGTYPE;
        strncpy(header.magic, TMAGIC.c_str(), TMAGIC_LEN - 1);
        header.version[0] = '0';
        header.version[1] = '0';

        // Calculate checksum (checksum field treated as spaces)
        memset(header.chksum, ' ', CHKSUM_LEN);
        uint32_t chksum = 0;
        const uint8_t *p = reinterpret_cast<const uint8_t *>(&header);
        for (size_t i = 0; i < sizeof(header); i++) {
            chksum += p[i];
        }
        snprintf(header.chksum, sizeof(header.chksum), "%06o", chksum);
        header.chksum[6] = '\0';
        header.chksum[7] = ' ';

        // Write tar file: header + data (padded) + two zero blocks
        ofstream tar(tarPath, ios::binary);
        ASSERT_TRUE(tar.is_open());
        tar.write(reinterpret_cast<const char *>(&header), sizeof(header));

        tar.write(content.data(), content.size());
        size_t paddedSize = ((content.size() + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
        size_t padding = paddedSize - content.size();
        string pad(padding, '\0');
        tar.write(pad.data(), padding);

        string zeroBlock(BLOCK_SIZE, '\0');
        tar.write(zeroBlock.data(), BLOCK_SIZE);
        tar.write(zeroBlock.data(), BLOCK_SIZE);
        tar.close();

        // Prepare UntarFile singleton: open tar file, position at data start
        auto &untar = UntarFile::GetInstance();
        untar.rootPath_ = "";

        untar.tarFilePtr_ = fopen(tarPath.c_str(), "rb");
        ASSERT_NE(untar.tarFilePtr_, nullptr);

        fseeko(untar.tarFilePtr_, sizeof(header), SEEK_SET);
        untar.pos_ = ftello(untar.tarFilePtr_);
        untar.tarFileSize_ = static_cast<off_t>(content.size());
        untar.tarFileBlockCnt_ = (static_cast<off_t>(content.size()) + BLOCK_SIZE - 1) / BLOCK_SIZE;
        untar.readCnt_ = BLOCK_SIZE;
    }

    static void CleanupUntar()
    {
        auto &untar = UntarFile::GetInstance();
        if (untar.tarFilePtr_ != nullptr) {
            fclose(untar.tarFilePtr_);
            untar.tarFilePtr_ = nullptr;
        }
        untar.tarFileSize_ = 0;
        untar.tarFileBlockCnt_ = 0;
        untar.pos_ = 0;
        untar.readCnt_ = 0;
        untar.rootPath_.clear();
    }

    struct FileProps {
        string content;
        mode_t mode {0644};
        off_t mtime {0};
        long mtimeNsec {0};
        off_t atime {0};
        long atimeNsec {0};
    };

    static void MakeSourceFile(const string &path, const FileProps &props)
    {
        {
            ofstream ofs(path, ios::binary);
            ofs.write(props.content.data(), props.content.size());
            ofs.close();
        }
        EXPECT_EQ(chmod(path.c_str(), props.mode), 0);
        struct timespec ts[2] = {{props.atime, props.atimeNsec}, {props.mtime, props.mtimeNsec}};
        EXPECT_EQ(utimensat(AT_FDCWD, path.c_str(), ts, 0), 0);
    }

    static void VerifyRestoredFile(const string &path, const FileProps &expected, bool checkAtime)
    {
        ifstream ifs(path, ios::binary);
        ASSERT_TRUE(ifs.is_open());
        string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
        ifs.close();
        EXPECT_EQ(readContent, expected.content);
        struct stat st;
        ASSERT_EQ(stat(path.c_str(), &st), 0);
        EXPECT_EQ(st.st_mtim.tv_sec, expected.mtime) << "mtime should be preserved";
        EXPECT_EQ(st.st_mtim.tv_nsec, expected.mtimeNsec) << "mtime nsec should be preserved";
        if (checkAtime) {
            EXPECT_EQ(st.st_atim.tv_sec, expected.atime) << "atime should be preserved";
            EXPECT_EQ(st.st_atim.tv_nsec, expected.atimeNsec) << "atime nsec should be preserved";
        }
        EXPECT_EQ(st.st_size, static_cast<off_t>(expected.content.size()));
        EXPECT_EQ(st.st_mode & 0777, expected.mode) << "mode should be preserved";
    }

    static string DoPacketAndUnPacket(const vector<pair<string, string>> &srcFiles,
                                      const string &pkgName, const string &srcDir,
                                      const string &restoreDir)
    {
        TarMap tarMap {};
        auto reportCb = [](std::string msg, int err) {};
        EXPECT_TRUE(TarFile::GetInstance().Packet(srcFiles, pkgName, srcDir, tarMap, reportCb));
        string tarFile = srcDir + "/" + pkgName + ".0.tar";
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, restoreDir);
        EXPECT_EQ(ret, 0);
        ClearCache();
        return tarFile;
    }

    /**
     * GenRealPath strips the leading '/' from absolute paths, so the restored
     * file may be created at a relative path (relative to CWD) instead of the
     * expected absolute path. This helper checks both the absolute path and
     * the relative path (with leading '/' stripped) and returns whichever exists.
     */
    static string ResolveRestoredPath(const string &absPath)
    {
        if (access(absPath.c_str(), F_OK) == 0) {
            return absPath;
        }
        if (absPath[0] == '/') {
            string relPath = absPath.substr(1);
            if (access(relPath.c_str(), F_OK) == 0) {
                return relPath;
            }
        }
        return absPath;
    }
};

void UntarFileTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << "SetUpTestCase enter";
}

void UntarFileTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << "TearDownTestCase enter";
}

static void ClearCache()
{
    UntarFile::GetInstance().rootPath_.clear();
    UntarFile::GetInstance().tarFileSize_ = 0;
    UntarFile::GetInstance().tarFileBlockCnt_ = 0;
    UntarFile::GetInstance().pos_ = 0;
    UntarFile::GetInstance().readCnt_ = 0;
    if (UntarFile::GetInstance().tarFilePtr_ != nullptr) {
        fclose(UntarFile::GetInstance().tarFilePtr_);
        UntarFile::GetInstance().tarFilePtr_ = nullptr;
    }
    TarFile::GetInstance().fileCount_ = 0;
    TarFile::GetInstance().tarMap_.clear();
    TarFile::GetInstance().rootPath_.clear();
    TarFile::GetInstance().packagePath_.clear();
    TarFile::GetInstance().baseTarName_.clear();
    TarFile::GetInstance().tarFileName_.clear();
    TarFile::GetInstance().ioBuffer_.clear();
    TarFile::GetInstance().currentTarName_.clear();
    TarFile::GetInstance().currentTarFileSize_ = 0;
    TarFile::GetInstance().tarFileCount_ = 0;
    TarFile::GetInstance().currentFileName_.clear();
    if (TarFile::GetInstance().currentTarFile_ != nullptr) {
        fclose(TarFile::GetInstance().currentTarFile_);
        TarFile::GetInstance().currentTarFile_ = nullptr;
    }
}

/**
 * @tc.number: SUB_Untar_File_GetInstance_0100
 * @tc.name: SUB_Untar_File_GetInstance_0100
 * @tc.desc: 测试 GetInstance 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_GetInstance_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_GetInstance_0100";
    try {
        UntarFile::GetInstance();
        EXPECT_TRUE(true);
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_GetInstance_0100";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0100
 * @tc.name: SUB_Untar_File_UnPacket_0100
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0100";
    try {
        string tarFile("");
        string rootPath("");
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, ENOENT);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0100";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0200
 * @tc.name: SUB_Untar_File_UnPacket_0200
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0200";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0200");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "/a.txt";
        string bFile = testDir + "/b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        string tarFile = root + "test.tar";
        string cmd = "tar -cvf " + tarFile + " " + testDir;
        string rootPath(root);
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0200";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0300
 * @tc.name: SUB_Untar_File_UnPacket_0300
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0300";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0300");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "/a.txt";
        SaveStringToFile(aFile, "hello");

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(aFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0300";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0400
 * @tc.name: SUB_Untar_File_UnPacket_0400
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0400";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0400");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string tarFile = root + "test.tar";
        string cmd = "tar -cvf " + tarFile + " " + testDir;
        if (system(cmd.c_str()) != 0) {
            GTEST_LOG_(INFO) << " execute tar failure, errno :" << errno;
            throw BError(errno);
        }

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0400";
}

/**
 * @tc.number: SUB_Untar_File_UnPacket_0500
 * @tc.name: SUB_Untar_File_UnPacket_0500
 * @tc.desc: 测试 UnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_UnPacket_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_UnPacket_0500";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_UnPacket_0500");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        string tarFile = root + "/test.0.tar";
        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string rootPath(root);
        auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, rootPath);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_UnPacket_0500";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0100
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0100
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0100";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0100");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0100";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0200
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0200
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0200";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0200");
        string root = tm.GetRootDirCurTest();
        // 构造tarFile不存在
        string tarFile = root + "/empty.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 2); // 错误码2表示找不到文件或路径
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0200";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0300
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0300
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0300";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0300");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = "a.txt";
        SaveStringToFile(aFile, "hello");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        // 构造文件标头为空
        FILE *currentTarFile = fopen(tarFile.c_str(), "wb+");
        fwrite("\0", sizeof(uint8_t), 1, currentTarFile);
        fclose(currentTarFile);
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0300";
}

/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0400
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0400
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0400";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0400");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir;
        string bFile = testDir;
        // 循环100次，用来构造超长路径和超长文件名
        for (int i = 0; i < 100; i++) {
            aFile += "test001/test002/test003/test004/test005/";
            bFile += "ab";
        }
        aFile += "a.txt";
        bFile += ".txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");

        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);

        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        // 构造文件标头为空
        FILE *currentTarFile = fopen(tarFile.c_str(), "wb+");
        fseeko(currentTarFile, 1L, SEEK_SET);
        fwrite("\0", sizeof(uint8_t), 1, currentTarFile);
        fclose(currentTarFile);
        auto [ret, fileInfos, errFileInfos] =
            UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(ret, 0);
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0400";
}


/**
 * @tc.number: SUB_Untar_File_IncrementalUnPacket_0500
 * @tc.name: SUB_Untar_File_IncrementalUnPacket_0500
 * @tc.desc: 测试 IncrementalUnPacket 接口
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 * @tc.require: I6F3GV
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_IncrementalUnPacket_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_IncrementalUnPacket_0500";
    try {
        // 预置文件和目录
        TestManager tm("SUB_Untar_File_IncrementalUnPacket_0500");
        string root = tm.GetRootDirCurTest();
        string testDir = root + "/testdir/";
        if (mkdir(testDir.data(), S_IRWXU) && errno != EEXIST) {
            GTEST_LOG_(INFO) << " invoked mkdir failure, errno :" << errno;
            throw BError(errno);
        }
        string aFile = testDir + "a.txt";
        string bFile = testDir + "b.txt";
        SaveStringToFile(aFile, "hello");
        SaveStringToFile(bFile, "world");
        // 打 tar
        TarMap tarMap {};
        vector<string> smallFiles;
        smallFiles.emplace_back(aFile);
        smallFiles.emplace_back(bFile);
        auto reportCb = [](std::string msg, int err) {
            return;
        };
        TarFile::GetInstance().Packet(smallFiles, "test", root, tarMap, reportCb);
        // 首次恢复
        string tarFile = root + "/test.0.tar";
        string rootPath(root);
        unordered_map<string, struct ReportFileInfo> cloudFiles;
        cloudFiles.emplace(make_pair(aFile.substr(1), ReportFileInfo())); // 1: the pos of /
        cloudFiles.emplace(make_pair(bFile.substr(1), ReportFileInfo())); // 1: the pos of /
        auto result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<1>(result).size(), 2); // 1: First ele, 2: 2 files, aFile and bFile
        EXPECT_EQ(std::get<0>(result), 0);
        // 增量恢复，aFile 有变化
        cloudFiles.erase(aFile.substr(1)); // 1: the pos of /
        result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<0>(result), 0);
        EXPECT_EQ(std::get<1>(result).size(), 1); // 1: the first ele, 1: result size -> bFile
        // 恢复归一后 clone等其他场景，rp文件没数据
        cloudFiles.erase(bFile.substr(1)); // 1: the pos of /
        result = UntarFile::GetInstance().IncrementalUnPacket(tarFile, rootPath, cloudFiles);
        EXPECT_EQ(std::get<0>(result), 0);
        EXPECT_EQ(std::get<1>(result).size(), 2); // 1: First ele, 2: 2 files, aFile and bFile
        ClearCache();
    } catch (...) {
        EXPECT_TRUE(false);
        GTEST_LOG_(INFO) << "UntarFileTest-an exception occurred by UntarFile.";
    }
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_IncrementalUnPacket_0500";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_Real_0100
 * @tc.name: SUB_Untar_File_ParseRegularFile_Real_0100
 * @tc.desc: 测试 ParseRegularFile 真实解包后 mtime 正确恢复（小文件，缓冲区内）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParseRegularFile_Real_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParseRegularFile_Real_0100";
    SetUpTestDir();
    string tarPath = string(TEST_DIR) + "/test.tar";
    string destPath = string(TEST_DIR) + "/dest.txt";
    // Small file: 100 bytes — will be buffered in C library buffer (<= st_blksize)
    // This is the exact scenario that triggers the bug without fflush
    string content(100, 'X');
    off_t expectedMtime = 1000000000; // 2001-09-09
    long expectedMtimeNsec = 500000000; // 0.5s
    mode_t expectedMode = 0644;

    BuildTarAndPrepare(tarPath, "dest.txt", content, expectedMtime, expectedMode, destPath);

    FileStatInfo info {};
    info.fullPath = destPath;
    info.mode = expectedMode;
    info.mtime = expectedMtime;
    info.mtimeNsec = expectedMtimeNsec;
    info.atime = 0;
    info.atimeNsec = 0;

    ErrFileInfo errFileInfo = UntarFile::GetInstance().ParseRegularFile(info);
    EXPECT_EQ(errFileInfo.find(destPath), errFileInfo.end())
        << "ParseRegularFile should not report errors";

    // Verify file content
    ifstream ifs(destPath, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    // Verify mtime — this is the key assertion
    struct stat st;
    ASSERT_EQ(stat(destPath.c_str(), &st), 0);
    EXPECT_EQ(st.st_mtim.tv_sec, expectedMtime)
        << "mtime should be preserved, not overwritten by fclose's implicit flush";
    EXPECT_EQ(st.st_mtim.tv_nsec, expectedMtimeNsec)
        << "mtime nsec should be preserved";
    EXPECT_EQ(st.st_mode & 0777, expectedMode);

    CleanupUntar();
    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParseRegularFile_Real_0100";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_Real_0200
 * @tc.name: SUB_Untar_File_ParseRegularFile_Real_0200
 * @tc.desc: 测试 ParseRegularFile 真实解包后 mtime 正确恢复（大文件，跨缓冲区）
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParseRegularFile_Real_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParseRegularFile_Real_0200";
    SetUpTestDir();
    string tarPath = string(TEST_DIR) + "/test_large.tar";
    string destPath = string(TEST_DIR) + "/dest_large.txt";
    // Large file: 600KB — spans multiple READ_BUFF_SIZE blocks, last chunk > st_blksize
    string content(600 * 1024, 'Y');
    off_t expectedMtime = 1234567890;
    mode_t expectedMode = 0644;

    BuildTarAndPrepare(tarPath, "dest_large.txt", content, expectedMtime, expectedMode, destPath);

    FileStatInfo info {};
    info.fullPath = destPath;
    info.mode = expectedMode;
    info.mtime = expectedMtime;
    info.mtimeNsec = 0;
    info.atime = 0;
    info.atimeNsec = 0;

    ErrFileInfo errFileInfo = UntarFile::GetInstance().ParseRegularFile(info);
    EXPECT_EQ(errFileInfo.find(destPath), errFileInfo.end());

    // Verify file content
    ifstream ifs(destPath, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    struct stat st;
    ASSERT_EQ(stat(destPath.c_str(), &st), 0);
    EXPECT_EQ(st.st_mtim.tv_sec, expectedMtime)
        << "mtime should be preserved even for large files";
    EXPECT_EQ(st.st_size, static_cast<off_t>(content.size()));

    CleanupUntar();
    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParseRegularFile_Real_0200";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_Real_0300
 * @tc.name: SUB_Untar_File_ParseRegularFile_Real_0300
 * @tc.desc: 测试 ParseRegularFile 真实解包后 atime 和 mtime 均正确恢复
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParseRegularFile_Real_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParseRegularFile_Real_0300";
    SetUpTestDir();
    string tarPath = string(TEST_DIR) + "/test_atime.tar";
    string destPath = string(TEST_DIR) + "/dest_atime.txt";
    string content(200, 'Z');
    off_t expectedMtime = 1111111111;
    off_t expectedAtime = 2222222222;
    long expectedMtimeNsec = 123456789;
    long expectedAtimeNsec = 987654321;
    mode_t expectedMode = 0600;

    BuildTarAndPrepare(tarPath, "dest_atime.txt", content, expectedMtime, expectedMode, destPath);

    FileStatInfo info {};
    info.fullPath = destPath;
    info.mode = expectedMode;
    info.mtime = expectedMtime;
    info.mtimeNsec = expectedMtimeNsec;
    info.atime = expectedAtime;
    info.atimeNsec = expectedAtimeNsec;

    ErrFileInfo errFileInfo = UntarFile::GetInstance().ParseRegularFile(info);
    EXPECT_EQ(errFileInfo.find(destPath), errFileInfo.end());

    // Verify file content
    ifstream ifs(destPath, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    struct stat st;
    ASSERT_EQ(stat(destPath.c_str(), &st), 0);
    EXPECT_EQ(st.st_mtim.tv_sec, expectedMtime) << "mtime should be preserved";
    EXPECT_EQ(st.st_mtim.tv_nsec, expectedMtimeNsec) << "mtime nsec should be preserved";
    EXPECT_EQ(st.st_atim.tv_sec, expectedAtime) << "atime should be preserved";
    EXPECT_EQ(st.st_atim.tv_nsec, expectedAtimeNsec) << "atime nsec should be preserved";
    EXPECT_EQ(st.st_mode & 0777, expectedMode);

    CleanupUntar();
    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParseRegularFile_Real_0300";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_Real_0400
 * @tc.name: SUB_Untar_File_ParseRegularFile_Real_0400
 * @tc.desc: 测试 ParseRegularFile 真实解包空文件后 mtime 正确恢复
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParseRegularFile_Real_0400, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParseRegularFile_Real_0400";
    SetUpTestDir();
    string tarPath = string(TEST_DIR) + "/test_empty.tar";
    string destPath = string(TEST_DIR) + "/dest_empty.txt";
    string content; // empty file
    off_t expectedMtime = 999999999;
    mode_t expectedMode = 0666;

    BuildTarAndPrepare(tarPath, "dest_empty.txt", content, expectedMtime, expectedMode, destPath);

    FileStatInfo info {};
    info.fullPath = destPath;
    info.mode = expectedMode;
    info.mtime = expectedMtime;
    info.mtimeNsec = 0;
    info.atime = 0;
    info.atimeNsec = 0;

    ErrFileInfo errFileInfo = UntarFile::GetInstance().ParseRegularFile(info);
    EXPECT_EQ(errFileInfo.find(destPath), errFileInfo.end());

    // Verify file content (empty)
    ifstream ifs(destPath, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    struct stat st;
    ASSERT_EQ(stat(destPath.c_str(), &st), 0);
    EXPECT_EQ(st.st_size, 0);
    EXPECT_EQ(st.st_mtim.tv_sec, expectedMtime) << "mtime should be preserved for empty files";

    CleanupUntar();
    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParseRegularFile_Real_0400";
}

/**
 * @tc.number: SUB_Untar_File_ParseRegularFile_Real_0500
 * @tc.name: SUB_Untar_File_ParseRegularFile_Real_0500
 * @tc.desc: 测试 ParseRegularFile mtime=0 时跳过时间设置
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_ParseRegularFile_Real_0500, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_ParseRegularFile_Real_0500";
    SetUpTestDir();
    string tarPath = string(TEST_DIR) + "/test_zero_mtime.tar";
    string destPath = string(TEST_DIR) + "/dest_zero_mtime.txt";
    string content(50, 'W');
    mode_t expectedMode = 0644;

    BuildTarAndPrepare(tarPath, "dest_zero_mtime.txt", content, 0, expectedMode, destPath);

    FileStatInfo info {};
    info.fullPath = destPath;
    info.mode = expectedMode;
    info.mtime = 0; // SetFileTime should return early
    info.mtimeNsec = 0;
    info.atime = 0;
    info.atimeNsec = 0;

    ErrFileInfo errFileInfo = UntarFile::GetInstance().ParseRegularFile(info);
    EXPECT_EQ(errFileInfo.find(destPath), errFileInfo.end());

    // Verify file content
    ifstream ifs(destPath, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    // File should still be created with correct content
    struct stat st;
    ASSERT_EQ(stat(destPath.c_str(), &st), 0);
    EXPECT_EQ(st.st_size, static_cast<off_t>(content.size()));
    // mtime should NOT be 0 (it should be the current time since SetFileTime was skipped)
    EXPECT_NE(st.st_mtim.tv_sec, 0);

    CleanupUntar();
    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_ParseRegularFile_Real_0500";
}

/**
 * @tc.number: SUB_Untar_File_PacketUnPacket_Real_0100
 * @tc.name: SUB_Untar_File_PacketUnPacket_Real_0100
 * @tc.desc: TarFile::Packet 打包单个文件后 UntarFile::UnPacket 解包，验证内容、时间戳（含纳秒）、权限均正确
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_PacketUnPacket_Real_0100, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_PacketUnPacket_Real_0100";
    SetUpTestDir();
    string srcDir = string(TEST_DIR) + "/src";
    filesystem::create_directories(srcDir);
    string restoreDir = string(TEST_DIR) + "/restore";
    filesystem::create_directories(restoreDir);

    string srcFile = srcDir + "/hello.txt";
    FileProps props;
    props.content = "Hello, Packet+UnPacket!";
    props.mode = 0640;
    props.mtime = 1500000000;
    props.mtimeNsec = 250000000;
    props.atime = 1600000000;
    props.atimeNsec = 750000000;
    MakeSourceFile(srcFile, props);

    vector<pair<string, string>> srcFiles = {{srcFile, "src/hello.txt"}};
    DoPacketAndUnPacket(srcFiles, "pkt", srcDir, restoreDir);

    VerifyRestoredFile(ResolveRestoredPath(restoreDir + "/src/hello.txt"), props, true);

    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_PacketUnPacket_Real_0100";
}

/**
 * @tc.number: SUB_Untar_File_PacketUnPacket_Real_0200
 * @tc.name: SUB_Untar_File_PacketUnPacket_Real_0200
 * @tc.desc: TarFile::Packet 打包多个文件（小文件+大文件）后 UntarFile::UnPacket 解包，验证内容、时间戳、权限均正确
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_PacketUnPacket_Real_0200, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_PacketUnPacket_Real_0200";
    SetUpTestDir();
    string srcDir = string(TEST_DIR) + "/src";
    filesystem::create_directories(srcDir);
    string restoreDir = string(TEST_DIR) + "/restore";
    filesystem::create_directories(restoreDir);

    FileProps props1;
    props1.content = string(100, 'A');
    props1.mode = 0600;
    props1.mtime = 1400000000;
    props1.mtimeNsec = 100000000;
    MakeSourceFile(srcDir + "/small.txt", props1);

    FileProps props2;
    props2.content = string(600 * 1024, 'B');
    props2.mode = 0644;
    props2.mtime = 1700000000;
    props2.mtimeNsec = 900000000;
    MakeSourceFile(srcDir + "/large.bin", props2);

    vector<pair<string, string>> srcFiles = {
        {srcDir + "/small.txt", "src/small.txt"},
        {srcDir + "/large.bin", "src/large.bin"},
    };
    DoPacketAndUnPacket(srcFiles, "multi", srcDir, restoreDir);

    VerifyRestoredFile(ResolveRestoredPath(restoreDir + "/src/small.txt"), props1, false);
    VerifyRestoredFile(ResolveRestoredPath(restoreDir + "/src/large.bin"), props2, false);

    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_PacketUnPacket_Real_0200";
}

/**
 * @tc.number: SUB_Untar_File_PacketUnPacket_Real_0300
 * @tc.name: SUB_Untar_File_PacketUnPacket_Real_0300
 * @tc.desc: TarFile::Packet 打包嵌套目录中的文件后 UntarFile::UnPacket 解包，验证内容、时间戳、权限均正确
 * @tc.size: MEDIUM
 * @tc.type: FUNC
 * @tc.level Level 1
 */
HWTEST_F(UntarFileTest, SUB_Untar_File_PacketUnPacket_Real_0300, testing::ext::TestSize.Level1)
{
    GTEST_LOG_(INFO) << "UntarFileTest-begin SUB_Untar_File_PacketUnPacket_Real_0300";
    SetUpTestDir();
    string srcDir = string(TEST_DIR) + "/src";
    string nestedDir = srcDir + "/dir1/dir2";
    filesystem::create_directories(nestedDir);
    string restoreDir = string(TEST_DIR) + "/restore";
    filesystem::create_directories(restoreDir);

    // Create nested file
    string srcFile = nestedDir + "/deep.txt";
    string content = "Deep nested file content";
    mode_t expectedMode = 0660;
    off_t expectedMtime = 1300000000;
    long expectedMtimeNsec = 333333333;
    {
        ofstream ofs(srcFile, ios::binary);
        ofs.write(content.data(), content.size());
        ofs.close();
    }
    chmod(srcFile.c_str(), expectedMode);
    struct timespec ts[2] = {{expectedMtime, 0}, {expectedMtime, expectedMtimeNsec}};
    utimensat(AT_FDCWD, srcFile.c_str(), ts, 0);

    // Packet — use pair<physicalPath, restorePath> to control tar entry name
    TarMap tarMap {};
    vector<pair<string, string>> srcFiles = {{srcFile, "src/dir1/dir2/deep.txt"}};
    auto reportCb = [](std::string msg, int err) {};
    ASSERT_TRUE(TarFile::GetInstance().Packet(srcFiles, "nested", srcDir, tarMap, reportCb));
    string tarFile = srcDir + "/nested.0.tar";

    // UnPacket
    auto [ret, fileInfos, errFileInfos] = UntarFile::GetInstance().UnPacket(tarFile, restoreDir);
    EXPECT_EQ(ret, 0);
    ClearCache();

    // Verify restored file
    string restoredFile = ResolveRestoredPath(restoreDir + "/src/dir1/dir2/deep.txt");
    ifstream ifs(restoredFile, ios::binary);
    ASSERT_TRUE(ifs.is_open());
    string readContent((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
    ifs.close();
    EXPECT_EQ(readContent, content);

    struct stat st;
    ASSERT_EQ(stat(restoredFile.c_str(), &st), 0);
    EXPECT_EQ(st.st_mtim.tv_sec, expectedMtime) << "mtime should be preserved";
    EXPECT_EQ(st.st_mtim.tv_nsec, expectedMtimeNsec) << "mtime nsec should be preserved";
    EXPECT_EQ(st.st_mode & 0777, expectedMode) << "mode should be preserved";

    TearDownTestDir();
    GTEST_LOG_(INFO) << "UntarFileTest-end SUB_Untar_File_PacketUnPacket_Real_0300";
}
} // namespace OHOS::FileManagement::Backup