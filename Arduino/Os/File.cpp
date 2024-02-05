#include <FpConfig.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <SD.h>

namespace Os {

    ::File fd;

    File::File() : m_fd(0), m_mode(OPEN_NO_MODE), m_lastError(0) {}

    File::~File() {
        if (this->m_mode != OPEN_NO_MODE) {
            this->close();
        }
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

        fd = SD.open(fileName, flags);

        if(!fd) {
            return File::Status::NOT_OPENED;
        }

        this->m_mode = mode;

        return OP_OK;
    }

    bool File::isOpen() {
        if(fd) {
            return true;
        }

        return false;
    }

    File::Status File::seek(NATIVE_INT_TYPE offset, bool absolute) {

        if (this->m_mode == OPEN_NO_MODE) {
            return NOT_OPENED;
        }

        if (fd.seek(offset)) {
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

        if (fd.available()) {
            fd.read(static_cast<U8*>(buffer), size);
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

        size = fd.write(static_cast<char*>(const_cast<void*>(buffer)));

        if (size == 0) {
            return File::Status::OTHER_ERROR;
        }

        return OP_OK;
    }

    void File::close() {
        if (this->m_mode != OPEN_NO_MODE) {
            (void) fd.close();
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