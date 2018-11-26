#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"

#ifdef USE_GCC

#include <sys/ioctl.h>

#endif

using namespace std;

namespace NetworkLib {

AbstractSocket::AbstractSocket(SocketMode mode)
    : portNumber_(-1)
    , hostName_()
    , socketDescriptor_(-1)
    , socketMode_(mode)
{
}

AbstractSocket::AbstractSocket(int sock, string hostName, int port, SocketMode mode)
    : portNumber_(port)
    , hostName_(hostName)
    , socketDescriptor_(sock)
    , socketMode_(mode)
{
    if(socketDescriptor_ < 0)
    {
        throw BaseLib::Exception("AbstractSocket::AbstractSocket(socket, hostname, port): ERROR! Client socket is not valid!");
    }
}

AbstractSocket::AbstractSocket(int sock, SocketMode mode)
    : portNumber_(-1)
    , hostName_()
    , socketDescriptor_(sock)
    , socketMode_(mode)
{
    if(socketDescriptor_ < 0)
    {
        throw BaseLib::Exception("AbstractSocket::AbstractSocket(socket): ERROR! Client socket is not valid!");
    }

    hostName_   = peerAddress();
    portNumber_ = peerPort();
}

AbstractSocket::~AbstractSocket(void)
{
    if(socketDescriptor_ >= 0) this->close();
}

int AbstractSocket::Write(const char* bytes, const int& length)
{
    return NetworkFunctions::my_write(socketDescriptor_, bytes, length);
}

int AbstractSocket::Read(char* bytes, const int& length)
{
    return NetworkFunctions::my_recv(socketDescriptor_, bytes, length);
}

int AbstractSocket::ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port)
{
    ICRITICAL() << "Not implemented!";
    return 1;
}

int AbstractSocket::WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port)
{
    ICRITICAL() << "Not implemented!";
    return -1;
}


/*bool AbstractSocket::isValid()
{       
	if(socketDescriptor_ <= -1) return false;

	int ret = send(socketDescriptor_, 0, 0, 0);
	if(ret == -1) return false;

	return true;
}*/

/**
 * @brief AbstractSocket::isValid
 * @return
 */
bool AbstractSocket::isValid()
{
    if(socketDescriptor_ <= -1) return false;

#if 0 //def WIN32
    int val = -1;
    int sz = sizeof(val);
    int err = getsockopt(socketDescriptor_, IPPROTO_TCP, SO_TYPE, (char*)&val, &sz);
    if(err < 0)
    {
        switch(errno)
        {
            case EPIPE:
            case EBADF:
            case ENOTSOCK:
            case ENOTCONN:
                //::close(socketDescriptor_);
                this->close();
                return false;
            case EINVAL:
            default:
                IWARNING()  << " unhandled socket error : " << strerror(errno) << endl;
                break;
        }
    }
#else
    return UtilityFunctions::IsValidSocket(socketDescriptor_);
#endif

    return true;
}

bool AbstractSocket::isBound()
{
    int ret = state();

    switch(ret)
    {
        case 0:
        case 1:
        case 2:
        case 6:
        {
            IWARNING() << "Socket not bound! ret = " << ret;
            return false;
        }
        case 3:
        case 4:
        case 5:
        {
            return true;
        }
        default:
        {
            ICRITICAL() << "ERROR! Could not identify socket state!";
            break;
        }
    }

    return false;
}


void AbstractSocket::disconnectFromHost()
{
    //if(isValid() == false) return;

    flush();

    close();
}

// TODO: Use SO_LINGER to ensure that all unsent data are sent before closing socket
void AbstractSocket::close()
{
    if(socketDescriptor_ <= -1) return;

    UtilityFunctions::ShutdownSocket(socketDescriptor_);
    UtilityFunctions::CloseSocket(socketDescriptor_);

    socketDescriptor_ = -1;
}

bool AbstractSocket::flush()
{
    //IWARNING()  << "WARNING! Function is not implemented!" << endl;
    return true;
}

int AbstractSocket::state() const
{
    int       type;
    socklen_t len = sizeof(type);

#ifdef WIN32
    int ret = getsockopt(socketDescriptor_, SOL_SOCKET, SO_TYPE, (char*)&type, (int*) &len);
#else
    int ret = getsockopt(socketDescriptor_, SOL_SOCKET, SO_TYPE, (char*) &type, &len);
#endif

    if(ret == -1)
    {
        IWARNING() << strerror(errno);
        return 0;
    }

    switch(type)
    {
        case SOCK_STREAM:
            //socketType= AbstractSocket::TcpSocket;
            return 3;
        case SOCK_DGRAM:
            //socketType= AbstractSocket::UdpSocket;
            return 4;
        case SOCK_SEQPACKET:
        case SOCK_RAW:
        case SOCK_RDM:
        default:
            IWARNING() << "socket type not recognized: " << type;
            return 3;
    }

    return 0;
    //if(isValid() == true) return 3;
    //return 0;
}

int64 AbstractSocket::bytesAvailable() const
{
    int64 bytesReady = 0;

    int err = NetworkFunctions::my_ioctl(socketDescriptor_, FIONREAD, bytesReady);
    if(err != 0) return -1;

    return bytesReady;
}

