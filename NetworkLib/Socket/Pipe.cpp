#include "NetworkLib/Socket/Pipe.h"
#include "NetworkLib/Utility/NetworkFunctions.h"
#include "NetworkLib/Utility/UtilityFunctions.h"

#ifdef USE_GCC

#include <sys/ioctl.h>

#elif WIN32
#include <io.h>
#endif

namespace NetworkLib {

void logPipeError(int error)
{
    //[EFAULT]           The fildes buffer is in an invalid area of the process's address space.
    //[EMFILE]           Too many descriptors are active.
    //[ENFILE]           The system file table is full.

    switch(error)
    {
        case EFAULT:
            ICRITICAL() << "[EFAULT] The fildes buffer is in an invalid area of the process's address space.";
            break;
        case EMFILE:
            ICRITICAL() << "[EMFILE] Too many descriptors are active.";
            break;
        case ENFILE:
            ICRITICAL() << "[ENFILE] The system file table is full";
            break;
        default:
            ICRITICAL() << "Unknown pipe error occured";
            break;
    }
}

Pipe::Pipe()
{
#ifdef USE_GCC
    if(::pipe(fd_) != 0)
    {
        logPipeError(errno);
    }
#elif WIN32
    _pipe( fd_, 256, O_BINARY );
#endif
}

Pipe::~Pipe()
{
    Close();
}

void Pipe::Close()
{
    ::close(fd_[READ]);
    ::close(fd_[WRITE]);
}

void Pipe::CloseRead()
{
    ::close(fd_[READ]);
}

void Pipe::CloseWrite()
{
    ::close(fd_[WRITE]);
}

bool Pipe::IsClosed()
{
    return !IsValidRead() && !IsValidWrite();
}

int Pipe::Write(const char* bytes, const int& length)
{
    return NetworkFunctions::my_write(fd_[WRITE], bytes, length);
}

int Pipe::Read(char* bytes, const int& length)
{
    return NetworkFunctions::my_read(fd_[READ], bytes, length);
}

int Pipe::ReadDescriptor() const
{
    return fd_[READ];
}

int Pipe::WriteDescriptor() const
{
    return fd_[WRITE];
}

int Pipe::BytesAvailable() const
{
    int64 bytesReady = 0;

    int err = NetworkFunctions::my_ioctl(fd_[READ], FIONREAD, bytesReady);
    if(err != 0) return -1;

    return bytesReady;
}

bool Pipe::IsValidRead() const
{
    return UtilityFunctions::IsValidReadPipe(this->fd_[READ]);
}

bool Pipe::IsValidWrite() const
{
    return UtilityFunctions::IsValidWritePipe(this->fd_[WRITE]);
}

}
