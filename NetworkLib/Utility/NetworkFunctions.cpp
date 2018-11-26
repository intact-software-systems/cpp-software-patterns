/***************************************************************************
                          NetworkFunctions.cpp  -  description
                             -------------------
    begin                : Wed Mar 21 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#include"NetworkLib/Utility/NetworkFunctions.h"
#include"NetworkLib/Utility/UtilityFunctions.h"
#include"NetworkLib/Address/NetworkInterface.h"

#include<iostream>
#include<algorithm>
#include<errno.h>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<cstdlib>

#ifdef USE_GCC
#include<sys/utsname.h>
#include<sys/ioctl.h>
#include<netinet/tcp.h>
#include<ifaddrs.h>
#include<net/if.h>
#include<net/if_arp.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/uio.h>
#else //if WIN32
//#include<WinError.h>
//#include<Ws2tcpip.h>
//#include<WSPiApi.h>
#include<sys/types.h>
#include<sys/utime.h>
#include<sys/timeb.h>
//#include<Ipexport.h>
//#include<IPHlpApi.h>
//#include<sys/socket.h>
//#undef UNICODE
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdio.h>
// link with Ws2_32.lib
//#pragma comment(lib, "Ws2_32.lib")
//#pragma comment(lib, "Ws2_32.lib")
#endif

using namespace BaseLib;
using namespace std;

namespace NetworkLib { namespace NetworkFunctions
{

void DIE(const char *msg)
{
    IDEBUG() << " " ; perror(msg);
    char m[1024];
    sprintf(m, "%s %s", msg, strerror(errno));

    throw BaseLib::Exception(m);
}

/*void DIE(std::string msg)
{
    IDEBUG() << " " ; perror(msg.c_str());
    char m[1024];
    sprintf(m, "%s %s", msg.c_str(), strerror(errno));

    throw BaseLib::Exception(m);
}*/

void ByteSwap(unsigned char * b, int n)
{
   int i = 0;
   int j = n-1;
   while (i<j)
   {
      std::swap(b[i], b[j]);
      i++, j--;
   }
}

/*-----------------------------------------------------------------------
                    receiver/connect
------------------------------------------------------------------------- */
#ifdef USE_GCC
int receiver_wscan(int &port, long val)
{
    int sock = -1, err = -1;
    port = -1;
    sockaddr_in addr;

    //port = (int) (max * rand() / (RAND_MAX + 1.0));
    for(port = 8180; port < 65000; port++)
    {
        sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        if(sock < 0) DIE("Can't establish socket");

        int True = 1;
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&True, sizeof(int)) == -1)
        {
            UtilityFunctions::CloseSocket(sock);
            DIE(" can't setsockopt: SO_REUSEADDR");
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        err = ::bind(sock, (struct sockaddr*)&addr, sizeof(addr));
        if(err < 0)
        {
            //cerr << "Can't bind TCP socket to port" << strerror(errno) << endl;
            UtilityFunctions::CloseSocket(sock);
            continue;
        }
        else break;
    }
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Can't bind TCP socket to port");
    }

    // listen
    err = ::listen(sock, SOMAXCONN);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Can't listen to TCP socket");
    }

    // non blocking
    //long val = O_NONBLOCK;
    if(val >= 0)
    {
        err = fcntl(sock, F_SETFL, val);
        if(err < 0)
        {
            UtilityFunctions::CloseSocket(sock);
            DIE("Can't set listen socket to val");
        }
    }

    return sock;
}
#endif

int tcp_port(int &port)
{
    int sock = -1, err = -1;
    port = -1;
    sockaddr_in addr;
    for(port = 7000 ; port < 65000; port++)
    {
        sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock < 0) throw("Can't establish socket");

        int True = 1;
        if(::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &True, sizeof(True)) == -1)
        {
            UtilityFunctions::CloseSocket(sock);
            throw(" can't setsockopt: SO_REUSEADDR");
        }
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        err = ::bind(sock, (const sockaddr*)&addr, sizeof(addr));
        if(err < 0)
        {
            //cerr << "Can't bind TCP socket to port" << strerror(errno) << endl;
            UtilityFunctions::CloseSocket(sock);
            continue;
        }
        else break;
    }
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        throw("Can't bind TCP socket to port");
    }

    // listen
    err = ::listen(sock, SOMAXCONN);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        throw("Can't listen to TCP socket");
    }

    // non blocking
    err = UtilityFunctions::SetSocketNonBlocking(sock);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        throw("Can't set listen socket to non-blocking");
    }

    return sock;
}

int udp_port(int &port)
{
    int sock = -1, err = -1;
    port = -1;
    sockaddr_in addr;
    for(port = 6000 ; port < 65000; port++)
    {
        sock = ::socket(PF_INET, SOCK_DGRAM, 0);
        if(sock < 0) throw("Can't establish socket");

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);
        err = ::bind(sock, (const sockaddr*)&addr, sizeof(addr));
        if(err < 0)
        {
            //cerr << "Can't bind UDP socket to port" << strerror(errno) << endl;
            UtilityFunctions::CloseSocket(sock);
            continue;
        }
        else break;
    }
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        throw("Can't bind UDP socket to port");
    }

    return sock;
}
/*-----------------------------------------------------------------------
                    receiver/connect
------------------------------------------------------------------------- */
int receiver(int port)
{
    int sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock < 0) DIE("Can't establish socket");

    int True = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &True, sizeof(int)) == -1)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE(" can't setsockopt: SO_REUSEADDR");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    // bind
    int err = ::bind(sock, (const sockaddr*)&addr, sizeof(addr));
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Can't bind TCP socket to port");
    }

    // listen
    err = ::listen(sock, SOMAXCONN);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Can't listen to TCP socket");
    }

    // non blocking
    err = UtilityFunctions::SetSocketNonBlocking(sock);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Can't set listen socket to non-blocking");
    }

    return sock;
}

/*int bindListen(int &port)
{
    int socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socket == -1)
        DIE("Could not create a socket.");

    int flags = fcntl(socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    if(fcntl(socket, F_SETFL, flags)==-1)
    {
        UtilityFunctions::CloseSocket(socket);
        DIE("Could not set the socket to non-blocking mode");
    }
    fcntl(socket, F_SETFD, FD_CLOEXEC);

    int reuseAddressOption = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddressOption, sizeof(reuseAddressOption)))
    {
        UtilityFunctions::CloseSocket(socket);
        DIE("Could not set SO_REUSEADDR");
    }

    sockaddr_in addr;

    // -- debug start--
    int err = -1;
    if(port <= 0)
    {
        for(port = 1000 ; port < 65000; port++)
        {
            //sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            //if(sock < 0) throw("Can't establish socket");

            //int True = 1;
            //if(::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &True, sizeof(True)) == -1)
            //{
            //	UtilityFunctions::CloseSocket(sock);
            //	throw(" can't setsockopt: SO_REUSEADDR");
            //}
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(port);
            err = ::bind(socket, (struct sockaddr*)&addr, sizeof(addr));
            if(err < 0)
            {
                //cerr << "Can't bind TCP socket to port" << strerror(errno) << endl;
                continue;
            }
            //else
            //{
            //	IDEBUG() << " Bound to port " << port << endl;
            //	break;
            //}

            err = ::listen(socket, SOMAXCONN);
            if(err == -1)
            {
                //UtilityFunctions::CloseSocket(socket);
                //DIE("Could not start listening for connections.");
                IDEBUG() << " Can't Listen to port " << port << " " << strerror(errno);
            }
            else
            {
                IDEBUG() << " Listening to port " << port << endl;
                break;
            }
        }

        if(err < 0)
        {
            UtilityFunctions::CloseSocket(socket);
            throw BaseLib::Exception("Can't bind TCP socket to port");
        }
    }
    else
    // -- debug end --
    {
        memset(&addr, 0, sizeof(addr));
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);

        int err = bind(socket, (sockaddr*)&addr, sizeof(sockaddr_in));
        if(err == -1)
        {
            std::stringstream ss;
            //ss << "Socket server on " << GetLocalHostName() << " could not bind to the local port (port number " << port << ")";
            ss << PRETTY_FUNCTION << " Socket bind error: "  << strerror(errno) << endl;
            UtilityFunctions::CloseSocket(socket);
            DIE(ss.str().c_str());
        }

        err = ::listen(socket, SOMAXCONN);
        if(err == -1)
        {
            UtilityFunctions::CloseSocket(socket);
            DIE("Could not start listening for connections.");
        }
    }

    //err = ::listen(socket, SOMAXCONN);
    //if(err == -1)
    //{
    //	UtilityFunctions::CloseSocket(socket);
    //	DIE("Could not start listening for connections.");
    //}

    //socklen_t addr_length = sizeof(addr);
    //getsockname(socket, (sockaddr*)&addr, &addr_length);
    //serverPort_ = ntohs(addr.sin_port);

    return socket;
}*/

