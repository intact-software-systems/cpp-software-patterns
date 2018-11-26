#include "NetworkLib/Socket/SocketStream.h"

namespace NetworkLib {

SocketStream::SocketStream(AbstractSocket::Ptr abstractSocket)
    : isClosed_(false)
    , abstractSocket_(abstractSocket)
{
}

SocketStream::~SocketStream()
{
}

int SocketStream::Read(char* bytes, const int& length)
{
    MutexLocker lock(&readMutex_);

    int bytesRead = -1;
    if(length <= 0)
        IWARNING() << "Trying to read length 0!";

    switch(abstractSocket_->socketType())
    {
        case AbstractSocket::UdpSocket:
        case AbstractSocket::TcpSocket:
        {
            bytesRead = readDefault(bytes, length);
            break;
        }
        default:
            ICRITICAL() << "Unknown streamType: " << abstractSocket_->socketType();
            break;
    }

    if(!isClosed_)
    {
        if(bytesRead <= -1)
        {
            IDEBUG() << "Could not read from socket";
            throw Exception("Could not read from socket");
        }
    }

    return bytesRead;
}

int SocketStream::ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port)
{
    MutexLocker lock(&readMutex_);

    int bytesRead = -1;
    if(length <= 0)
        IWARNING() << "Trying to read length 0!";

    switch(abstractSocket_->socketType())
    {
        case AbstractSocket::TcpSocket:
        {
            IWARNING() << "Socket is TCP, but you are attempting to read as UDP!";
            bytesRead = readDefault(bytes, length);
            break;
        }
        case AbstractSocket::UdpSocket:
        {
            bytesRead = readDefault(bytes, length, hostAddress, port);
            break;
        }
        default:
            ICRITICAL() << "Unknown streamType: " << abstractSocket_->socketType();
            break;
    }

    if(!isClosed_)
    {
        if(bytesRead <= -1)
        {
            IDEBUG() << "Could not read from socket";
            throw Exception("Could not read from socket");
        }
    }

    return bytesRead;
}

// TODO: What if socket is non-blocking?
// then read will return immediately when there is nothing to read!
int SocketStream::readDefault(char* bytes, const int& length)
{
    //ASSERT(abstractSocket_->isValid());
    //if(abstractSocket_->bytesAvailable() < length && length >= 0)
    //	abstractSocket_->waitForReadyRead(1000);

    /*int bytesAvailable = 0;
    while(bytesAvailable < length && length > 0)
    {
        bytesAvailable = abstractSocket_->bytesAvailable();
        if(bytesAvailable <= -1) return -1;

        if(bytesAvailable < length)
        {
            if(abstractSocket_->isValid() == false) return -1;

            abstractSocket_->waitForReadyRead(1000);
        }
    }*/

    //cout << "." ;
    return abstractSocket_->Read(bytes, length);
}

int SocketStream::readDefault(char* bytes, const int& length, std::string& hostAddress, int& port)
{
    if(abstractSocket_->bytesAvailable() < length && length > 0)
    {
        abstractSocket_->waitForReadyRead(1000);
    }

    return abstractSocket_->ReadDatagram(bytes, length, hostAddress, port);
}

int SocketStream::Write(const char* bytes, const int& length)
{
    ASSERT(bytes != NULL);

    MutexLocker lock(&writeMutex_);

    int bytesWritten = 0;

    switch(abstractSocket_->socketType())
    {
        case AbstractSocket::UdpSocket:
        case AbstractSocket::TcpSocket:
        {
            bytesWritten = writeDefault(bytes, length);
            break;
        }
        default:
            ICRITICAL() << "Unknown streamType: " << abstractSocket_->socketType();
            break;
    }

    // --------------------------------------------------------
    // ignore read error if the network stream has been closed
    // --------------------------------------------------------
    if(!isClosed_)
    {
        if(bytesWritten <= -1)
        {
            throw Exception("Could not write to socket!");
        }
        if(bytesWritten == 0)
        {
            throw Exception("Tried to write to a gracefully closed socket!");
        }
    }

    ASSERT(bytesWritten == length);

    return bytesWritten;
}

int SocketStream::WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port)
{
    ASSERT(bytes != NULL);

    MutexLocker lock(&writeMutex_);

    int bytesWritten = 0;

    switch(abstractSocket_->socketType())
    {
        case AbstractSocket::TcpSocket:
        {
            IWARNING() << "Socket is TCP, but you are attempting to write as UDP!";
            bytesWritten = writeDefault(bytes, length);
            break;
        }
        case AbstractSocket::UdpSocket:
        {
            bytesWritten = writeDefault(bytes, length, hostAddress, port);
            break;
        }
        default:
            ICRITICAL() << "Unknown streamType: " << abstractSocket_->socketType();
            break;
    }

    // --------------------------------------------------------
    // ignore read error if the network stream has been closed
    // --------------------------------------------------------
    if(!isClosed_)
    {
        if(bytesWritten <= -1)
        {
            throw Exception("Could not write to socket!");
        }
        if(bytesWritten == 0)
        {
            throw Exception("Tried to write to a gracefully closed socket!");
        }
    }

    ASSERT(bytesWritten == length);

    return bytesWritten;
}

int SocketStream::writeDefault(const char* bytes, const int& length)
{
    return abstractSocket_->Write(bytes, length);
}

int SocketStream::writeDefault(const char* bytes, const int& length, const std::string& hostAddress, const int& port)
{
    return abstractSocket_->WriteDatagram(bytes, length, hostAddress, port);
}

int SocketStream::Available() const
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->bytesAvailable();
}

bool SocketStream::IsConnected() const
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->isValid();
}

bool SocketStream::IsBound() const
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->isBound();
}

void SocketStream::Close()
{
    MutexLocker lock(&mutexLock_);
    abstractSocket_->close();
    isClosed_ = true;
}

bool SocketStream::Flush()
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->flush();
}

int SocketStream::socketDescriptor() const
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->socketDescriptor();
}

AbstractSocket::SocketType SocketStream::GetSocketType() const
{
    MutexLocker lock(&mutexLock_);
    return abstractSocket_->socketType();
}

} // namespace NetworkLib

