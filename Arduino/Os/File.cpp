#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <SD.h>

namespace Os {

    File::File() : m_fd(reinterpret_cast<NATIVE_INT_TYPE>(new ::File)), m_mode(OPEN_NO_MODE), m_lastError(0) {}

    File::~File() {
        if (this->m_mode != OPEN_NO_MODE) {
            this->close();
        }
        delete reinterpret_cast<::File*>(m_fd);
    }

    File::Status File::open(const char* fileName, File::Mode mode) {
        return this->open(fileName, mode, true);
    }

    File::Status File::open(const char* fileName, File::Mode mode, bool include_excl) {

        NATIVE_INT_TYPE flags = 0;

        switch (mode) {
            case OPEN_READ:
                flags = O_RDONLY;
                break;
            case OPEN_WRITE:
                flags = O_WRONLY | O_CREAT;
                break;
            case OPEN_SYNC_WRITE:
                flags = O_WRONLY | O_CREAT | O_SYNC;
                break;
            case OPEN_CREATE:
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                if(include_excl) {
                    flags |= O_EXCL;
                }
                break;
            case OPEN_APPEND:
                flags = O_WRONLY | O_CREAT | O_APPEND;
                break;
            default:
                FW_ASSERT(0, mode);
                break;
        }

        *reinterpret_cast<::File*>(m_fd) = SD.open(fileName, flags);

        if(reinterpret_cast<::File*>(m_fd) == nullptr) {
            return File::Status::NOT_OPENED;
        }

        this->m_mode = mode;

        return OP_OK;
    }

    bool File::isOpen() {
        return (reinterpret_cast<::File*>(m_fd) != nullptr);
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {

        if (this->m_mode == OPEN_NO_MODE) {
            return NOT_OPENED;
        }

        if (reinterpret_cast<::File*>(m_fd)->seek(offset)) {
            return OP_OK;
        }

        return OTHER_ERROR;
    }

    File::Status File::read(void * buffer, NATIVE_INT_TYPE &size, bool waitForFull) {
        
        FW_ASSERT(buffer);

        if (this->m_mode == OPEN_NO_MODE) {
            size = 0;
            return NOT_OPENED;
        }

        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        if (reinterpret_cast<::File*>(m_fd)->available()) {
            reinterpret_cast<::File*>(m_fd)->read(static_cast<U8*>(buffer), size);
            return OP_OK;
        }

        return OTHER_ERROR;
    }

    File::Status File::write(const void * buffer, NATIVE_INT_TYPE &size, bool waitForDone) {

        if (this->m_mode == OPEN_NO_MODE) {
            size = 0;
            return NOT_OPENED;
        }

        if (size <= 0) {
            size = 0;
            return BAD_SIZE;
        }

        size = reinterpret_cast<::File*>(m_fd)->write(static_cast<char*>(const_cast<void*>(buffer)));

        if (size == 0) {
            return File::Status::OTHER_ERROR;
        }

        return OP_OK;
    }

    void File::close() {
        if (this->m_mode != OPEN_NO_MODE) {
            (void) reinterpret_cast<::File*>(m_fd)->close();
        }
        this->m_mode = OPEN_NO_MODE;
    }

    NATIVE_INT_TYPE File::getLastError() {
        return 0;
    }

    const char* File::getLastErrorString() {
        return "FileBad";
    }

}