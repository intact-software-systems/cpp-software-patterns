#pragma once

#include"NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE Pipe
{
private:
    enum PipeType
    {
        READ  = 0,
        WRITE = 1
    };

public:
    Pipe();
    virtual ~Pipe();

    void Close();
    void CloseRead();
    void CloseWrite();
    bool IsClosed();

    int Write(const char* bytes, const int& length);
    int Read(char* bytes, const int& length);

    int ReadDescriptor() const;
    int WriteDescriptor() const;
    int BytesAvailable() const;

    bool IsValidRead() const;
    bool IsValidWrite() const;

private:
    int fd_[2];
};

}