// ret = recv(fd, buf, len, MSG_PEEK);
// Use select(). Are there better alternatives?
bool AbstractSocket::waitForReadyRead(unsigned int milliseconds)
{
    struct timeval tv = {0, 0};

    // Set how long to block.
    tv.tv_sec  = 0;
    tv.tv_usec = milliseconds * 1000;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(socketDescriptor_, &fdset);

    int retval = select(socketDescriptor_ + 1, &fdset, NULL, NULL, (struct timeval*) &tv);
    if(retval < 0)
    {
        stringstream stream;
        stream << PRETTY_FUNCTION << "ERROR! " << strerror(errno);
        cout << stream.str() << endl;
        throw BaseLib::Exception(stream.str());
        return false;
    }
    //else if(retval)
    //	return true;

    // -- experiment starts --
    //IDEBUG() << " peaking " << socketDescriptor_ << endl;
    int     length      = 1;
    char    buf;
    ssize_t recevRetVal = ::recv(socketDescriptor_, &buf, length, MSG_PEEK); // | MSG_DONTWAIT);
    if(recevRetVal == -1)
    {
        switch(errno)
        {
            case EPIPE:
            case ECONNRESET:
            case ECONNREFUSED:
            case ENOTSOCK:
            case ENOTCONN:
            case EINVAL:
            case EBADF:
            {
                stringstream stream;
                stream << PRETTY_FUNCTION << " bad file descriptor " << socketDescriptor_ << "err: " << strerror(errno) << endl;
                cout << stream.str();
                throw BaseLib::Exception(stream.str());
                break;
            }
            case EIO:
            case EINTR:
            case EAGAIN:
                break;
            default:
                IWARNING() << " read undefined error : " << strerror(errno);
                //return -1;
                break;
        }
    }
    else if(recevRetVal == 0)
    {
        stringstream stream;
        stream << PRETTY_FUNCTION << " peer reset descriptor " << socketDescriptor_ << "err: " << strerror(errno) << endl;
        cout << stream.str();
        throw BaseLib::Exception(stream.str());
    }
    // -- experiment ends --

    if(!FD_ISSET(socketDescriptor_, &fdset))
    {
        return false;
    }

    return true;
}

AbstractSocket::SocketType AbstractSocket::socketType() const
{
    int type;
#ifdef WIN32
    int len = sizeof(type);
#else
    socklen_t len = sizeof(type);
#endif

    int ret = getsockopt(socketDescriptor_, SOL_SOCKET, SO_TYPE, (char*) &type, &len); //sizeof(type));
    if(ret == -1)
    {
        IWARNING() << strerror(errno);
        return AbstractSocket::SocketError;
    }

    SocketType socketType = AbstractSocket::UnknownSocketType;
    switch(type)
    {
        case SOCK_STREAM:
            socketType = AbstractSocket::TcpSocket;
            break;
        case SOCK_DGRAM:
            socketType = AbstractSocket::UdpSocket;
            break;
        case SOCK_SEQPACKET:
        case SOCK_RAW:
        case SOCK_RDM:
        default:
            socketType = AbstractSocket::UnknownSocketType;
            ICRITICAL() << "socket type not recognized: " << (int) type;
            break;
    }

    return socketType;
}


/*void AbstractSocket::SetReceiveBufferSize(int bufferSize)
{
    cout << "AbstractSocket::SetReceiveBufferSize ignored" << endl;

    //if(bufferSize<1024*1024)
    //    bufferSize = 1024*1024;

    //if(setsockopt(socketDescriptor_, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize)))
    //    throw runtime_error("Could not set the receive buffer size for the socket");
    //int actualBufferSize = GetReceiveBufferSize();
    //if(actualBufferSize < bufferSize)
    //    cout << "Could not set the receive buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    //else
    //    cout << "Receive buffer size: " << actualBufferSize/1024 << " KB." << endl;
}*/


int AbstractSocket::GetReceiveBufferSize()
{
    return NetworkFunctions::getReceiveBufferSize(socketDescriptor_);
}


/*void AbstractSocket::SetSendBufferSize(int bufferSize)
{
    cout << "AbstractSocket::SetSendBufferSize ignored" << endl;

    //if(bufferSize<1024*1024)
    //    bufferSize = 1024*1024;
   
    //if(setsockopt(socketDescriptor_, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof(bufferSize)))
    //    throw runtime_error("Could not set the send buffer size for the socket");
    //int actualBufferSize = GetSendBufferSize();
    //if(actualBufferSize < bufferSize)
    //    cout << "Could not set the send buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    //else
    //    cout << "Send buffer size: " << actualBufferSize/1024 << " KB." << endl;
}*/


int AbstractSocket::GetSendBufferSize()
{
    return NetworkFunctions::getSendBufferSize(socketDescriptor_);
}

string AbstractSocket::GetLocalHostName()
{
#ifdef WIN32
    char name[1000];
    int error = gethostname(name, sizeof(name));
    return error ? "" : string(name);
#else
    utsname u;
    uname(&u);
    return u.nodename;
#endif
}


bool AbstractSocket::setSocketDescriptor(int sock)
{
    int ret = send(sock, 0, 0, 0);
    if(ret == -1)
    {
        ICRITICAL() << "Socket " << sock << " not valid! Returning!";
        return false;
    }

    if(isValid())
    {
        IWARNING() << "Socket was already initialized, close socket old socket!";
        close();
    }

    socketDescriptor_ = sock;
    return true;
}

std::string AbstractSocket::peerAddress()
{
    string peerAddress;
    int    ret = NetworkFunctions::getPeerAddress(socketDescriptor_, peerAddress);
    if(ret == -1)
        ICRITICAL() << "Failed to get peer address for socket " << socketDescriptor_;

    return peerAddress;
}

int AbstractSocket::peerPort()
{
    int peerPort;
    int ret = NetworkFunctions::getPeerPort(socketDescriptor_, peerPort);
    if(ret == -1)
        ICRITICAL() << "Failed to get peer port for socket " << socketDescriptor_;

    return peerPort;
}

}


