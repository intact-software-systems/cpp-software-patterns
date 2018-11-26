#ifndef NetworkLib_SerializeBase_IsIncluded
#define NetworkLib_SerializeBase_IsIncluded

#include<string>
#include<stdlib.h>
#include<stdint.h>

#include "NetworkLib/Socket/IOBase.h"
#include "NetworkLib/Export.h"
namespace NetworkLib
{
class DLL_STATE SerializeWriter
{
public:
    CLASS_TRAITS(SerializeWriter)

    virtual int Write(const char *bytes, const int &length) = 0;
    virtual int WriteInt8(const char &data) = 0;
    virtual int WriteInt16(const short &data) = 0;
    virtual int WriteInt32(const int32_t &data) = 0;
    virtual int WriteInt64(const int64_t &data) = 0;
    virtual int WriteDouble(const double &data) = 0;
    virtual int WriteString(const std::string &str) = 0;

public:
    virtual bool Flush(IOWriter *writer) = 0;
    virtual bool Flush(IOWriter *writer, const std::string &hostAddress, const int &port) = 0;

    virtual int 	GetLength() const = 0;
    virtual bool 	IsBuffered() const = 0;
    virtual int64_t GetBytesWritten() const = 0;

    virtual bool    SetPosition(int pos) = 0;
    virtual void 	Clear() = 0;
    virtual bool 	IsEmpty() const = 0;

    virtual const char* GetData() const = 0;
};

class DLL_STATE SerializeReader
{
public:
    CLASS_TRAITS(SerializeReader)

    virtual void 		Read(char *bytes, const int &length) = 0;
    virtual char 		ReadInt8() = 0;
    virtual short 		ReadInt16() = 0;
    virtual int32_t 	ReadInt32() = 0;
    virtual int64_t 	ReadInt64() = 0;
    virtual double 		ReadDouble() = 0;
    virtual std::string ReadString() = 0;

public:
    virtual bool    SetPosition(int pos) = 0;
    virtual void 	Clear() = 0;

    virtual bool 	IsEmpty() const = 0;
    virtual bool 	IsBuffered() const = 0;

    virtual int64_t GetBytesRead() const = 0;
    virtual int 	GetLength() const = 0;
};

} // namespace NetworkLib

#endif // NetworkLib_SerializeBase_IsIncluded


