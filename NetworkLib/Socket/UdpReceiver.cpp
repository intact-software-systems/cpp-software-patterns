#include "NetworkLib/Socket/UdpReceiver.h"
#include "NetworkLib/Utility/NetworkFunctions.h"
#include <iostream>
#include <sstream>

namespace NetworkLib
{
UdpReceiver::UdpReceiver(UdpID id, int port)
	: UdpBase(port, std::string(""), id)
{
	if(id == UDP_MULTICAST)
		throw ("UdpReceiver::UdpReceiver(): Use alternate constructor to create a UDP Multicast connection!");

	Open();
}

UdpReceiver::UdpReceiver(int port, const std::string &multicastAddress, std::string localAddress)
	: UdpBase(port, multicastAddress, UDP_MULTICAST, localAddress)
{
	Open();
}

void UdpReceiver::Open()
{
	if(isOpen) throw ("UdpReceiver::Open - UDP receiver is already open.");

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
        IWARNING() << "UdpReceiver::Open(): Unidentified Error!";
	}
}

void UdpReceiver::OpenMulticast()
{
	struct sockaddr_in addr_listener;
	
	socket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if(socket < 0) throw ("Invalid Socket");

	memset(&addr_listener, 0, sizeof(addr_listener));
	addr_listener.sin_family = AF_INET;
	addr_listener.sin_port = htons(port);
	addr_listener.sin_addr.s_addr = htonl(INADDR_ANY);

	if(::bind(socket, (sockaddr*)&addr_listener, sizeof(addr_listener)) < 0)
		throw ("Error in bind");

	struct ip_mreq imr;
	imr.imr_multiaddr.s_addr = inet_addr(this->hostName.c_str());
	imr.imr_interface.s_addr = htonl(INADDR_ANY);

	if(!this->localAddress.empty())
		imr.imr_interface.s_addr = inet_addr(this->localAddress.c_str());

#ifdef USE_GCC
	int nret = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, &imr, sizeof(imr));
#else
	int nret = setsockopt(socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char FAR *) &imr, sizeof(struct ip_mreq));
#endif

	if(nret < 0)
	{
#ifndef USE_GCC
        std::cerr << "ERROR: " << WSAGetLastError() << " ret " << nret << std::endl;
#endif
		throw ("Error - setsockopt IP_ADD_MEMBERSHIP");
	}

	//networkStream = new NetworkStream(socket, addr_listener);
	isOpen = true;
}

void UdpReceiver::OpenUnicast()
{
	struct sockaddr_in addr_listener;

#ifdef USE_WSA
	socket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, 0, 0, WSA_FLAG_OVERLAPPED);
#else
	socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
#endif

    if(socket == -1)
		throw ("UdpReceiver::OpenUnicast - Could not create a socket.");

	memset(&addr_listener, 0, sizeof(addr_listener));
	addr_listener.sin_family = AF_INET;
	addr_listener.sin_port = htons(this->port);
	addr_listener.sin_addr.s_addr = htonl(INADDR_ANY);

	if(::bind(socket, (sockaddr*)&addr_listener, sizeof(addr_listener)) < 0)
		throw ("Error in bind");
	
	//networkStream = new NetworkStream(socket);
	isOpen = true;
}

void UdpReceiver::OpenBroadcast()
{
	OpenUnicast();
}

}; // namespace NetworkLib