int bindListen(int &port)
{
    int sock = ::socket(AF_INET, SOCK_STREAM, 0); // defaults to IPPROTO_TCP
    if(sock == -1)
        DIE("Could not create a socket.");

    int err = UtilityFunctions::SetSocketNonBlocking(sock);
    if(err < 0)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Could not set the socket to non-blocking mode");
    }

#ifdef USE_GCC
    fcntl(sock, F_SETFD, FD_CLOEXEC);
#endif

    int reuseAddressOption = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddressOption, sizeof(reuseAddressOption)))
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Could not set SO_REUSEADDR");
    }

    if(port < 0 || port > 65638) // if port == 0 port is assigned by OS
        port = 0;

    sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    err = ::bind(sock, (const sockaddr*)&addr, sizeof(sockaddr_in));
    if(err == -1)
    {
        std::stringstream ss;
        //ss << "Socket server on " << GetLocalHostName() << " could not bind to the local port (port number " << port << ")";
        ss << TSPRETTY_FUNCTION << " Socket bind error: "  << strerror(errno) << endl;
        UtilityFunctions::CloseSocket(sock);
        DIE(ss.str().c_str());
    }

    err = ::listen(sock, SOMAXCONN);
    if(err == -1)
    {
        UtilityFunctions::CloseSocket(sock);
        DIE("Could not start listening for connections.");
    }

    socklen_t addr_length = (int) sizeof(addr);
    getsockname(sock, (sockaddr*)&addr, &addr_length);
    port = ntohs(addr.sin_port);

    return sock;
}

int bindIPMulticastReceiver(struct sockaddr_in &addr_listener, string hostName, int port, string localAddress)
{
    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(socket < 0) throw ("Invalid Socket");

    memset(&addr_listener, 0, sizeof(addr_listener));
    addr_listener.sin_family = AF_INET;
    addr_listener.sin_port = htons(port);
    addr_listener.sin_addr.s_addr = htonl(INADDR_ANY);

    if(::bind(socket, (const sockaddr*)&addr_listener, sizeof(addr_listener)) < 0)
    {
        UtilityFunctions::CloseSocket(socket);
        throw ("Error in bind");
    }

    struct ip_mreq imr;
    imr.imr_multiaddr.s_addr = inet_addr(hostName.c_str());
    imr.imr_interface.s_addr = htonl(INADDR_ANY);

    if(!localAddress.empty())
        imr.imr_interface.s_addr = inet_addr(localAddress.c_str());

    int nret = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*) &imr, sizeof(imr));
    if(nret < 0)
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Error - setsockopt IP_ADD_MEMBERSHIP");
    }

    unsigned int yes = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR,(const char*) &yes, sizeof(yes)))
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set SO_REUSEADDR");
    }
    IDEBUG() << hostName << " " << port << " socket " << socket;
    return socket;
}

int bindIPMulticastSender(struct sockaddr_in &addr_sender, string hostName, int port, string localAddress, int timeToLive)
{
    int socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if(socket < 0) throw ("Invalid Socket");

    int reuseAddressOption = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddressOption, sizeof(reuseAddressOption)))
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set SO_REUSEADDR");
    }

    memset(&addr_sender, 0, sizeof(addr_sender));
    addr_sender.sin_family = AF_INET;
    addr_sender.sin_port = htons(port);
    addr_sender.sin_addr.s_addr = inet_addr(hostName.c_str());

    if(!localAddress.empty())
        bindSocketToLocalAddress(socket, localAddress, port);

    if(timeToLive > 1)
        setSocketOptionTTL(socket, timeToLive);

    unsigned int yes = 1;
    if(setsockopt(socket, IPPROTO_IP, IP_MULTICAST_LOOP,(const char*) &yes, sizeof(yes)))
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set IP_MULTICAST_LOOP");
    }

    return socket;
}

int bindUdpUnicastReceiver(int &port)
{
    int socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket == -1)
        throw BaseLib::Exception("UdpReceiver::OpenUnicast - Could not create a socket.");

    int reuseAddressOption = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseAddressOption, sizeof(reuseAddressOption)))
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set SO_REUSEADDR");
    }

    struct sockaddr_in addr_listener;
    memset(&addr_listener, 0, sizeof(addr_listener));
    addr_listener.sin_family = AF_INET;
    if(port < 0)	// port is assigned by OS
        addr_listener.sin_port = htons(0);
    else
        addr_listener.sin_port = htons(port);
    addr_listener.sin_addr.s_addr = htonl(INADDR_ANY);

    if(::bind(socket, (const sockaddr*)&addr_listener, sizeof(addr_listener)) < 0)
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Error in bind");
    }

    if(port <= 0)
    {
        socklen_t length = (int) sizeof(addr_listener);
        int ret = getsockname(socket, (sockaddr*)&addr_listener, &length);
        if(ret < 0)
        {
            UtilityFunctions::CloseSocket(socket);
            throw BaseLib::Exception("Error in getsockname");
        }
        iDebug("Server Port is: %d\n", ntohs(addr_listener.sin_port));
        port = addr_listener.sin_port;
    }

    return socket;
}

int bindUdpUnicastSender(struct sockaddr_in &addr_listener, string hostName, int port, string localAddress)
{
    int socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket == -1)
        throw ("UdpReceiver::OpenUnicast - Could not create a socket.");

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    stringstream portString;
    portString << port;

    addrinfo *ai = 0;
    int status = getaddrinfo(hostName.c_str(), portString.str().c_str(), &hints, &ai);
    if(status)
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception(string("Could not find the specified host." + hostName));
    }

    sockaddr_in *addr_sender = 0;
    addr_sender = (sockaddr_in*)ai[0].ai_addr;
    addr_listener = *addr_sender;

    freeaddrinfo(ai);

    if(!localAddress.empty())
        bindSocketToLocalAddress(socket, localAddress, port);

    return socket;
}

/*int bindUdpUnicast(struct sockaddr_in &addr_listener, const int &port)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0; // any protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    addrinfo *result = 0;
    int status = getaddrinfo(NULL, (char*)port, &hints, &result);
    if(status)
    {
        throw BaseLib::Exception(string("Could not getaddrinfo for port." + port));
    }

    int socket = -1;
    struct addrinfo *rp = NULL;
    for(rp = result; rp != NULL; rp = rp->ai_next)
    {
        socket = ::socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(socket == -1)
            continue;

        if(bind(socket, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  // Success

        UtilityFunctions::CloseSocket(socket);
    }

    if(rp == NULL)
    {               // No address succeeded
        throw BaseLib::Exception("Could not find any valid address!");
    }

    sockaddr_in *addr_sender = 0;
    addr_sender = (struct sockaddr_in*)rp->ai_addr;
    addr_listener = *addr_sender;

    freeaddrinfo(hints);

    return socket;
}*/


int bindUdpBroadcastReceiver(int &port)
{
    return bindUdpUnicastReceiver(port);
}

int bindUdpBroadcastSender(struct sockaddr_in &addr, int port, string localAddress)
{
    int socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socket == -1)
        throw BaseLib::Exception("UdpSender::OpenBroadcast - Could not create a socket.");

    int reuseAddressOption = 1;
    if(setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuseAddressOption, sizeof(reuseAddressOption)))
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set SO_REUSEADDR");
    }

    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    int optval = 1; // "true"
    int status = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (const char*) &optval, sizeof(optval));
    if(status != 0)
    {
        UtilityFunctions::CloseSocket(socket);
        throw BaseLib::Exception("Could not set broadcast option.");
    }

    if(!localAddress.empty())
        bindSocketToLocalAddress(socket, localAddress, port);

    return socket;
}

