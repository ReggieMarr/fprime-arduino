#include <FpConfig.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <SD.h>

namespace Os {
namespace FileSystem {
Status createDirectory(const char* path) {
    if (SD.exists(path)) {
        return ALREADY_EXISTS;
    }
    if (SD.mkdir(path)) {
        return OP_OK;
    }
    return OTHER_ERROR;
}  // end createDirectory

Status removeDirectory(const char* path) {
    if (!SD.exists(path)) {
        return INVALID_PATH;
    }
    if (SD.rmdir(path)) {
        return OP_OK;
    }
    return OTHER_ERROR;
}  // end removeDirectory

Status readDirectory(const char* path, const U32 maxNum, Fw::String fileArray[]) {
    return OTHER_ERROR;
}

Status removeFile(const char* path) {
    if (!SD.exists(path)) {
        return INVALID_PATH;
    }
    if (SD.remove(path)) {
        return OP_OK;
    }
    return OTHER_ERROR;
}  // end removeFile

Status moveFile(const char* originPath, const char* destPath) {
    Status stat;
    
    stat = Os::FileSystem::copyFile(originPath, destPath);
    if (stat != OP_OK) {
        return stat;
    }

    stat = Os::FileSystem::removeFile(originPath);

    return stat;
}  // end moveFile

Status handleFileError(File::Status fileStatus) {
    return OTHER_ERROR;
}  // end handleFileError

Status copyFile(const char* originPath, const char* destPath) {
    if (!SD.exists(originPath)) {
        return INVALID_PATH;
    }

    ::File origin = SD.open(originPath, FILE_READ);
    ::File dest = SD.open(destPath, FILE_WRITE);

    if (!origin || !dest) {
        return OTHER_ERROR;
    }

    if (origin.isDirectory() || dest.isDirectory()) {
        origin.close();
        dest.close();
        return IS_DIR;
    }

    while (origin.available()) {
        dest.write(origin.read());
    }

    origin.close();
    dest.close();

    return OP_OK;
}  // end copyFile

Status getFileSize(const char* path, FwSizeType& size) {
    if (!SD.exists(path)) {
        return INVALID_PATH;
    }

    ::File fd = SD.open(path);
    if (!fd) {
        return OTHER_ERROR;
    }

    size = fd.size();

    fd.close();

    return OP_OK;
}  // end getFileSize

Status changeWorkingDirectory(const char* path) {
    return OTHER_ERROR;
}  // end changeWorkingDirectory

// Public function to get the file count for a given directory.
Status getFileCount(const char* directory, U32& fileCount) {
    fileCount = 0;

    ::File fd = SD.open(directory);

    ::File entry = nullptr;
    while ((entry = fd.openNextFile())) {
        fileCount = fileCount + 1;
        entry.close();
    }

    return OP_OK;
}  // end getFileCount

Status appendFile(const char* originPath, const char* destPath, bool createMissingDest) {
    return OTHER_ERROR;
}

Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return OTHER_ERROR;
}

}  // namespace FileSystem
}  // namespace Os
