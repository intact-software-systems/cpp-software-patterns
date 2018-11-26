#include"NetworkLib/Utility/UtilityFunctions.h"
#include"NetworkLib/IncludeExtLibs.h"

#ifdef USE_GCC
#include<unistd.h>
#include<sys/socket.h>
#include<sys/utsname.h>
//#include<netinet/tcp.h>
//#include<ifaddrs.h>
//#include<net/if.h>
//#include<net/if_arp.h>
//#include<arpa/inet.h>
#include<fcntl.h>
#elif WIN32
#include<io.h>
#endif

//#include <getopt.h>
//#include<stdlib.h>
//#include<cstdlib>
#include<iostream>
#include<algorithm>

namespace NetworkLib
{

/*-----------------------------------------------------------------------
    local/private function to this file
------------------------------------------------------------------------- */
void UtilityFunctions::ShutdownSocket(int socket)
{
#ifdef WIN32

#ifdef SD_SEND
    ::shutdown(socket, SD_SEND);
#else
     ::shutdown(socket, 1);
#endif

#else
    ::shutdown(socket, SHUT_WR);
#endif
}

void UtilityFunctions::CloseSocket(int socket)
{
#ifdef USE_GCC
	::close(socket);
#else
    ::closesocket(socket);
#endif
}

void UtilityFunctions::MicroSleep(int64 microsec)
{
#ifdef USE_GCC
	::usleep(microsec);
#else
    int ms = std::max(1, int(microsec/1000));
    ::Sleep(ms);
#endif
}

std::string UtilityFunctions::MakeRandomTempFile()
{
#ifdef USE_GCC
    char filename[40];
    sprintf(filename,"temp-XXXXXX");
    int fd = mkstemp(filename);
    ASSERT(fd != -1);
    CloseSocket(fd);
    return std::string(filename);
#else
    std::stringstream filename;
    filename << "temp-" << BaseLib::Thread::currentThreadId() << "-" << BaseLib::Utility::GetCurrentTimeMs();
    return filename.str();
#endif
}

/**
 * @brief UtilityFunctions::GetAddress
 * @param addr
 * @return
 */
std::string UtilityFunctions::GetAddress(const sockaddr *addr)
{
    std::string address;

    ASSERT(addr);
    if(addr == NULL)
    {
        ICRITICAL() << "Nullpointer given as paramter!";
        return address;
    }

    if(addr->sa_family == AF_INET)
    {
        struct sockaddr_in *ip = (struct sockaddr_in *) addr;

        if(ip)
        {
            char addr_as_string[INET_ADDRSTRLEN+1];

            if(inet_ntop (AF_INET, &ip->sin_addr, addr_as_string, INET_ADDRSTRLEN) != NULL)
            {
                address = std::string(addr_as_string);
            }
        }
        else
        {
            IWARNING() << "Could not cast to sockaddr_in!";
        }
    }
    else if(addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *ip = (struct sockaddr_in6 *) addr;

        if(ip)
        {
            char addr_as_string[INET6_ADDRSTRLEN+1];

            if(inet_ntop (AF_INET6, &ip->sin6_addr, addr_as_string, INET6_ADDRSTRLEN) != NULL)
            {
                address = std::string(addr_as_string);
            }
        }
        else
        {
            IWARNING() << "Could not cast to sockaddr_in6!";
        }
    }

    if(address.empty())
    {
        ICRITICAL() << "Could not convert address family: " << addr->sa_family;
    }

    return address;
}

/**
 * @brief UtilityFunctions::GetPortNtohs
 * @param addr
 * @return
 */
uint16 UtilityFunctions::GetPortNtohs(const sockaddr *addr)
{
    ASSERT(addr);
    if(addr == NULL)
    {
        ICRITICAL() << "Nullpointer given as paramter!";
        return 0;
    }

    if(addr->sa_family == AF_INET)
    {
        struct sockaddr_in *ip = (struct sockaddr_in *) addr;
        return ntohs(ip->sin_port);
    }
    else if(addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *ip = (struct sockaddr_in6 *) addr;
        return ntohs(ip->sin6_port);
    }

    ICRITICAL() << "Could not convert port number!";

    return 0;
}

int UtilityFunctions::SetSocketNonBlocking(int sock)
{
#ifdef USE_GCC
    int flags = fcntl(sock, F_GETFL, 0);
    flags |= O_NONBLOCK;
	return ::fcntl(sock, F_SETFL, flags);
#else
    int64 nonBlockingMode = 1;
    return ::ioctlsocket(sock, FIONBIO, &nonBlockingMode);
#endif
}

int UtilityFunctions::SetSocketBlocking(int sock)
{
#ifdef USE_GCC
    int flags = fcntl(sock, F_GETFL, 0);
    flags |= ~O_NONBLOCK;
    return ::fcntl(sock, F_SETFL, flags);
#else
    int64 nonBlockingMode = 0;
    return ::ioctlsocket(sock, FIONBIO, &nonBlockingMode);
#endif
}

bool UtilityFunctions::IsValidSocket(int socket)
{
#ifdef WIN32
    //int ret = send(socket, 0, 0, 0);
    int ret = _write(socket, 0, 0);
    if(ret == -1) return false;

    return true;

#elif USE_GCC
    int length = 1;
	char buf;
    int retval = ::recv(socket, &buf, length, MSG_PEEK | MSG_DONTWAIT);
	if(retval == -1)
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
                    IDEBUG() << " bad file descriptor " << socket << "err: " << strerror(errno);
					return false;
					break;
				}
			case EIO:
			case EINTR:
			case EAGAIN:
				break;
			default:
				IDEBUG() << " read undefined error : " << strerror(errno);
				break;
		}
	}
	else if(retval == 0)
	{
		IDEBUG() << " peer reset descriptor " << socket << " err: " << strerror(errno);
		return false;
	}
#endif

    return true;
}

bool UtilityFunctions::IsValidWritePipe(int pipe)
{
#ifdef WIN32
    int ret = _write(pipe, 0, 0);
    if(ret == -1) return false;

    return true;

#elif USE_GCC
    int ret = write(pipe, 0, 0);
    if(ret == -1)
    {
        IDEBUG() << " bad file descriptor " << pipe << "err: " << strerror(errno);
        return false;
    }
    return true;
#endif
}

bool UtilityFunctions::IsValidReadPipe(int pipe)
{
#ifdef WIN32
    int ret = _read(pipe, 0, 0);
    if(ret == -1) return false;

    return true;

#elif USE_GCC
    int ret = read(pipe, 0, 0);
    if(ret == -1)
    {
        IDEBUG() << " bad file descriptor " << pipe << "err: " << strerror(errno);
        return false;
    }
    return true;
#endif
}

/*int PASCAL FAR getsockopt (
                           __in SOCKET s,
                           __in int level,
                           __in int optname,
                           __out_bcount(*optlen) char FAR * optval,
                           __inout int FAR *optlen);



int UtilityFunctions::GetSockOpt(int socketDescriptor, int level, int optname, char *optval, socklen_t *len)
{
    int ret = getsockopt(socketDescriptor_, SOL_SOCKET, SO_TYPE, (char*)&type, &len);
}*/

} // namespace NetworkLib