void resolveIP(char *name, const char *ip)
{
    std::string filename = UtilityFunctions::MakeRandomTempFile();

    char command[100];
    sprintf(command, "resolveip %s > %s", ip, filename.c_str());
    int ret = system(command);

    ifstream myfile;
    myfile.open(filename.c_str());
    if(myfile.is_open() == false)
    {
        IDEBUG() << " could not execute " << command;
        remove(filename.c_str());
        return;
    }

    ASSERT(myfile.is_open());

    char *token = NULL;
    while(!myfile.eof())
    {
        string line;
        getline(myfile, line);

        char *buffer = (char*)line.c_str();
        token = STRTOK_S((char*)line.c_str(), " ", &buffer); // Host
        if(token == NULL) break;
        else if(strncmp(token, "resolveip", 9) == 0) break;
        else if(strncmp(token, "Broadcast", 9) == 0) break;

        token = STRTOK_S(NULL, " ", &buffer); // name
        token = STRTOK_S(NULL, " ", &buffer); // of
        token = STRTOK_S(NULL, " ", &buffer); // ip/name
        token = STRTOK_S(NULL, " ", &buffer); // is
        token = STRTOK_S(NULL, " ", &buffer); // name
        break;
    }

    if(token == NULL)
        strcpy(name, "unsolved");
    else
        strcpy(name, token);

    remove(filename.c_str());
}

double my_ping(const char *addr, int num)
{
    std::string filename = UtilityFunctions::MakeRandomTempFile();

    char command[200];
    sprintf(command, "ping -W 3 -c %i %s > %s", num, addr, filename.c_str());
    //printf("%s\n", command);

    int ret = system(command);

    FILE *myfile = fopen(filename.c_str(), "r");
    if(myfile == NULL)
    {
        printf("WARNING! Could not find file %s\n", filename.c_str());
        remove(filename.c_str());
        return 0;
    }

    double min_time = 0, avg_time_diff = 0, max_time = 0, mdev_time = 0;
    size_t len = 0;
    char *token = NULL, *line = NULL;
    //while(getline(&line, &len, myfile) != -1)
    while(fgets(line, sizeof(line), myfile))
    {
        //printf(" line %s\n", line);
        token = STRTOK_S(line, " ", &line);
        //printf(" token: %s\n",token);
        if(token == NULL) break;

        if(strcmp(token, "rtt") == 0)
        {
            token = STRTOK_S(NULL, " ", &line); // min/avg/max/mdev
            token = STRTOK_S(NULL, " ", &line); // =

            // collect ping stats
            token = STRTOK_S(NULL, "/", &line); // min
            min_time = atof(token);

            token = STRTOK_S(NULL, "/", &line); // avg
            avg_time_diff = atof(token);

            token = STRTOK_S(NULL, "/", &line); // max
            max_time = atof(token);

            token = STRTOK_S(NULL, " ", &line); // mdev
            mdev_time = atof(token);
            break;
        }
    }

    //pclose(myfile);
    ::fclose(myfile);
    remove(filename.c_str());

    return min_time;
}

/*-----------------------------------------------------------------------
                    Interface functions
------------------------------------------------------------------------- */
bool isPrivateIP(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        struct sockaddr_in *sa4 = (struct sockaddr_in *) sa;

        // 10.x.x.x/8
        if (sa4->sin_addr.s_addr >> 24 == 0x0A)
            return true;

        // 172.16.0.0 - 172.31.255.255 = 172.16.0.0/10
        if (sa4->sin_addr.s_addr >> 20 == 0xAC1)
            return true;

        // 192.168.x.x/16
        if (sa4->sin_addr.s_addr >> 16 == 0xC0A8)
            return true;

        // 169.254.x.x/16  (for APIPA)
        if (sa4->sin_addr.s_addr >> 16 == 0xA9FE)
            return true;
    }

    return false;
}

std::list<std::string> getLocalInterfaceAddresses(void)
{
    std::list<std::string> interfaces;

#ifdef USE_GCC
    struct ifaddrs *ifa, *results;

    if(getifaddrs(&results) < 0)
    {
        IDEBUG() << "getifaddrs returned error!" << strerror(errno);
        return interfaces;
    }

    // Loop and get each interface the system has, one by one...
    for(ifa = results; ifa; ifa = ifa->ifa_next)
    {
        // no ip address from interface that is down
        if((ifa->ifa_flags & IFF_UP) == 0)
            continue;

        if(ifa->ifa_addr == NULL)
            continue;


        if(ifa->ifa_addr->sa_family == AF_INET)
        {
            NetworkInterface networkInterface;

            // Interface
            {
                IDEBUG() << "Interface: " << ifa->ifa_name;
                interfaces.push_front(string(ifa->ifa_name));

            }

            NetworkAddressEntry networkAddress;

            // IP
            {
                struct sockaddr_in *sa4 = (struct sockaddr_in *) ifa->ifa_addr;

                std::string hostaddress = inet_ntoa(sa4->sin_addr);

                IDEBUG() << "IP " << hostaddress;
            }

            // Netmask
            {
                struct sockaddr_in *netmask = (struct sockaddr_in *) ifa->ifa_netmask;

                IDEBUG() << "Netmask: " << inet_ntoa(netmask->sin_addr);
            }

            // Broadcast or P2P address
            {
                struct sockaddr_in *broadcast = (struct sockaddr_in *) ifa->ifa_dstaddr;

                IDEBUG() << "Broadcast: " << inet_ntoa(broadcast->sin_addr);
            }
        }
        else if(ifa->ifa_addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;

            interfaces.push_front(string(ifa->ifa_name));
        }
    }

    freeifaddrs(results);
#elif WIN32
    /*
     Check out: http://msdn.microsoft.com/en-us/library/aa365915.aspx
      */
    ULONG               ulOutBufLen;
    DWORD               dwRetVal;
    unsigned int       i;

    IP_INTERFACE_INFO*  pInterfaceInfo;

    pInterfaceInfo = (IP_INTERFACE_INFO *) malloc(sizeof (IP_INTERFACE_INFO));
    ulOutBufLen = sizeof(IP_INTERFACE_INFO);

    if(GetInterfaceInfo(pInterfaceInfo, &ulOutBufLen) == ERROR_INSUFFICIENT_BUFFER)
    {
        free(pInterfaceInfo);
        pInterfaceInfo = (IP_INTERFACE_INFO *) malloc(ulOutBufLen);
    }

    if((dwRetVal = GetInterfaceInfo(pInterfaceInfo, &ulOutBufLen)) != NO_ERROR)
    {
        IDEBUG() << "GetInterfaceInfo failed with error: " << dwRetVal;
    }

    //iDebug("GetInterfaceInfo succeeded");
    //iDebug("Num Adapters: %u\n", pInterfaceInfo->NumAdapters);

    for(i = 0; i < (unsigned int) pInterfaceInfo->NumAdapters; i++)
    {
        //iDebug("Adapter Index[%d]: %u\n", i, pInterfaceInfo->Adapter[i].Index);
        //iDebug("Adapter Name[%d]:  %ws\n\n", i, pInterfaceInfo->Adapter[i].Name);

        char name[sizeof(pInterfaceInfo->Adapter[i].Name) + 1];

        sprintf(name, "%ws", pInterfaceInfo->Adapter[i].Name);

        //IDEBUG() << "Interface name " << name;

        interfaces.push_front(name);
    }

    if(pInterfaceInfo)
    {
        free(pInterfaceInfo);
        pInterfaceInfo = NULL;
    }

#endif
    return interfaces;
}

