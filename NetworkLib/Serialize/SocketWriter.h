#pragma once

#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Utility/EndianConverter.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {
class IOWriter;

class DLL_STATE SocketWriter : public SerializeWriter
{
public:
    SocketWriter(IOWriter* stream);
    SocketWriter(IOWriter* stream, std::string hostAddress, int portNumber);
    virtual ~SocketWriter();

    void SetEndian(EndianConverter::Endian endian);
    EndianConverter::Endian GetEndian();

    virtual int Write(const char* bytes, const int& length);
    virtual int WriteInt8(const char& data);
    virtual int WriteInt16(const short& data);
    virtual int WriteInt32(const int32_t& data);
    virtual int WriteInt32s(int32_t num, ...);
    virtual int WriteInt64(const int64_t& data);
    virtual int WriteDouble(const double& data);
    virtual int WriteString(const std::string& str);

    virtual bool Flush();
    virtual bool Flush(IOWriter* writer);
    virtual bool Flush(IOWriter* writer, const std::string& hostAddress, const int& port);

    virtual bool IsEmpty() const;
    virtual const char* GetData() const;

public:
    virtual int GetLength() const;
    virtual bool IsBuffered() const;
    virtual int64_t GetBytesWritten() const;

    virtual bool SetPosition(int pos);
    virtual void Clear();

private:
    IOWriter* socketStream_;

    // only if UDP is used
    std::string                hostAddress_;
    int                        portNumber_;
    AbstractSocket::SocketType socketType_;
    bool                       swappingNecessary_;
    EndianConverter::Endian    internalEndian_;    // the endian of the system on which this code is running (little/big)
    EndianConverter::Endian    externalEndian_;    // the endian of the binary stream to be read (little/big)

private:
    int64_t bytesWritten_;
};

}
