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

    FileSystem::Status fs_status;
    File::Status file_status;
    FwSizeType fileSize = 0;
    
    // Check if files exist
    if (!SD.exists(originPath)) {
        return INVALID_PATH;
    }
    if (!createMissingDest && !SD.exists(destPath)) {
        return INVALID_PATH;
    }

    Os::File source;
    Os::File destination;

    // Get the file size:
    fs_status = FileSystem::getFileSize(originPath, fileSize);  //!< gets the size of the file (in bytes) at location path
    if (FileSystem::OP_OK != fs_status) {
        return fs_status;
    }

    file_status = source.open(originPath, Os::File::Mode::OPEN_READ);
    if (file_status != File::Status::OP_OK) {
        return OTHER_ERROR;
    }
    
    file_status = destination.open(destPath, Os::File::Mode::OPEN_APPEND);
    if (file_status != File::Status::OP_OK) {
        return OTHER_ERROR;
    }

    static_assert(FILE_SYSTEM_CHUNK_SIZE != 0, "FILE_SYSTEM_CHUNK_SIZE must be >0");
    U8 fileBuffer[FILE_SYSTEM_CHUNK_SIZE];

    const FwSizeType copyLoopLimit = (fileSize / FILE_SYSTEM_CHUNK_SIZE) + 2;

    FwSizeType loopCounter = 0;
    NATIVE_INT_TYPE chunkSize;

    while (loopCounter < copyLoopLimit) {
        chunkSize = FILE_SYSTEM_CHUNK_SIZE;
        file_status = source.read(&fileBuffer, chunkSize, false);
        if (file_status != File::OP_OK) {
            return OTHER_ERROR;
        }

        if (chunkSize == 0) {
            // file has been successfully copied
            break;
        }

        file_status = destination.write(fileBuffer, chunkSize, true);
        if (file_status != File::OP_OK) {
            return OTHER_ERROR;
        }
        loopCounter++;
    }
    FW_ASSERT(loopCounter < copyLoopLimit);

    (void)source.close();
    (void)destination.close();

    return fs_status;
}

Status getFreeSpace(const char* path, FwSizeType& totalBytes, FwSizeType& freeBytes) {
    return OTHER_ERROR;
}

}  // namespace FileSystem
}  // namespace Os
