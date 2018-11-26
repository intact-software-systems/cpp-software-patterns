/***************************************************************************
                          NATNetworkFunctions.h  -  description
                             -------------------
    begin                : Tue Oct 18 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelv@ifi.uio.no
 ***************************************************************************/
#ifndef NetworkLib_NATNetworkFunctions_h_IsIncluded
#define NetworkLib_NATNetworkFunctions_h_IsIncluded

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
#endif

/*
The solution

Peer-to-peer protocols that wish to be NAT-friendly must be aware that any addresses they embed in
their data packets may be invalid once the packets pass through the NAT, and compensate accordingly.
One way to do it is as follows:

All traffic between the peers is done via a single UDP port. There is an address server which is not
behind any NAT. Users connect to the address server first, and send it the IP address they think they have;
the server notes both that address and the address it sees in the UDP header. The server then sends both
addresses to the other peers. At this point, everyone knows everyone else's address(es).

To open up peer-to-peer connections, all old peers send a UDP packet to the new peer, and the new peer sends
a UDP packet to each of the old peers. Since nobody knows at first whether they are behind the same NAT, the
first packet is always sent to both the public and the private address.

This causes everyone's NAT to open up a bidirectional hole for the UDP traffic to go through. Once the first
reply comes back from each peer, the sender knows which return address to use, and can stop sending to both addresses.

Compatibility Requirements

Above and beyond the basic NAT RFC, a NAT device that wants to support this scheme should have the following desirable property:

NATs should not change the number of UDP ports used by a stream of packets. 

Correlary:

If a host behind a NAT sends a series of packets from a single UDP port, the packets as relayed by the NAT should also
appear to come from a single host and UDP port.

*/
namespace NetworkLib { namespace NATNetworkFunctions 
{


}}


#endif // NetworkLib_NATNetworkFunctions_h_IsIncluded