std::list<std::string> getLocalIPs(bool include_loopback)
{
    std::list<std::string> ips, loopbacks;

#ifdef USE_GCC
    struct ifaddrs *ifa, *results;

    if(getifaddrs (&results) < 0)
    {
        IDEBUG() << "getifaddrs returned error! " << strerror(errno);
        return ips;
    }

    // Loop through the interface list and get the IP address of each IF
    for(ifa = results; ifa; ifa = ifa->ifa_next)
    {
        char addr_as_string[INET6_ADDRSTRLEN+1];

        // no ip address from interface that is down
        if((ifa->ifa_flags & IFF_UP) == 0)
            continue;

        if(ifa->ifa_addr == NULL)
        {
            continue;
        }
        else if(ifa->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *sa4 = (struct sockaddr_in *) ifa->ifa_addr;

            if(inet_ntop (AF_INET, &sa4->sin_addr, addr_as_string, INET6_ADDRSTRLEN) == NULL)
                continue;
        }
        else if(ifa->ifa_addr->sa_family == AF_INET6)
        {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)ifa->ifa_addr;

            // Skip link-local addresses, they require a scope
            if(IN6_IS_ADDR_LINKLOCAL (&sa6->sin6_addr))
                continue;

            if(inet_ntop (AF_INET6, &sa6->sin6_addr, addr_as_string, INET6_ADDRSTRLEN) == NULL)
                continue;
        }
        else
            continue;

        // -- start debug --
        //IDEBUG() << "Interface: " << ifa->ifa_name;
        //IDEBUG() << "IP Address:" << addr_as_string;
        // -- end debug --

        if((ifa->ifa_flags & IFF_LOOPBACK) == IFF_LOOPBACK)
        {
            if (include_loopback)
                loopbacks.push_back(addr_as_string);
            else
                IDEBUG() << "Ignoring loopback interface";
        }
        else
        {
            if(isPrivateIP(ifa->ifa_addr))
                ips.push_back(addr_as_string);
            else
                ips.push_front(addr_as_string);
        }
    }

    freeifaddrs (results);

    if(include_loopback && loopbacks.empty() == false)
        ips.merge(loopbacks);
        //ips = g_list_concat (ips, loopbacks);

#else
    MIB_IPADDRTABLE  *pIPAddrTable;
    DWORD            dwSize = 0;
    DWORD            dwRetVal;

    pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof(MIB_IPADDRTABLE) );

    if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        free( pIPAddrTable );
        pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
    }

    if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) != NO_ERROR )
    {
        iDebug("GetIpAddrTable call failed with %u\n", dwRetVal);
    }
    else
    {
        for(unsigned int i = 0; i < pIPAddrTable->dwNumEntries; i++)
        {
            //iDebug("IP Address:         %ul\n", pIPAddrTable->table[i].dwAddr);
            //iDebug("IP Mask:            %ul\n", pIPAddrTable->table[i].dwMask);
            //iDebug("IF Index:           %ul\n", pIPAddrTable->table[i].dwIndex);
            //iDebug("Broadcast Addr:     %ul\n", pIPAddrTable->table[i].dwBCastAddr);
            //iDebug("Re-assembly size:   %ul\n", pIPAddrTable->table[i].dwReasmSize);


            struct in_addr addr;
            addr.s_addr = (long)pIPAddrTable->table[i].dwAddr;

            //IDEBUG() << "IP Address: " << inet_ntoa(addr);;

            ips.push_back(inet_ntoa(addr));
        }
    }

    if (pIPAddrTable)
            free(pIPAddrTable);

#endif

    return ips;
}


std::string getNodeAddress()
{
    string hostaddress;
    //int ret = getSockAddress(hostaddress);
    //if(ret == -1)
    getNodeAddress(hostaddress);

    return hostaddress;
}

int getSockAddress(string &hostaddress)
{
    int port = 0;
    int sock = tcp_port(port);

    struct sockaddr_in local;
    socklen_t sz = sizeof(local);
    int ret = getsockname(sock, (struct sockaddr*)&local, &sz);
    if(ret < 0) return -1;

    //struct sockaddr_in remote;
    //int client;
    //while(client = ::accept(sock,(struct sockaddr *)&remote,(socklen_t *) sizeof(remote)))
    //{
    //	getsockname(client,(struct sockaddr *)&local,(socklen_t *) sizeof(local));
    //	printf("%s:%d\n",inet_ntoa(local.sin_addr),ntohs(local.sin_port));
    //	UtilityFunctions::CloseSocket(client);
    //}

    hostaddress = inet_ntoa(local.sin_addr);

    // -- debug --
    //IDEBUG() ;
    //fprintf(stderr, " got host name %s, socket %d\n", hostaddress.c_str(), sock);
    // -- end debug --

    UtilityFunctions::CloseSocket(sock);

    return 1;
}

int getSockAddress(int sock, string &hostaddress)
{
    struct sockaddr_in local;
    socklen_t sz = sizeof(local);
    int ret = getsockname(sock, (struct sockaddr*)&local, &sz);
    if(ret < 0) return -1;

    hostaddress = inet_ntoa(local.sin_addr);

    //struct sockaddr_in remote;
    //for(int i = 0; i < 3; i++)
    //{
    //	int client = ::accept(sock,(struct sockaddr *)&remote,(socklen_t *) sizeof(remote));
    //	getsockname(client,(struct sockaddr *)&local,(socklen_t *) sizeof(local));

    //	IDEBUG() ;
    //	printf("%s:%d\n",inet_ntoa(local.sin_addr),ntohs(local.sin_port));
    //	UtilityFunctions::CloseSocket(client);
    //}

    // -- debug --
    //IDEBUG() ;
    //fprintf(stderr, " got host name %s, socket %d\n", hostaddress.c_str(), sock);
    // -- end debug --

    return 1;
}

int getPeerAddress(int sock, string &hostaddress)
{
    struct sockaddr_in local;
    socklen_t sz = sizeof(local);
    int ret = getpeername(sock, (struct sockaddr*)&local, &sz);
    if(ret < 0) return -1;

    hostaddress = inet_ntoa(local.sin_addr);

    // -- debug --
    //IDEBUG() ;
    //fprintf(stderr, " got host name %s, socket %d\n", hostaddress.c_str(), sock);
    // -- end debug --

    return 1;
}

int getPeerPort(int sock, int &port)
{
    struct sockaddr_in local;
    socklen_t sz = sizeof(local);
    int ret = getpeername(sock, (struct sockaddr*)&local, &sz);
    if(ret < 0) return -1;

    port = ntohs(local.sin_port);

    // -- debug --
    //IDEBUG() ;
    //fprintf(stderr, " got host name %s, socket %d\n", hostaddress.c_str(), sock);
    // -- end debug --

    return 1;
}

int getPeerSockAddr(int sock, struct sockaddr_in &local)
{
    socklen_t sz = sizeof(local);
    int ret = getpeername(sock, (struct sockaddr*)&local, &sz);
    if(ret < 0) return -1;

    //IDEBUG() ;
    //fprintf(stderr, " got host name %s, socket %d\n", hostaddress.c_str(), sock);

    return 1;
}

