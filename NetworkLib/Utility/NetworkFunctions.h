/***************************************************************************
                          NetworkFunctions.h  -  description
                             -------------------
    begin                : Wed Mar 21 2007
    copyright            : (C) 2007 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef NetworkLib_NetworkFunctions_h_IsIncluded
#define NetworkLib_NetworkFunctions_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <list>

#ifdef USE_GCC
#include <getopt.h>
#include <utime.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/select.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#endif

#include"NetworkLib/Export.h"
namespace NetworkLib { namespace NetworkFunctions 
{
void DIE(const char *msg);
	
void ByteSwap(unsigned char * b, int n);
int tcp_port(int &port);
int udp_port(int &port);
void resolveIP(char* name, const char *ip);

void getNodeAddress(std::string &hostaddress);
std::string getNodeAddress();
int getSockAddress(std::string &hostaddress);
int getSockAddress(int sock, std::string &hostaddress);
int getPeerAddress(int sock, std::string &hostaddress);
int getPeerPort(int sock, int &port);
int getPeerSockAddr(int sock, struct sockaddr_in &addr);
double my_ping(const char *addr, int num);

DLL_STATE std::list<std::string> getLocalInterfaceAddresses(void);
DLL_STATE std::list<std::string> getLocalIPs(bool include_loopback);
DLL_STATE bool isPrivateIP(const struct sockaddr *sa);
DLL_STATE std::string getIpForInterface(const char *interfaceName);
DLL_STATE bool isIPAddressOnHost(const std::string &hostName, const std::string &ipAddress);
DLL_STATE std::string getIPAddress(const std::string &hostName);


// tcp socket functions
int receiver(int port);
#ifdef USE_GCC
int receiver_wscan(int &port, long val);
#endif
int bindListen(int &port);
int connect(const char *name, int port);
int accept(int socket, sockaddr_in &);
int my_accept(int socket, sockaddr_in &); //, long val = -1);

// udp socket functions
int bindIPMulticastReceiver(struct sockaddr_in &addr_listener, std::string hostName, int port, std::string localAddress);
int bindUdpUnicastReceiver(int &port);
int bindUdpBroadcastReceiver(int &port);

int bindIPMulticastSender(struct sockaddr_in &addr_listener, std::string hostName, int port, std::string localAddress, int timeToLive);
int bindUdpUnicastSender(struct sockaddr_in &addr_listener, std::string hostName, int port, std::string localAddress);
int bindUdpBroadcastSender(struct sockaddr_in &addr_listener, int port, std::string localAddress);

//int bindUdpUnicast(struct sockaddr_in &addr_listener, const int &port);

// buffer sizes
int getReceiveBufferSize(int socket);
int getSendBufferSize(int socket);
void setReceiveBufferSize(int socket, int bufferSize);
void setSendBufferSize(int socket, int bufferSize);

// set socket options
void bindSocketToLocalAddress(int socket, std::string localAddress, int port);
void setSocketOptionTTL(int socket, int timeToLive);

// iovec
#ifdef USE_GCC
int my_sendv(const int &sock, struct iovec vector[], int pos);
#endif

int my_send(const int &sock, const void* buf, const int &len);
int my_sendto(const int &socket, const void *bytes, const int &length, struct sockaddr_in &addr_info, int length_addr_info);
int my_write(const int &sock, const void* buf, const int &len);

#ifdef USE_GCC
int my_writev(const int &sock, struct iovec vector[], const int &pos);
int my_size(struct iovec vector[], int pos);
#endif
	
// read buffer
int my_read(const int &sock, void* buf, const int &length);
int my_recv(const int &sock, void* buf, const int &length);
int my_recvfrom(const int &sock, void *bytes, const int &length, struct sockaddr_in &addr_info, int length_addr_info);
int my_ioctl(const int &sock, const int &request, int64 &m);

// get socket options
DLL_STATE double GetRoundTripTimeMS(const int &sock);

#ifdef USE_WSA
namespace Windows 
{
int bindListen(int port);
int connect(const char *name, int port);
int my_recv(int socket);
}
#endif

}} // namespace NetworkFunctions

#define ByteSwap5(x) NetworkFunctions::ByteSwap((unsigned char *) &x,sizeof(x))

#endif // PLANETLAB_SOCKETFUNCTIONS_KHV


