#include "NetworkLib/Socket/UdpSocket.h"
#include "NetworkLib/Utility/UtilityFunctions.h"

#ifdef USE_GCC
#endif

using namespace std;

namespace NetworkLib {

UdpSocket::UdpSocket()
    : AbstractSocket()
    , timeToLive(1)
    , udpID(UdpSocket::UnicastSender)
{
}

UdpSocket::UdpSocket(UdpID id, string localAddr, int ttl)
    : AbstractSocket()
    , localAddress(localAddr)
    , timeToLive(ttl)
    , udpID(id)
{
}

UdpSocket::UdpSocket(int clientSocket, const string& hostName, int port)
    : AbstractSocket(clientSocket, hostName, port)
    , timeToLive(1)
    , udpID(UdpSocket::UnicastSender)
{
}

UdpSocket::~UdpSocket(void)
{
    close();
}

int UdpSocket::Write(const char* bytes, const int& length)
{
    int ret = 0;
    switch(this->udpID)
    {
        case UdpSocket::BroadcastSender:
        case UdpSocket::UnicastSender:
        case UdpSocket::MulticastSender:
        {
            ret = NetworkFunctions::my_sendto(socketDescriptor_, bytes, length, addr_listener, sizeof(addr_listener));
            break;
        }
        case UdpSocket::MulticastReceiver:
        case UdpSocket::UnicastReceiver:
        case UdpSocket::BroadcastReceiver:
        {
            throw BaseLib::Exception("UdpSocket is initialized as receiver not sender!");
            ret = -1;
            break;
        }
        default:
        {
            throw BaseLib::Exception("ERROR! Could not identify UdpID!");
            break;
        }
    }

    if(ret <= -1)
        IWARNING() << "ERROR! Could not write to socket! ret: " << ret;
    return ret;
}

int UdpSocket::Read(char* bytes, const int& length)
{
    int ret = 0;
    switch(this->udpID)
    {
        case UdpSocket::MulticastReceiver:
        {
            ret = NetworkFunctions::my_recvfrom(socketDescriptor_, bytes, length, addr_listener, sizeof(addr_listener));
            break;
        }
        case UdpSocket::UnicastReceiver:
        case UdpSocket::BroadcastReceiver:
        {
            ret = NetworkFunctions::my_read(socketDescriptor_, bytes, length);
            break;
        }
        case UdpSocket::BroadcastSender:
        case UdpSocket::UnicastSender:
        case UdpSocket::MulticastSender:
        {
            throw BaseLib::Exception("UdpSocket is initialized as sender not receiver!");
            ret = -1;
            break;
        }
        default:
        {
            throw BaseLib::Exception("ERROR! Could not identify UdpID!");
            break;
        }
    }

    if(ret <= -1)
        IWARNING() << "ERROR! Could not read from socket! ret: " << ret;
    return ret;
}

int UdpSocket::ReadDatagram(char* bytes, const int& length, std::string& hostAddress, int& port)
{
    int ret = 0;
    switch(this->udpID)
    {
        case UdpSocket::MulticastReceiver:
        {
            ret = NetworkFunctions::my_recvfrom(socketDescriptor_, bytes, length, addr_listener, sizeof(addr_listener));
            if(ret <= -1)
                IWARNING() << "ERROR! Could not read from socket! ret: " << ret;
            break;
        }
        case UdpSocket::BroadcastSender:
        case UdpSocket::UnicastSender:
            IWARNING() << "UdpSocket is initialized as sender! Reading anyway!";
        case UdpSocket::UnicastReceiver:
        case UdpSocket::BroadcastReceiver:
        {
            struct sockaddr_in peer_addr;
            socklen_t          peer_addr_length = sizeof(peer_addr);

            ret = NetworkFunctions::my_recvfrom(socketDescriptor_, bytes, length, peer_addr, peer_addr_length);
            if(ret <= -1)
            {
                IWARNING() << "ERROR! Could not read from socket! ret: " << ret;
                break;
            }

            hostAddress = UtilityFunctions::GetAddress((sockaddr*) &peer_addr);
            port        = UtilityFunctions::GetPortNtohs((sockaddr*) &peer_addr);

            // -- debug --
            {    // alternative way
                char host[NI_MAXHOST], service[NI_MAXSERV];
                int  s = getnameinfo((struct sockaddr*) &peer_addr, peer_addr_length, (char*) hostAddress.c_str(), NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
                if(s == 0)
                {
                    printf("Received %ld bytes from %s:%s\n", (long) ret, host, service);
                }
                else
                {
                    fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
                }
            }
            IDEBUG() << " read " << ret << " from " << hostAddress << "," << port;
            // -- debug --
            break;
        }
        case UdpSocket::MulticastSender:
        {
            throw BaseLib::Exception("UdpSocket::readDatagram(): UdpID=MulticastSender: UdpSocket is initialized as sender not receiver!");
            ret = -1;
            break;
        }
        default:
        {
            throw BaseLib::Exception("ERROR! Could not identify UdpID!");
            break;
        }
    }

    return ret;
}

int UdpSocket::WriteDatagram(const char* bytes, const int& length, const std::string& hostAddress, const int& port)
{
    /*** get UDPServer1 Host information, NAME and INET ADDRESS ***/
    //if( (hp = gethostbyname(argv[1])) == NULL )
    //{
    //	addr.sin_addr.s_addr = inet_addr(hostAddress.c_str());
    //		hp = gethostbyaddr((char *)&addr.sin_addr.s_addr, sizeof(addr.sin_addr.s_addr),AF_INET);
    //	}
    //	printf("----UDPServer1 running at host NAME: %s\n", hp->h_name);
    //	bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
    //	printf("(UDPServer1 INET ADDRESS is: %s )\n", inet_ntoa(server.sin_addr));

    int ret = 0;
    switch(this->udpID)
    {
        case UdpSocket::MulticastSender:
        {
            ret = NetworkFunctions::my_sendto(socketDescriptor_, bytes, length, addr_listener, sizeof(addr_listener));
            break;
        }
        case UdpSocket::UnicastReceiver:
        case UdpSocket::BroadcastReceiver:
            IWARNING() << "UdpSocket is initialized as receiver! Writing anyway!";
        case UdpSocket::BroadcastSender:
        case UdpSocket::UnicastSender:
        {
            struct addrinfo* result, * rp;

            // Resolve hostAddress
            struct addrinfo hints;
            memset(&hints, 0, sizeof(struct addrinfo));
            hints.ai_family   = AF_UNSPEC;    // Allow IPv4 or IPv6
            hints.ai_socktype = SOCK_DGRAM; // Datagram socket
            hints.ai_flags    = 0;
            hints.ai_protocol = 0;          // Any protocol

            std::stringstream portStr;
            portStr << port;

            int s = getaddrinfo(hostAddress.c_str(), portStr.str().c_str(), &hints, &result);
            if(s != 0)
            {
                iDebug("getaddrinfo: %s\n", gai_strerror(s));
                throw BaseLib::Exception(string("Could not find the specified host." + hostAddress));
            }
            freeaddrinfo(result);           // No longer needed

            struct sockaddr_in peer_addr;
            memcpy(&peer_addr, (struct sockaddr_in*) result[0].ai_addr, sizeof(struct sockaddr_in));
            socklen_t peer_addr_length = sizeof(peer_addr);

            ret = NetworkFunctions::my_sendto(socketDescriptor_, bytes, length, peer_addr, peer_addr_length);
            break;
        }
        case UdpSocket::MulticastReceiver:
        {
            throw BaseLib::Exception("UdpSocket::writeDatagram(): UdpID=MulticastSender:UdpSocket is initialized as receiver not sender!");
            ret = -1;
            break;
        }
        default:
        {
            throw BaseLib::Exception("ERROR! Could not identify UdpID!");
            break;
        }
    }

    if(ret <= -1)
        IWARNING() << "ERROR! Could not write to socket! ret: " << ret;
    return ret;
}

long long UdpSocket::pendingDatagramSize() const
{
    return (long long) bytesAvailable();
}

bool UdpSocket::hasPendingDatagrams() const
{
    return pendingDatagramSize() > 0;
}

bool UdpSocket::bindReceiver(const string& hostName, int& port)
{
    MutexLocker mutexLocker(&mutexLock_);

    if(isValid())
    {
        IWARNING() << "Socket already opened!";
        return true;
    }

    try
    {
        switch(this->udpID)
        {
            case UdpSocket::UnicastReceiver:
            {
                socketDescriptor_ = NetworkFunctions::bindUdpUnicastReceiver(port);
                break;
            }
            case UdpSocket::MulticastReceiver:
            {
                socketDescriptor_ = NetworkFunctions::bindIPMulticastReceiver(addr_listener, hostName.c_str(), port, localAddress);
                break;
            }
            case UdpSocket::BroadcastReceiver:
            {
                socketDescriptor_ = NetworkFunctions::bindUdpBroadcastReceiver(port);
                break;
            }
            default:
            {
                throw BaseLib::Exception("ERROR! Could not identify UdpID!");
                break;
            }
        }

        if(socketDescriptor_ == -1)
        {
            throw BaseLib::Exception("Failed to open socket!");
        }

        hostName_   = hostName;
        portNumber_ = port;
    }
    catch(BaseLib::Exception ex)
    {
        IWARNING() << " " << ex.what();
        return false;
    }
    catch(const char* msg)
    {
        IWARNING() << " " << msg;
        return false;
    }
    catch(...)
    {
        IWARNING() << " Failed to open socket!";
        return false;
    }

    return true;
}

// TODO: If I want to use read/write on the same socket then sender has to bind to a port as well!
bool UdpSocket::bindSender(const string& hostName, const int& port)
{
    MutexLocker mutexLocker(&mutexLock_);

    if(isValid())
    {
        IWARNING() << "WARNING! Socket already opened!";
        return true;
    }

    try
    {
        switch(this->udpID)
        {
            case UdpSocket::UnicastSender:
            {
                socketDescriptor_ = NetworkFunctions::bindUdpUnicastSender(addr_listener, hostName.c_str(), port, localAddress);
                break;
            }
            case UdpSocket::MulticastSender:
            {
                socketDescriptor_ = NetworkFunctions::bindIPMulticastSender(addr_listener, hostName.c_str(), port, localAddress, 1);
                break;
            }
            case UdpSocket::BroadcastSender:
            {
                socketDescriptor_ = NetworkFunctions::bindUdpBroadcastSender(addr_listener, port, localAddress);
                break;
            }
            default:
            {
                throw BaseLib::Exception("ERROR! Could not identify UdpID!");
                break;
            }
        }

        if(socketDescriptor_ == -1)
        {
            throw BaseLib::Exception("Failed to open socket!");
        }

        hostName_   = hostName;
        portNumber_ = port;
    }
    catch(BaseLib::Exception ex)
    {
        IWARNING() << " " << ex.what();
        return false;
    }
    catch(const char* msg)
    {
        IWARNING() << " " << msg;
        return false;
    }
    catch(...)
    {
        IWARNING() << " Failed to open socket!";
        return false;
    }

    return true;
}


bool UdpSocket::isValid()
{
    return socketDescriptor_ > -1;
}


}; // namespaces end ..