std::string getIpForInterface(const char *interface_name)
{
#ifdef USE_GCC
    struct ifreq ifr;
    struct sockaddr_in *sa;
    int sockfd;

    ifr.ifr_addr.sa_family = AF_INET;
    memset (ifr.ifr_name, 0, sizeof (ifr.ifr_name));
    strncpy (ifr.ifr_name, interface_name, sizeof (ifr.ifr_name));

    if((sockfd = socket (AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0)
    {
        iDebug("Error : Cannot open socket to retreive interface list");
        return string();
    }

    if(ioctl (sockfd, SIOCGIFADDR, &ifr) < 0)
    {
        iDebug("Error : Unable to get IP information for interface %s", interface_name);
        UtilityFunctions::CloseSocket(sockfd);
        return string();
    }

    UtilityFunctions::CloseSocket(sockfd);
    sa = (struct sockaddr_in *) &ifr.ifr_addr;
    iDebug("Address for %s: %s", interface_name, inet_ntoa (sa->sin_addr));

    return string(inet_ntoa (sa->sin_addr));
#else
    IFATAL() << "Not supported on windows!";
    return std::string(interface_name);
#endif
}

bool isIPAddressOnHost(const std::string &hostName, const std::string &ipAddress)
{
    hostent *ent = gethostbyname(hostName.c_str());
    if(ent == NULL)
    {
        IDEBUG() << "ERROR! Failed to gethostbyname " << hostName;
        return false;
    }
    if(strncmp(hostName.c_str(), "localhost", 9) == 0 && strncmp(ipAddress.c_str(), "127.0.0.1", 9) == 0)
        return true;

    struct in_addr addr;
    string hostIP = hostName;

    for(int i = 0; ent->h_addr_list[i] != 0; i++)
    {
        //if(ent->h_addr_list[i] != INADDR_LOOPBACK)
        addr.s_addr = *(u_long*)ent->h_addr_list[i];
        string ip = string(inet_ntoa(addr));

        if(strcmp(ip.c_str(), ipAddress.c_str()) == 0)
        {
            //cout << "HostInformationSpace::IsIPAddressOnHost(hostName, ipAddress): Found host ip in host ent : " << ip << endl;
            return true;
        }
    }

    iCritical() << PRETTY_FUNCTION << "WARNING! " << ipAddress << " was not found on " << hostName;

    return false;
}



/*std::string getIPAddress(const std::string &hostName)
{
#if 1
    IDEBUG() << "WARNING! Outdated function!";

    static Mutex mutex;
    MutexLocker lock(&mutex);

    hostent *ent = gethostbyname(hostName.c_str());
    if(ent == NULL)
    {
        //cerr << "HostInformationSpace::getIPAddress(hostName): ERROR! Failed to gethostbyname " << hostName << endl;
        return hostName;
    }
    if(strncmp(hostName.c_str(), "localhost", 9) == 0)
        return string("127.0.0.1");

    struct in_addr addr;
    string hostIP = hostName;

    for(int i = 0; ent->h_addr_list[i] != 0; i++)
    {
        //if(ent->h_addr_list[i] != INADDR_LOOPBACK)
        addr.s_addr = *(u_long*)ent->h_addr_list[i];
        string ip = string(inet_ntoa(addr));
        hostIP = ip;
        break;
    }

    if(strncmp(hostIP.c_str(), "127.0.0.1", 9) == 0)
    {
#ifdef USE_GCC
        // Linux:
        // /sbin/ifconfig eth0 | grep "inet addr:"
        // Result: inet addr:192.168.0.114  Bcast:192.168.0.255  Mask:255.255.255.0
        stringstream hostNameFile, stream;
        hostNameFile << hostName << rand() << "." << Thread::processId();
        stream << "/sbin/ifconfig eth0 | grep \"inet addr:\" > " << hostNameFile.str() ;
        int ret = system(stream.str().c_str());
        if(ret == -1)
        {
            remove(hostNameFile.str().c_str());
            return hostName;
        }

        char line[4096];
        char *context = NULL;

        ifstream infile(hostNameFile.str().c_str());

        while(!infile.eof())
        {
            infile.getline(line, 4096);
            if(line == NULL) break;

            char *c = strtok_r(line, ":", &context);
            if(c == NULL) break;

            char *address = strtok_r(NULL, " ", &context);
            if(address == NULL) break;

            hostIP = string(address);
        }

        infile.close();
        remove(hostNameFile.str().c_str());
#else
    // TODO: Can windows fail and get localhost?
#endif
    }
    return hostIP;
#else

    string hostIP;
    //struct addrinfo hint = {0};
    //hint.ai_family = AF_INET;
    //hint.ai_socktype = SOCK_STREAM;

    struct addrinfo *aip = NULL;
    int ret = getaddrinfo(hostName.c_str(), NULL, NULL, &aip);
    if(ret != 0) return hostName;

    cerr << hostName << " ip list: " ;
    for(struct addrinfo *ptr = aip; ptr != NULL; ptr = ptr->ai_next)
    {
        cerr << "Name : " << ptr->ai_canonname << endl;
        hostIP = string(ptr->ai_canonname);

        //char hostname[NI_MAXHOST] = "";
        //int error = getnameinfo(ptr->ai_addr, ptr->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
        //if(error != 0) continue;

        //if(*hostname)
        //{
        //	struct in_addr addr;
        //	addr.s_addr = *(u_long*)hostname;
        //	cerr << hostname << " " << inet_ntoa(addr) << endl;
        //	hostIP = string(hostname);
        //}

        //switch(ptr->ai_family)
        //{
        //	case AF_INET:
        //	{
        //		cerr << "AF_INET (IPv4) " << endl;
        //		break;
        //	}
        //	default:
        //		cerr << "Unknown family: " << ptr->ai_family << endl;
        //		break;
        //}

        //cerr << "Name : " << ptr->ai_canonname << endl;
        //hostIP = string(ptr->ai_canonname);
    }

    freeaddrinfo(aip);

    return hostIP;
#endif
}*/

void printIpAddress(string hostName)
{
#if 1 //def USE_GCC
    struct addrinfo *result;
    struct addrinfo *res;
    int error;

    /* resolve the domain name into a list of addresses */
    error = getaddrinfo(hostName.c_str(), NULL, NULL, &result);
    if (error != 0)
    {
        fprintf(stderr, "error in getaddrinfo: %s\n", gai_strerror(error));
        return; //EXIT_FAILURE;
    }

    /* loop over all returned results and do inverse lookup */
    for (res = result; res != NULL; res = res->ai_next)
    {
        char hostname[NI_MAXHOST] = "";

        error = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0);
        if (error != 0)
        {
            fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(error));
            continue;
        }
        if (*hostname != '\0')
            printf("hostname: %s\n", hostname);
    }

    freeaddrinfo(result);
#else
    IFATAL() << "Not supported on windows!";
#endif
}

std::string getIPAddress(const std::string &hostName)
{
    iCritical() << PRETTY_FUNCTION << "WARNING! Outdated function! Pleas reimplement!";

    static Mutex staticMutex;
    MutexLocker lock(&staticMutex);

    //PrintIpAddress(hostName);
    // 1st: hostName == localhost then return
    if(strncmp(hostName.c_str(), "localhost", 9) == 0)
        return string("127.0.0.1");

    // 2nd: find hostName and IP in host files in either Linux or Windows
    ifstream infile;
    #ifdef USE_GCC
        infile.open("/etc/hosts", ios::in);
        if(infile.is_open() == false)
            iDebug() << "NetworkFunctions::GetIPAddress(" << hostName << "): WARNING! Couldn't open Linux /etc/hosts file.";
    #else

        char name[255];
        PHOSTENT hostinfo;
        if(gethostname ( name, sizeof(name)) == 0)
        {
            if((hostinfo = gethostbyname(name)) != NULL)
            {
                char *ip = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list);
                if(ip != NULL) return std::string(ip);
            }
        }


        if(infile.is_open() == false)
        {
            infile.open("c:\\WINNT\\system32\\drivers\\etc\\hosts", ios::in);
            if(infile.is_open() == false)
                infile.open("c:\\Windows\\system32\\drivers\\etc\\hosts", ios::in);
            if(infile.is_open() == false)
                infile.open("c:\\WINNT\\SysWOW64\\drivers\\etc\\hosts", ios::in);
            if(infile.is_open() == false)
                infile.open("c:\\Windows\\SysWOW64\\drivers\\etc\\hosts", ios::in);
            if(infile.is_open() == false)
            {
                iDebug() << "HostInformationSpace::GetIPAddress(" << hostName << "): WARNING! Couldn't open Windows hosts file.";
                //char *systemRoot = getenv("SystemRoot"); // TODO: use _dupenv_s()
                //if(systemRoot == NULL)
                //	systemRoot = getenv("WinDir");       // TODO: use _dupenv_s()
                //if(systemRoot != NULL)
                //	cout << "Try to code test path: " << systemRoot << "\\drivers\\etc\\hosts " << endl;

                char *systemRoot;
                size_t len;
                int err = _dupenv_s(&systemRoot, &len, "SystemRoot");
                if(err == 0 && systemRoot != NULL)
                {
                    iDebug() << "Try to code test path: " << systemRoot << "\\drivers\\etc\\hosts ";
                    free(systemRoot);
                }
                else if(err)
                {
                    err = _dupenv_s(&systemRoot, &len, "WinDir");
                    if(err == 0 && systemRoot != NULL)
                    {
                        iDebug() << "Try to code test path: " << systemRoot << "\\drivers\\etc\\hosts ";
                        free(systemRoot);
                    }
                }
            }
        }
    #endif
    if(infile.is_open() == true)
    {
        char line[4096];
        char *context = NULL;

        int failsafe = 0;
        while(!infile.eof() && infile.is_open())
        {
            infile.getline(line, 4096);
            //if(line == NULL) break;
            if(infile.gcount() <= 0) break;
            if(failsafe++ > 1000)
            {
                iDebug() << "NetworkFunctions::GetIPAddress(" << hostName << ") WARNING! Function is flawed! Failsafe kicked in. Please report problem!";
                break;
            }

            char *ip = STRTOK_S(line, " ", &context);
            if(ip == NULL) continue;
            if(strcmp(ip, "#") == 0) continue;

            char *address = STRTOK_S(NULL, " ", &context);
            if(address == NULL) continue;

            if(strcmp(address, hostName.c_str()) == 0)
                return string(ip);
        }

        infile.close();
    }

    // 3rd: find hostName and IP by using gethostbyname
    hostent *ent = gethostbyname(hostName.c_str());
    if(ent != NULL)
    {
        struct in_addr addr;
        string hostIP = hostName;
        //cerr << "GetHostByName(" << ent->h_name << ") Adr-list: " << endl;
        for(int i = 0; ent->h_addr_list[i] != 0; i++)
        {
            //if(ent->h_addr_list[i] != INADDR_LOOPBACK)
            addr.s_addr = *(u_long*)ent->h_addr_list[i];
            string ip = string(inet_ntoa(addr));

            //cerr << ip << " " ;

            //if(i == 0)
            hostIP = ip;
            break;
        }
        //cerr << endl;
        //cerr << "Aliases : " ;
        //for(int i = 0; ent->h_aliases[i] != 0; i++)
        //{
        //	cerr << ent->h_aliases[i] << " ";
        //}
        //cerr << endl;

        // found localhost IP? -> check /sbin/ifconfig
        if(strncmp(hostIP.c_str(), "127.0.0.1", 9) == 0)
        {
            #ifdef  USE_GCC
                // Linux:
                // /sbin/ifconfig eth0 | grep "inet addr:"
                // Result: inet addr:192.168.0.114  Bcast:192.168.0.255  Mask:255.255.255.0
                stringstream hostNameFile, stream;
                hostNameFile << hostName << rand() << "." << BaseLib::Thread::processId();
                stream << "/sbin/ifconfig eth0 | grep \"inet addr:\" > " << hostNameFile.str() ;
                int ret = system(stream.str().c_str());
                //if(ret == -1) return hostName;

                char line[4096];
                char *context = NULL;

                ifstream infile(hostNameFile.str().c_str());
                if(infile.is_open())
                {
                    while(!infile.eof() && infile.is_open())
                    {
                        infile.getline(line, 4096);
                        //if(line == NULL) break;
                        if(infile.gcount() <= 0) break;

                        char *c = strtok_r(line, ":", &context);
                        if(c == NULL) break;

                        char *address = strtok_r(NULL, " ", &context);
                        if(address == NULL) break;

                        hostIP = string(address);
                    }
                    infile.close();
                }

                remove(hostNameFile.str().c_str());
            #else
                // TODO: Can windows fail and get localhost?
                iDebug() << "NetworkFunctions::getIPAddress(" << hostName << "): WARNING! gethostbyname returned 127.0.0.1";
            #endif
        }
        return hostIP;
    }

    iDebug() << "NetworkFunctions::getIPAddress(" << hostName << "): WARNING! Failed to get host IP.";
    return hostName;
}

