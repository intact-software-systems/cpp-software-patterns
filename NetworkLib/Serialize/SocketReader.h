#pragma once

#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/Utility/EndianConverter.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Export.h"

namespace NetworkLib {

class IOReader;

class DLL_STATE SocketReader : public SerializeReader
{
public:
    SocketReader(IOReader* stream);
    SocketReader(IOReader* stream, std::string hostAddress, int portNumber);
    virtual ~SocketReader();

    void SetEndian(EndianConverter::Endian endian);
    EndianConverter::Endian GetEndian();

public:
    virtual void Read(char* bytes, const int& length);
    virtual char ReadInt8();
    virtual short ReadInt16();
    virtual int32_t ReadInt32();

    /*int* ReadInt32s(int num)
    {
        int *vals = new int[num];

        for(int i = 0; i < num; i++)
        {
            int data = ReadInt32();
            vals[i] = data;
        }

        return vals;
    }*/

    virtual int64_t ReadInt64();
    virtual double ReadDouble();
    virtual std::string ReadString();

public:
    virtual bool SetPosition(int pos);
    virtual void Clear();

    virtual bool IsEmpty() const;
    virtual bool IsBuffered() const;

    virtual int64_t GetBytesRead() const;
    virtual int GetLength() const;

private:
    IOReader* socketStream_;

    // only if UDP is used
    std::string                hostAddress_;
    int                        portNumber_;
    AbstractSocket::SocketType socketType_;
    bool                       swappingNecessary_;
    EndianConverter::Endian    internalEndian_;    // the endian of the system on which this code is running (little/big)
    EndianConverter::Endian    externalEndian_;    // the endian of the binary stream to be read (little/big)

private:
    int64_t bytesRead_;
};

}

