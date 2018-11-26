#pragma once

#include "NetworkLib/IncludeExtLibs.h"
#include "NetworkLib/Socket/IOBase.h"
#include "NetworkLib/Export.h"

namespace NetworkLib {

class DLL_STATE SocketReactorStream : public IOReaderWriter, public ENABLE_SHARED_FROM_THIS(SocketReactorStream)
{
public:
    SocketReactorStream();
    virtual ~SocketReactorStream();

    CLASS_TRAITS(SocketReactorStream)

    SocketReactorStream::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    virtual int Read(char* bytes, const int& length);
    virtual int ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port);

    virtual int Write(const char* bytes, const int& length);
    virtual int WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port);

    virtual void Close();
    virtual bool Flush();

};

}
