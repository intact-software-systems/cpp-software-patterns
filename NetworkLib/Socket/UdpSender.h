#ifndef NetworkLib_UdpSender_h_IsIncluded
#define NetworkLib_UdpSender_h_IsIncluded

#include <stdlib.h>
#include "NetworkLib/Socket/UdpBase.h"

namespace NetworkLib
{
/**
* @brief Creates and opens a UDP sender, which is either Unicast, Multicast or Broadcast.
*/
class UdpSender : public UdpBase
{
public:
	/**
	* Opens a UDP sender which is either Unicast or Multicast.
	*
	* @param UdpID					Decides whether UDP is used for Unicast or Multicast.
	* @param port					The port to use for UDP sending.
	* @param destinationAddress		For Unicast, it is the destinationAddress to send UDP packets to. 
									For Multicast, it defines the multicast group to join and send Multicast data to.
	* @param localAddress			Optional argument to decide which local network interface to use.
	* @param timeToLive				Optional argument to set time to live (hop-count) of UDP Multicast packets.
	*								Generally, you should use the default timeToLive = 1.
	*/
	UdpSender(UdpID id, int port, const std::string &destinationAddress, std::string localAddress = std::string(""), int timeToLive = 1);

	/**
	* Opens a UDP sender connection which is Broadcast.
	*
	* @param port					The port to use for UDP sending.
	* @param localAddress			Optional argument to decide which local network interface to use.
	*/
	UdpSender(int port, std::string localAddress = std::string(""));

	virtual ~UdpSender(void)
	{ }

protected:
	/**
	* Opens a UDP sender connection which is either Unicast, Multicast or Broadcast.
	*/
	virtual void Open();

private:
	
	/**
	* Opens a UDP broadcast sender.
	*/
	void OpenBroadcast();
	
	/**
	* Opens a UDP multicast sender.
	*/
	void OpenMulticast();
	
	/**
	* Opens a UDP unicast sender.
	*/
	void OpenUnicast();
};

}; // namespace NetworkLib

#endif
