#ifndef NetworkLib_UdpBase_h_IsIncluded
#define NetworkLib_UdpBase_h_IsIncluded

#include<stdlib.h>
#include<string>

namespace NetworkLib
{

/**
* @brief Base class for UdpSender and UdpReceiver
*/
class UdpBase
{
public:
	enum UdpID
	{
		NO_UDP_ID = 0,
		UDP_UNICAST,
		UDP_MULTICAST,
		UDP_BROADCAST
	};

protected:
    std::string		hostName;
	std::string		localAddress;
	int				port;
	int				timeToLive;
	int				socket;

	bool			isOpen;
	UdpID			udpID;

	//NetworkStream	*networkStream;

protected:
	/**
	* Base class for UdpSender and UdpReceiver. UdpSender and UdpReceiver implement the Open() function.
	*/
	UdpBase(int Port, std::string address, UdpID id, std::string localAddr = std::string(""), int ttl = 1) :
		hostName(address),
		localAddress(localAddr),
		port(Port), 
		timeToLive(ttl),
		socket(0),
		isOpen(false),
		udpID(id)
		//networkStream(0)
	{ }

	/**
	* Destructor closes the UDP connection.
	*/
	~UdpBase();

	void bindSocketToLocalAddress();
	void setSocketOptionTTL();

public:	

    /**
    * Returns the name of the host to which the client is connected.
    *
    * @return   a reference to the host name
    */
	inline const std::string& GetHostName() const { return hostName; }

    /**
    * Returns the port number to which the client is connected.
    *
    * @return   the UDP/IP port number 
    */
	inline int GetPort() const { return port; }

	/**
	* Opens this UDP client connection.
	*/
	virtual void Open() {}

	/**
	* Closes this UDP client connection.
	*/
	void Close();

	/**
	* Returns a network stream.  The returned stream is owned by this UdpBase and should
    * not be deleted by the client calling this method.
	*
	* @return the new network stream
	*/
	//NetworkStream* GetNetworkStream();


	/**
	* Check if UDP connection is open
	* 
	* @return true if connection is open, false otherwise
	*/
	inline bool IsOpen() { return isOpen; }


    /**
    * Sets the size of the receive buffer.
    *
    * @param bufferSize     the size of the buffer in bytes
    */
    void SetReceiveBufferSize(int bufferSize);


    /**
    * Returns the size of the receive buffer.
    *
    * @return     the size of the buffer in bytes
    */
    int GetReceiveBufferSize();


    /**
    * Sets the size of the send buffer.
    *
    * @param bufferSize     the size of the buffer in bytes
    */
    void SetSendBufferSize(int bufferSize);


    /**
    * Returns the size of the send buffer.
    *
    * @return     the size of the buffer in bytes
    */
    int GetSendBufferSize();
};

}; // namespace NetworkLib

#endif