void getNodeAddress(string &hostaddress)
{
    char ac[80];
    if(gethostname(ac, sizeof(ac)) == -1) perror("couldn't get local host name");

    struct hostent *phe = gethostbyname(ac);
    if(phe == 0)
    {
        throw("Bad host lookup.");
        return;
    }

    // -- debug --
    //IDEBUG() << "Host name is " << ac << " / " << phe->h_name << " type " << phe->h_addrtype ;
    ASSERT(phe->h_addrtype == AF_INET);
    // -- end debug --

    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(addr));

        //IDEBUG() << "Host address " << i << ": " << inet_ntoa(addr) ;

        hostaddress = inet_ntoa(addr);
    }

    // getting ip
    std::string filename = UtilityFunctions::MakeRandomTempFile();

    char command[100];
    sprintf(command, "/sbin/ifconfig > %s", filename.c_str());

    int ret = system(command);

    ifstream myfile;
    myfile.open(filename.c_str());
    if(myfile.is_open() == false)
    {
        IDEBUG() << " system call " << command << " failed ";
        remove(filename.c_str());
        return;
    }

    ASSERT(myfile.is_open());

    char *token = NULL;
    bool eth0 = false, inet = false;
    while(!myfile.eof() && inet == false)
    {
        string line;
        getline(myfile, line);

        char *buffer = (char*)line.c_str();
        token = STRTOK_S((char*)line.c_str(), " ", &buffer);
        //cerr << " token: " << token << endl;
        if(token == NULL) break;

        if(strcmp(token, "inet") == 0 && eth0 == true)
        {
            inet = true;
            token = STRTOK_S(NULL, ":", &buffer); // addr:
            //cerr << " found inet, token : " << token ;
            token = STRTOK_S(NULL, " ", &buffer);
            //IDEBUG() " eth address : " << token ;
            hostaddress = token;
        }

        if(strcmp(token, "eth0") == 0 || strcmp(token, "eth1") == 0)
            eth0=true;
    }

    remove(filename.c_str());
    IDEBUG() << " address is " << hostaddress;
}

/*-----------------------------------------------------------------------
                            accept
------------------------------------------------------------------------- */
int accept(int listen_socket, sockaddr_in &addr)
{
    // receive message
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));

    int conn_sock = ::accept(listen_socket, (struct sockaddr*)&addr, &len);
    if(conn_sock == -1)
    {
        switch(errno)
        {
            case EINTR :
            case EWOULDBLOCK :
                // everything is all right, try again silently
                break;
            case EMFILE :
            case ENFILE :
                perror( "WARN: Too many open connections, quitting" );
                //exit( -__LINE__ );
                DIE("WARN: Too many open connections, quitting");
                break;
            default :
                perror( "WARN: Connection failure, ignore" );
                break;
        }
        return -1;
    }

    // -- debug --
    IDEBUG();
    fprintf(stderr, " Connection from host %s, port %d, socket %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), conn_sock);
    // -- end debug --

    //int err = fcntl(conn_sock, F_SETFL, O_NONBLOCK);
    //if(err == -1)
    //	IDEBUG() << " Warning: Failed to set non blocking " << strerror(errno);

#ifdef USE_GCC
    int err = fcntl(conn_sock, F_SETFD, FD_CLOEXEC);
    if(err == -1)
        IDEBUG() << " Warning: Failed to set flag " << FD_CLOEXEC << " err: " << strerror(errno);
#endif

    return conn_sock;
}

int my_accept(int listen_socket, sockaddr_in &addr) //, long flag)
{
    // receive message
    socklen_t len = sizeof(addr);
    memset(&addr, 0, sizeof(addr));

    int conn_sock = -1;
    for(int count = 0; count < 100 && conn_sock == -1; count++)
    {
        conn_sock = ::accept(listen_socket, (struct sockaddr*)&addr, &len);
        if(conn_sock > -1) break;

        switch(errno)
        {
            case EINTR :
            case EWOULDBLOCK :
                // everything is all right, try again silently
                break;
            case EMFILE :
            case ENFILE :
                perror( "WARN: Too many open connections, quitting" );
                //exit( -__LINE__ );
                DIE("WARN: Too many open connections, quitting");
                return -1;
                break;
            default :
                perror( "WARN: Connection failure, ignore" );
                return -1;
                break;
        }
        UtilityFunctions::MicroSleep(1000);
    }

    // -- debug --
    IDEBUG() ;
    fprintf(stderr, " Connection from host %s, port %d, socket %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), conn_sock);
    // -- end debug --

#ifdef USE_GCC
    /*if(flag != -1)
    {
        int err = fcntl(conn_sock, F_SETFL, flag);
        if(err == -1)
            IDEBUG() << " Warning: Failed to set flag " << flag << " err: " << strerror(errno);
    }*/

    int err = fcntl(conn_sock, F_SETFD, FD_CLOEXEC);
    if(err == -1)
        IDEBUG() << " Warning: Failed to set flag " << FD_CLOEXEC << " err: " << strerror(errno);
#endif

    int blockErr = UtilityFunctions::SetSocketBlocking(conn_sock);
    if(blockErr == -1)
        IDEBUG() << " Warning: Failed to set socket BLOCKING! err: " << strerror(errno);

    return conn_sock;
}

int getReceiveBufferSize(int socket)
{
    int bufferSize = 0;
    socklen_t optionLength = sizeof(bufferSize);
    if(getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &optionLength))
        DIE("Could not get the receive buffer size for the socket");

    return bufferSize;
}

