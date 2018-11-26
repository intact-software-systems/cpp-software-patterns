#include"NetworkLib/Socket/UdpBase.h"
#include"NetworkLib/Utility/NetworkFunctions.h"
#include <errno.h>
//#include "NetworkLib/NetworkStream.h"
#include <iostream>

using namespace std;

//#ifndef USE_GCC
//    #define socklen_t int
//#endif

namespace NetworkLib
{

UdpBase::~UdpBase(void)
{
	if(isOpen)
		Close();
    //if(networkStream)
    //    delete networkStream;
}

void UdpBase::Close()
{
    //if(isOpen) {
     //   networkStream->Close();
    //	isOpen = false;
    //}
}

void UdpBase::setSocketOptionTTL()
{
	int status = 0;
	switch(this->udpID)
	{
		case UdpBase::UDP_UNICAST:
		{
			//status = setsockopt(socket, SOL_SOCKET, SO_IP_TTL, (char*)&timeToLive, sizeof(timeToLive));
			break;
		}
		case UdpBase::UDP_MULTICAST:
		{
			status = setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&timeToLive, sizeof(timeToLive));
			break;
		}
		case UdpBase::UDP_BROADCAST:
		{
			//status = setsockopt(socket, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&timeToLive, sizeof(timeToLive));
			break;
		}
		default:
		{
			throw ("UdpBase::setSocketOptionTTL(): ERROR! Could not identify UdpID!");
			break;
		}
	}

	if(status != 0)
		throw "UdpBase::setSocketOptionTTL(): ERROR! Could not set time to live on socket!", errno;
}

void UdpBase::bindSocketToLocalAddress()
{
	if(this->localAddress.empty()) throw ("UdpBase::bindSocketToLocalAddress(): No local address given!");
	struct sockaddr_in addr_local;
	memset(&addr_local, 0, sizeof(addr_local));
	addr_local.sin_family = AF_INET;
	addr_local.sin_port = htons(this->port);
	addr_local.sin_addr.s_addr = inet_addr(this->localAddress.c_str());		

	if(::bind(socket, (sockaddr*)&addr_local, sizeof(addr_local)) < 0)
		throw (string("UdpBase::bindSocketToLocalAddress(): Error in binding to local interface address " + localAddress));
}

//NetworkStream* UdpBase::GetNetworkStream() 
//{
//	if(isOpen)
//		return networkStream;
//	else
//		throw ("UdpBase::GetNetworkStream(): Could not return a network stream because the UDP client connection is not open.");
//}


void UdpBase::SetReceiveBufferSize(int bufferSize)
{
    if(setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize)))
		throw ("UdpBase::SetReceiveBufferSize: Could not set the receive buffer size for the socket");
    int actualBufferSize = GetReceiveBufferSize();
    if(actualBufferSize < bufferSize)
        cout << "Could not set the receive buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    else
        cout << "Receive buffer size: " << actualBufferSize/1024 << " KB." << endl;
}


int UdpBase::GetReceiveBufferSize()
{
    int bufferSize = 0;
    socklen_t optionLength = sizeof(bufferSize);
    if(getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &optionLength))
		throw ("UdpBase::GetReceiveBufferSize(): Could not get the receive buffer size for the socket");

    return bufferSize;
}


void UdpBase::SetSendBufferSize(int bufferSize)
{  
    if(setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, sizeof(bufferSize)))
		throw ("UdpBase::SetSendBufferSize: Could not set the send buffer size for the socket");
    int actualBufferSize = GetSendBufferSize();
    if(actualBufferSize < bufferSize)
        cout << "Could not set the send buffer size to " << bufferSize << ".  Current buffer size is " << actualBufferSize << "." << endl;
    else
        cout << "Send buffer size: " << actualBufferSize/1024 << " KB." << endl;
}


int UdpBase::GetSendBufferSize()
{
    int bufferSize = 0;
    socklen_t optionLength = sizeof(bufferSize);
    if(getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char*)&bufferSize, &optionLength))
		throw ("UdpBase::GetSendBufferSize(): Could not get the send buffer size for the socket");

    return bufferSize;
}

} // namespace NetworkLib


