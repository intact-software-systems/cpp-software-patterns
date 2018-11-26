#ifndef NetworkLib_UdpSocket_h_IsIncluded
#define NetworkLib_UdpSocket_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/AbstractSocket.h"
#include"NetworkLib/Utility/NetworkFunctions.h"

#include"NetworkLib/Export.h"
namespace NetworkLib 
{

class DLL_STATE UdpSocket : public AbstractSocket
{
public:
	enum UdpID
	{
		NO_UDP_ID = 0,
		UnicastSender,
		MulticastSender,
		BroadcastSender,
		UnicastReceiver,
		MulticastReceiver,
		BroadcastReceiver
	};

public:
	UdpSocket();
    UdpSocket(UdpID id, std::string localAddr = std::string(""), int ttl = 1);
	UdpSocket(int clientSocket, const std::string &hostName, int port);
	~UdpSocket();

    CLASS_TRAITS(UdpSocket)
	
public:
	virtual int Write(const char* bytes, const int &length);
	virtual int Read(char* bytes, const int &length);

	virtual int ReadDatagram(char *bytes, const int &length, std::string &hostAddress, int &port);
	virtual int WriteDatagram(const char *bytes, const int &length, const std::string &hostAddress, const int &port);
	
	long long pendingDatagramSize() const;
	bool hasPendingDatagrams() const;

public: 
    bool bindReceiver(const std::string &hostName, int &port);
    bool bindSender(const std::string &hostName, const int &port);
	virtual bool isValid();

public:

    /**
     * @brief operator <<
     * @param ostr
     * @param socket
     * @return
     */
    friend std::ostream& operator<<(std::ostream& ostr, const UdpSocket::Ptr &socket)
    {
        if(!socket)
        {
            ostr << "NULL";
            return ostr;
        }

        ostr << "UdpSocket(fd: " << socket->socketDescriptor_ << ", port:" << socket->portNumber_ << " host:" << socket->hostName_ << ")";
        return ostr;
    }

private:
	Mutex 				mutexLock_;
	struct sockaddr_in 	addr_listener;
	std::string			localAddress;
	int					timeToLive;
	UdpID				udpID;
};

} // namespace NetworkLib

#endif // NetworkLib_UdpSocket_h_IsIncluded