int getSendBufferSize(int socket)
{
    int bufferSize = 0;
    socklen_t optionLength = sizeof(bufferSize);
    if(getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, &optionLength))
        DIE("Could not get the send buffer size for the socket");

    return bufferSize;
}

void setReceiveBufferSize(int socket, int bufferSize)
{
    //if(bufferSize<1024*1024)
    //    bufferSize = 1024*1024;
    if(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (const char*)&bufferSize, sizeof(bufferSize)))
        DIE("Could not set the receive buffer size for the socket");
    int actualBufferSize = NetworkFunctions::getReceiveBufferSize(socket);
    if(actualBufferSize < bufferSize)
        cout << "Could not set the receive buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    else
        cout << "Receive buffer size: " << actualBufferSize/1024 << " KB." << endl;
}

void setSendBufferSize(int socket, int bufferSize)
{
    //if(bufferSize<1024*1024)
    //    bufferSize = 1024*1024;

    if(setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (const char*)&bufferSize, sizeof(bufferSize)))
        DIE("Could not set the send buffer size for the socket");
    int actualBufferSize = NetworkFunctions::getSendBufferSize(socket);
    if(actualBufferSize < bufferSize)
        cout << "Could not set the send buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    else
        cout << "Send buffer size: " << actualBufferSize/1024 << " KB." << endl;
}

void bindSocketToLocalAddress(int socket, string localAddress, int port)
{
    struct sockaddr_in addr_local;
    memset(&addr_local, 0, sizeof(addr_local));
    addr_local.sin_family = AF_INET;
    addr_local.sin_port = htons(port);
    addr_local.sin_addr.s_addr = inet_addr(localAddress.c_str());

    if(::bind(socket, (sockaddr*)&addr_local, sizeof(addr_local)) < 0)
        throw (string("bindSocketToLocalAddress(): Error in binding to local interface address " + localAddress));
}

void setSocketOptionTTL(int socket, int timeToLive)
{
    int status = setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&timeToLive, sizeof(timeToLive));
    if(status != 0)
        throw ("setSocketOptionTTL(): ERROR! Could not set time to live on socket!", errno);
}


/*-----------------------------------------------------------------------
            connect to name using port, returns socket
------------------------------------------------------------------------- */
int connect(const char* name, int port)
{
    //IDEBUG() << " attempting connect: ( " << name << "," << port << ")" ;

    struct sockaddr_in addr;
    memset( &addr, 0, sizeof(struct sockaddr_in) );
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    int err = inet_pton( AF_INET, name, &addr.sin_addr );
    if( err < 0 ) DIE("Wrong address family" );
    else if( err == 0 )
    {
        struct hostent* hent;
        hent = gethostbyname( name );
        if( hent == NULL )
        {
        #ifdef USE_GCC
            herror( "name resolution failed" );
        #endif
            DIE("quitting");
        }
        //IDEBUG() << " Used gethostbyname and got host address "<<  inet_ntoa(addr.sin_addr);

        memcpy( &addr.sin_addr, hent->h_addr, hent->h_length );
    }

    int s;
    s = ::socket( PF_INET, SOCK_STREAM, 0); // 0 means default IPPROTO_TCP
    if( s <= -1 )DIE( "Could not create TCP socket" );

    //IDEBUG() << "Trying to connect to host address " << inet_ntoa(addr.sin_addr);

    int tries=0;
    do
    {
        err = ::connect( s, (struct sockaddr*)&addr, sizeof(sockaddr_in) );
        if( err < 0 )
        {
            //IDEBUG() << name << " connect failed socket: " << s << " socket: " << strerror(errno);
            if(tries < 3)
            {
                //cerr << " (retrying)" << endl;
                //sleep( 1 );
                UtilityFunctions::MicroSleep(10000);
            }
            else
            {
                //cerr << endl;
                //stringstream ostr;
                //IDEBUG() << "Failed to connect to : " << inet_ntoa(addr.sin_addr) << " port : " << ntohs(addr.sin_port);
                //DIE((char*)(ostr.str()).c_str());
                UtilityFunctions::CloseSocket(s);
                return -1;
            }
        }
        tries++;
    }
    while( err < 0 );

    IDEBUG() << " connected to host address " << inet_ntoa(addr.sin_addr);
    int True = 1;
    int len = sizeof(True);
    if(setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*)&True, len) == -1)
    {
        UtilityFunctions::CloseSocket(s);
        DIE(" can't setsockopt: SO_KEEPALIVE");
    }

    //struct linger {
    //	int l_onoff;    // linger active
    //	int l_linger;   // how many seconds to linger for
    //};

    struct linger ling;
    ling.l_onoff = 1;  // Sets linger on
    ling.l_linger = 3; // linger minimum 3 seconds to ensure all data is sent!

    if(setsockopt(s, SOL_SOCKET, SO_LINGER, (const char*)&ling, sizeof(ling)) == -1)
    {
        UtilityFunctions::CloseSocket(s);
        DIE(" can't setsockopt: SO_LINGER");
    }

#ifdef USE_GCC
    err = fcntl(s, F_SETFD, FD_CLOEXEC);
    if(err == -1)
        IDEBUG() << " Warning: Failed to set flag " << FD_CLOEXEC << " err: " << strerror(errno);
#endif

    return s;
}

#ifdef USE_GCC
int my_sendv(const int &sock, struct iovec vector[], const int &pos)
{
    int sz = my_size(vector, pos);

    int written = 0;
    for(int i = 0; i < pos; i++)
    {
        int err = my_send(sock, vector[i].iov_base, vector[i].iov_len);
        if(err <= -1) return -1;
        else written = written + err;
    }

    ASSERT(written == sz);

    return written;
}
#endif

int my_send(const int &sock, const void* buf, const int &length)
{
    int fail_safe = 0;
    int bwritten = 0, remains = length;
    while(bwritten < length)
    {
        int r = send(sock, (char*)buf + bwritten, remains, 0);
        if(r > -1) bwritten = bwritten + r;

        if(r == -1)
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
                        IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno);
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    stringstream stream;
                    stream << PRETTY_FUNCTION << "ERROR! read undefined error : " << strerror(errno) << endl;
                    DIE(stream.str().c_str());
                    //return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            //cerr << "%" ;
            ASSERT(bwritten <= length);
            ASSERT(bwritten >= 0);
            if(fail_safe++ > 10000000)
            {
                IDEBUG() << " fail safe kicked in: failed to send on socket " << sock ;
                return -1;
            }
            // -- debug end --
        }
        else if(bwritten < length)
            remains = length - bwritten;
        else if(bwritten == length)
            break;
    }

    return bwritten;
}


int my_sendto(const int &sock, const void *buf, const int &length, struct sockaddr_in &addr_info, int length_addr_info)
{
    int fail_safe = 0;
    int bwritten = 0, remains = length;
    while(bwritten < length)
    {
        int r = sendto(sock, (char*)buf + bwritten, remains, 0, (sockaddr*)&addr_info, length_addr_info);
        if(r > -1) bwritten = bwritten + r;

        if(r == -1)
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
                        IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno) ;
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    IDEBUG() << " read undefined error : " << strerror(errno) ;
                    //return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            //cerr << "%" ;
            ASSERT(bwritten <= length);
            ASSERT(bwritten >= 0);
            if(fail_safe++ > 10000000)
            {
                IDEBUG() << " fail safe kicked in: failed to send on socket " << sock;
                return -1;
            }
            // -- debug end --
        }
        else if(bwritten < length)
            remains = length - bwritten;
        else if(bwritten == length)
            break;
    }

    return bwritten;


}


int my_write(const int &sock, const void* buf, const int &length)
{
    //IDEBUG() << " socket " << sock << "write length " << length ;

    ASSERT(length > 0);
    int fail_safe = 0;
    int bread = 0, remains = length;
    while(bread < length)
    {
#ifdef USE_GCC
        int r = ::write(sock, (char*)buf + bread, remains);
#else
        int r = ::send(sock, (char*)buf + bread, remains, 0);
#endif
        if(r > -1) bread = bread + r;

        if(r == -1)
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
                    //IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno) ;
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    IDEBUG() << " undefined error : " << strerror(errno);
#ifdef WIN32
                    IDEBUG() << "Error " << GetLastError();
#endif
                    return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            cerr << "w" ;
            ASSERT(bread <= length);
            ASSERT(bread >= 0);
            if(fail_safe++ > 1000000000)
            {
                IDEBUG() << " fail safe kicked in: failed to write on socket " << sock;
                return -1;
            }
            // -- debug end --
        }
        else if(bread < length)
            remains = length - bread;
        else if(bread == length)
            break;
    }

    return bread;
}

