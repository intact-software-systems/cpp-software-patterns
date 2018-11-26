#include"NetworkLib/Serialize/SocketWriter.h"
#include"NetworkLib/Socket/IOBase.h"
#include"NetworkLib/Socket/SocketStream.h"

using namespace std;

namespace NetworkLib
{

SocketWriter::SocketWriter(IOWriter* stream)
    : socketStream_(stream)
    , portNumber_(-1)
    , socketType_(AbstractSocket::TcpSocket)
    , swappingNecessary_(false)
    , internalEndian_(EndianConverter::ENDIAN_LITTLE)
    , externalEndian_(EndianConverter::ENDIAN_LITTLE)
    , bytesWritten_(0)
{

}
SocketWriter::SocketWriter(IOWriter* stream, string hostAddress, int portNumber)
    : socketStream_(stream)
    , hostAddress_(hostAddress)
    , portNumber_(portNumber)
    , socketType_(AbstractSocket::UdpSocket)
    , swappingNecessary_(false)
    , internalEndian_(EndianConverter::ENDIAN_LITTLE)
    , externalEndian_(EndianConverter::ENDIAN_LITTLE)
    , bytesWritten_(0)
{

}

SocketWriter::~SocketWriter()
{

}

void SocketWriter::SetEndian(EndianConverter::Endian endian)
{
    externalEndian_ = endian;
    swappingNecessary_ = (externalEndian_ != internalEndian_);
}

EndianConverter::Endian SocketWriter::GetEndian()
{
    return externalEndian_;
}

int SocketWriter::Write(const char *bytes, const int &length)
{
    // TODO: endian conversion
    if(swappingNecessary_)
    {
        EndianConverter::ByteSwap((unsigned char*)bytes, length);
    }

    int bytesWritten = -1;
    switch(socketType_)
    {
        case AbstractSocket::TcpSocket:
        {
            bytesWritten = socketStream_->Write(bytes, length);
            break;
        }
        case AbstractSocket::UdpSocket:
        {
            bytesWritten = socketStream_->WriteDatagram(bytes, length, hostAddress_, portNumber_);
            break;
        }
        default:
            throw BaseLib::Exception("ScketWriter::Write(bytes, length) Unknown socket type!");
            break;
    }

    // -- start error checking --
    if(bytesWritten < 0)
    {
        throw BaseLib::Exception("SocketWriter::Write(bytes, length): ERROR! Could not write!");
    }
    // -- end error checking --

    this->bytesWritten_ += bytesWritten;

    return bytesWritten;
}

int SocketWriter::WriteInt8(const char &data)
{
    return Write((const char*)&data, sizeof(data));
}

int SocketWriter::WriteInt16(const short &data)
{
    return Write((const char*)&data, sizeof(data));
}

int SocketWriter::WriteInt32(const int32_t &data)
{
    return Write((const char*)&data, sizeof(data));
}

int SocketWriter::WriteInt32s(int32_t num, ...)
{
    int bytesWritten = 0;
    va_list arguments;
    va_start(arguments, num);

    for (int x = 0; x < num; x++ )
    {
        int32_t data = va_arg(arguments, int);
        bytesWritten += WriteInt32(data);
    }

    // Cleans up the list
    va_end(arguments);

    return bytesWritten;
}

int SocketWriter::WriteInt64(const int64_t &data)
{
    return Write((const char*)&data, sizeof(data));
}

int SocketWriter::WriteDouble(const double &data)
{
    return Write((const char*)&data, sizeof(data));
}

int SocketWriter::WriteString(const string &str)
{
    WriteInt32((int32_t)str.length());
    if(str.length() <= 0) return 0;

    return Write((const char*)str.c_str(), (int)str.length());
}

bool SocketWriter::Flush()
{
    return socketStream_->Flush();
}

bool SocketWriter::Flush(IOWriter *writer)
{
    return writer->Flush();
}

bool SocketWriter::Flush(IOWriter *writer, const std::string &hostAddress, const int &port)
{
    IFATAL() << "Not supported!";
    return false;
}

int SocketWriter::GetLength() const
{
    return 0;
}

int64_t SocketWriter::GetBytesWritten() const
{
    return bytesWritten_;
}

bool SocketWriter::IsBuffered() const
{
    return false;
}

bool SocketWriter::SetPosition(int pos)
{
    IWARNING() << "Function does nothing!";
    return false;
}

void SocketWriter::Clear()
{
    IWARNING() << "Function does nothing!";
}

bool SocketWriter::IsEmpty() const
{
    IWARNING() << "Function does nothing!";
    return false;
}

const char* SocketWriter::GetData() const
{
    throw Exception("SocketWriter::GetData(): Function does nothing!");
    return NULL;
}

}

