#include"NetworkLib/Serialize/SocketReader.h"
#include"NetworkLib/Socket/IOBase.h"
#include"NetworkLib/Socket/SocketStream.h"

namespace NetworkLib
{

SocketReader::SocketReader(IOReader* stream)
    : socketStream_(stream)
    , portNumber_(-1)
    , socketType_(AbstractSocket::TcpSocket)
    , swappingNecessary_(false)
    , internalEndian_(EndianConverter::ENDIAN_LITTLE)
    , externalEndian_(EndianConverter::ENDIAN_LITTLE)
    , bytesRead_(0)
{

}

SocketReader::SocketReader(IOReader* stream, std::string hostAddress, int portNumber)
    : socketStream_(stream)
    , hostAddress_(hostAddress)
    , portNumber_(portNumber)
    , socketType_(AbstractSocket::UdpSocket)
    , swappingNecessary_(false)
    , internalEndian_(EndianConverter::ENDIAN_LITTLE)
    , externalEndian_(EndianConverter::ENDIAN_LITTLE)
    , bytesRead_(0)
{

}

SocketReader::~SocketReader()
{

}

void SocketReader::SetEndian(EndianConverter::Endian endian)
{
    externalEndian_ = endian;
    swappingNecessary_ = (externalEndian_ != internalEndian_);
}

EndianConverter::Endian SocketReader::GetEndian()
{
    return externalEndian_;
}

void SocketReader::Read(char *bytes, const int &length)
{
    // TODO: endian conversion
    int bytesReceived = 0;
    switch(socketType_)
    {
        case AbstractSocket::TcpSocket:
        {
            while(bytesReceived < length)
            {
                bytesReceived += socketStream_->Read(bytes + bytesReceived, length-bytesReceived);
            }
            break;
        }
        case AbstractSocket::UdpSocket:
        {
            while(bytesReceived < length)
            {
                bytesReceived += socketStream_->ReadDatagram(bytes + bytesReceived, length-bytesReceived, hostAddress_, portNumber_);
            }
            break;
        }
        default:
            throw BaseLib::Exception("ScketReader::Read(bytes, length) Unknown socket type!");
            break;
    }

    // -- start error checking --
    ASSERT(bytesReceived == length);
    if(bytesReceived < length)
    {
        throw BaseLib::Exception("SocketReader::Read(bytes, length): ERROR! Could not read!");
    }
    // -- end error checking --

    bytesRead_ += bytesReceived;

    if(swappingNecessary_)
    {
        EndianConverter::ByteSwap((unsigned char*)bytes, length);
    }
}


char SocketReader::ReadInt8()
{
    char data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

short SocketReader::ReadInt16()
{
    short data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

int32_t SocketReader::ReadInt32()
{
    int32_t data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

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

int64_t SocketReader::ReadInt64()
{
    int64_t data = 0;
    Read((char*)&data, sizeof(data));
    return data;
}

double SocketReader::ReadDouble()
{
    double data = 0.0;
    Read((char*)&data, sizeof(data));
    return data;
}

std::string SocketReader::ReadString()
{
    int length = ReadInt32();
    if(length <= 0) return std::string("");

    char *bytes = new char[length + 1];

    Read(bytes, length);

    bytes[length] = 0;
    std::string str(bytes);
    delete [] bytes;
    return str;
}

bool SocketReader::SetPosition(int pos)
{
    IWARNING() << "Function does nothing!";
    return false;
}

void SocketReader::Clear()
{
    IWARNING() << "Function does nothing!";
}

bool SocketReader::IsEmpty() const
{
    IWARNING() << "Function does nothing!";
    return false;
}

bool SocketReader::IsBuffered() const
{
    return false;
}

int64_t SocketReader::GetBytesRead() const
{
    return bytesRead_;
}

int SocketReader::GetLength() const
{
    IWARNING() << "Function not applicable for non-buffered input-stream!";
    return 0;
}


} // namespace NetworkLib