#ifdef USE_GCC
int my_writev(const int &sock, struct iovec vector[], const int &pos)
{
    int sz = my_size(vector, pos);

    int written = 0, fail_safe = 0, curr_pos = 0;
    while(1)
    {
        int r = writev(sock, &(vector[curr_pos]), pos);
        if(r > 0) written = written + r;

        if(r <= -1)
        {
            switch(errno)
            {
                case ENOTSOCK:
                case ENOTCONN:
                case ECONNRESET:
                case ECONNREFUSED:
                case EINVAL:
                case EBADF:
                    IDEBUG() << " bad file descriptor " << sock << " error: " << strerror(errno) ;
                    return -1;
                    break;
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    IDEBUG() << " unrecognized error : " << strerror(errno);
                    return -1;
            }

            //cerr << "%" ;
            UtilityFunctions::MicroSleep(1000);
            if(fail_safe++ > 10000)
            {
                IDEBUG() << " fail safe kicked in: failed to writev on socket " << sock ;
                return -1; //DIE(" writev failed ");
            }
        }
        else if(written < sz && written > 0)
        {
            IDEBUG() << " socket " << sock << " writev wrote partially : " << written << " size: " << sz << " pos: " << pos << " err: " << strerror(errno) ;

            int s = 0, prev_s = 0;
            for(int i = 0; i < pos; i++)
            {
                prev_s = s;
                s += vector[i].iov_len;
                if(s == written)
                {
                    IDEBUG() << " writing from new pos : " << i + 1;
                    curr_pos = i + 1;
                    break;
                }
                else if(s > written) // wrote partially within a [i] -> finish block
                {
                    int new_len = s - written;
                    int start_pos = written - prev_s;
                    IDEBUG() << " start_pos " << start_pos << " new_len "<< new_len << " end_pos : " << s;

                    int e = my_write(sock, (char*)vector[i].iov_base + start_pos, new_len);
                    if(e <= -1) return -1;
                    written = written + e;
                    IDEBUG() << " socket " << sock << " wrote out position " << i << " size: " << e << " new_pos: " << i+1 << " written: " << written;

                    curr_pos = i + 1;
                    break;
                }
            }
        }

        if(written == sz) break;
    }

    ASSERT(written == sz);

    return written;
}

int my_size(struct iovec vector[], int p)
{
    int sz = 0;
    for(int i = 0; i < p; i++)
        sz += vector[i].iov_len;

    return sz;
}
#endif

int my_read(const int &sock, void* buf, const int &length)
{
    ASSERT(length > 0);
    int fail_safe = 0;
    int bread = 0, remains = length;
    while(bread < length)
    {
#ifdef USE_GCC
        int r = read(sock, (char*)buf + bread, remains);
#else
        int r = recv(sock, (char*)buf + bread, remains, 0);
#endif
        if(r == 0)
        {
            IDEBUG() << " Peer shut down : " << strerror(errno);
            return -1;
        }
        if(r > -1) bread = bread + r;

        if(r == -1)
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
                    IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno) ;
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    IDEBUG() << " undefined error : " << strerror(errno) ;
#ifdef WIN32
                    IDEBUG() << "Error: " << GetLastError();
#endif
                    return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            cerr << "r" ;
            ASSERT(bread <= length);
            ASSERT(bread >= 0);
            if(fail_safe++ > 1000000000)
            {
                IDEBUG() << " fail safe kicked in: failed to read on socket " << sock << strerror(errno) ;
                return -1;
            }
            // -- debug end --
        }
        else if(bread < length)
            remains = length - bread;
        else if(bread == length)
            break;
    }

    ASSERT(bread == length);

    return bread;
}

int my_recv(const int &sock, void* buf, const int &length)
{
    int fail_safe = 0;
    int bread = 0, remains = length;
    while(bread < length)
    {
        int r = recv(sock, (char*)buf + bread, remains, 0);
        if(r == 0)
        {
            IDEBUG() << " Peer shut down : " << strerror(errno);
            return -1;
        }
        if(r > -1) bread = bread + r;

        if(r == -1)
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
                    IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno) ;
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                case ENOMEM:
                default:
                    stringstream stream;
                    stream << PRETTY_FUNCTION << " read undefined error : " << strerror(errno) << " errno " << errno << endl;
                    DIE(stream.str().c_str());
                    //return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            if(fail_safe % 100)
                cerr << "%";

            ASSERT(bread <= length);
            ASSERT(bread >= 0);
            if(fail_safe++ > 1000000000)
            {
                IDEBUG() << " fail safe kicked in: failed to recv on socket " << sock ;
                return -1;
            }
            // -- debug end --
        }
        else if(bread < length)
            remains = length - bread;
        else if(bread == length)
            break;
    }

    ASSERT(bread == length);

    return bread;
}

int my_recvfrom(const int &sock, void *buf, const int &length, struct sockaddr_in &addr_info, int length_addr_info)
{
    int fail_safe = 0;
    int bread = 0, remains = length;
    while(bread < length)
    {
        int r = recvfrom(sock, (char*) buf + bread, remains, 0, (sockaddr*) &addr_info, (socklen_t*) &length_addr_info);
        if(r == 0)
        {
            IDEBUG() << " Peer shut down : " << strerror(errno);
            return -1;
        }
        if(r > -1) bread = bread + r;

        if(r == -1)
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
                    IDEBUG() << " bad file descriptor " << sock << "err: " << strerror(errno) ;
                    return -1;
                    break;
                case EIO:
                case EINTR:
                case EAGAIN:
                    break;
                default:
                    IDEBUG() << " read undefined error : " << strerror(errno) ;
                    //return -1;
                    break;
            }

            UtilityFunctions::MicroSleep(1000);
            // -- debug --
            //cerr << "%" ;
            ASSERT(bread <= length);
            ASSERT(bread >= 0);
            if(fail_safe++ > 1000000)
            {
                IDEBUG() << " fail safe kicked in: failed to recv on socket " << sock ;
                return -1;
            }
            // -- debug end --
        }
        else if(bread < length)
            remains = length - bread;
        else if(bread == length)
            break;
    }

    ASSERT(bread == length);
    return bread;
}

int my_ioctl(const int &sock, const int &request, int64 &amount)
{
    int err = 0;
    switch(request)
    {
        case FIONREAD:
#ifdef WIN32
            err = ::ioctlsocket(sock, request, &amount);
#else
            err = ::ioctl(sock, request, &amount);
#endif
            break;
        default:
            IDEBUG() << " undefined ioctl request : " ;
            break;
    }

    if(err < 0)
    {
        switch(errno)
        {
            case ECONNRESET:
            case ECONNREFUSED:
            case EINVAL:
            case EPIPE:
            case EBADF:
            case ENOTSOCK:
            case ENOTCONN:
                IDEBUG() << " error returned from ioctl " << strerror(errno) ;
                return -1;
                break;
            default:
                // ignore
                break;
        }
    }

    return 0;
}

double GetRoundTripTimeMS(const int &sock)
{
#ifdef MACX
    IWARNING() << "Not implemented!";
    return 0.0;
#elif USE_GCC
    struct tcp_info info;
    socklen_t sz = sizeof(info);
    int ret = getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, &sz);
    if(ret == -1)
    {
        IDEBUG() << " sock " << sock << " getsockopt rtt returned : " << ret << " error : " << strerror(errno) ;
        throw BaseLib::Exception("GetRoundTripTimeMS(socket): getsockopt TCP_INFO on socket i returned -1");
    }

    return info.tcpi_rtt/1000;
#else

    //	http://msdn.microsoft.com/en-us/library/windows/desktop/bb485738(v=vs.85).aspx
    IWARNING() << "Not implemented!";
    return 0.0;
#endif
}


}}; // namespace NetworkFunctions



