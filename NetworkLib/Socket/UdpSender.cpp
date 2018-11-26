#include"NetworkLib/Socket/UdpSender.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include<iostream>
#include<sstream>

namespace NetworkLib
{
UdpSender::UdpSender(UdpID id, int port, const std::string &destinationAddress, std::string localAddress, int timeToLive)
	: UdpBase(port, destinationAddress, id, localAddress, timeToLive)
{
	if(id == UDP_BROADCAST)
		throw ("UdpSender::UdpSender(): Use alternate constructor to create a UDP Broadcast connection!");

	Open();
}

UdpSender::UdpSender(int port, std::string localAddress)
	: UdpBase(port, std::string(""), UDP_BROADCAST, localAddress)
{
	Open();
}

void UdpSender::Open()
{
	if(isOpen) throw ("UdpSender::Open - UDP sender is already open.");

	try
	{
		switch(this->udpID)
		{
			case UdpBase::UDP_UNICAST:
			{
				OpenUnicast();
				break;
			}
			case UdpBase::UDP_MULTICAST:
			{
				OpenMulticast();
				break;
			}
			case UdpBase::UDP_BROADCAST:
			{
				OpenBroadcast();
				break;
			}
			default:
			{
				throw ("ERROR! Could not identify UdpID!");
				break;
			}
		}
	}
	catch(...)
	{
        IWARNING() << "UdpSender::Open(): Unidentified Error!";
	}
}

void UdpSender::OpenBroadcast()
{
#ifdef USE_WSA
	socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED);
#else
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    if(socket == -1)
		throw ("UdpSender::OpenBroadcast - Could not create a socket.");

	sockaddr_in addr;   
	addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);

    int optval = 1; // "true"
    int status = setsockopt(socket, SOL_SOCKET, SO_BROADCAST, (char*) &optval, sizeof(optval));
    if(status != 0)
		throw ("Could not set broadcast option.");

	if(!this->localAddress.empty())
		this->bindSocketToLocalAddress();

	status = connect(socket, (sockaddr*)&addr, sizeof (sockaddr_in));
	if(status == -1)
	{
        std::stringstream ss;
        ss << "Could not create UDP Broadcast socket on port " << this->port;
#ifdef USE_GCC
		throw (ss.str());
#else
        int lastError = WSAGetLastError();
		throw (ss.str(), lastError);
#endif
	}
	//networkStream = new NetworkStream(socket);

	isOpen = true;
}

void UdpSender::OpenMulticast()
{
	struct sockaddr_in addr_sender;
	
	socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if(socket < 0) throw ("Invalid Socket");

	memset(&addr_sender, 0, sizeof(addr_sender));
	addr_sender.sin_family = AF_INET;
	addr_sender.sin_port = htons(port);
	addr_sender.sin_addr.s_addr = inet_addr(this->hostName.c_str());
	
	if(!this->localAddress.empty())
		this->bindSocketToLocalAddress();

	if(this->timeToLive > 1)
		this->setSocketOptionTTL();

	//networkStream = new NetworkStream(socket, addr_sender);
	isOpen = true;
}

void UdpSender::OpenUnicast()
{
#ifdef USE_WSA
	socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED);
#else
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    if(socket == -1)
		throw ("UdpSender::OpenUnicast - Could not create a socket.");

	addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_canonname = 0;
	hints.ai_addr = 0;
	hints.ai_addrlen = 0;
	hints.ai_next = 0;

    std::stringstream portStringStream;
    portStringStream << port;

	addrinfo *ai = 0;
	int status = getaddrinfo(hostName.c_str(), portStringStream.str().c_str(), &hints, &ai);
	if(status) throw (std::string("Could not find the specified host." + hostName));

	if(!this->localAddress.empty()) 
		this->bindSocketToLocalAddress();

	status = connect(socket, ai[0].ai_addr, (int)ai[0].ai_addrlen);

	freeaddrinfo(ai);

	if(status == -1)
	{
        std::stringstream ss;
        ss << "Could not connect to port " << this->port << " on the host " << this->hostName;
#ifdef USE_GCC
		Close();
		throw (ss.str());
#else
        int lastError = WSAGetLastError();
		Close();
		throw (ss.str(), lastError);
#endif
	}
	
	//networkStream = new NetworkStream(socket);
	isOpen = true;
}

}; // namespace NetworkLib


