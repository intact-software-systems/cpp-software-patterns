#ifndef NetworkLib_UdpReceiver_h_IsIncluded
#define NetworkLib_UdpReceiver_h_IsIncluded

#include <stdlib.h>
#include "NetworkLib/Socket/UdpBase.h"

namespace NetworkLib
{
/**
* @brief Creates and opens a UDP receiver which is either Unicast, Multicast or Broadcast.
*/
class UdpReceiver : public UdpBase
{
public:
	/**
	* Opens a UDP receiver which is either Unicast or Broadcast.
	*
	* @param UdpID					Decides whether UDP is Unicast or Broadcast.
	* @param port					The port to receive UDP packets.
	*/
	UdpReceiver(UdpID id, int port);

	/**
	* Opens a UDP receiver for Multicast.
	*
	* @param port					The port to receive UDP packets.
	* @param multicastAddress		The multicastAddress defines the multicast group to join and receive Multicast data from.
	* @param localAddress			Optional argument to decide which local network interface to use.
	*/
	UdpReceiver(int port, const std::string &multicastAddress, std::string localAddress = std::string(""));

	virtual ~UdpReceiver(void)
	{ }

protected:
	/**
	* Opens a UDP receiver which is either Unicast, Multicast or Broadcast.
	*/
	virtual void Open();

private:
	/**
	* Opens a UDP broadcast receiver.
	*/
	void OpenBroadcast();
	
	/**
	* Opens a UDP multicast receiver.
	*/
	void OpenMulticast();
	
	/**
	* Opens a UDP unicast receiver.
	*/
	void OpenUnicast();
};

}; // namespace NetworkLib

#endif


