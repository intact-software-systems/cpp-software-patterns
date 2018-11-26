#ifndef MicroMiddleware_INetConnection_h_IsIncluded
#define MicroMiddleware_INetConnection_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/INet/INetInterfaces.h"
#include"MicroMiddleware/INet/INetBase.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class ConnectionManagerInterface;
class ProxyInterface;
class StubInterface;
class INetBase;

/******************************************************************************
 Purpose:
 INetConnection owns a socket
 INetConnection is used by both proxy and stub for all their RPC calls one-way and two-way
 INetConnection is passed to a stub/proxy through their constructor

 TODO: Add support for UDP in constructor and a bindToHost() function

 Identify Interface:
	 - ConnectionType
	 - ConnectionName
	 - ConnectionDomain
	 - ex: (StubConnection, "service.manager.interface", "Application.A")

TODO:
	- Support for peerAddress/peerPort, private/public addresses 
	- Request these addresses from RouteManager/AddressManager
	- Possible approach
		- Each connection is responsible for filling out 
			- peerAddress/peerPort
			- private/public addresses
		- Makes this information available to the rest of the system
		- Benefits: All services have complete information of connections

TODO: 
	- Should INetBase not be a base class of INetConnection but rather 
		created by INetConnection?
	- This would enable more specialized INetBase implementations based on
		if it is TCP or UDP, Proxy/Stub or Publisher/Subscriber that is needed

DDS-Terms:
		- An INetConnection is equivalent to "Publisher/Subscriber"

TODO:
	- Buffered messages in messageParser should be default because it is simplest and best
	- messageParser_->BeginUnmarshal can return a buffer holding one message of type SerializeReader
	- INetConnection can then parse this message if it is a middleware message
	- or, pass it to appropriate proxy/stub/publisher/subscriber for parsing

 ******************************************************************************/
class DLL_STATE INetConnection : public ConnectionInterface
							   , public Thread
{
public:
	typedef std::map<INetInterfaceIdentifier, INetInterface*> MapIdInterface;

public:
	INetConnection(NetworkLib::SocketStream* socketStream
					, const InterfaceHandle &handle
					, ConnectionInterface::ConnectionType type
					, bool autoStart = true);
	
	INetConnection(const InterfaceHandle &handle
					, ConnectionInterface::ConnectionType type
					, bool autoStart = true);

	virtual ~INetConnection();	

	virtual bool IsConnected();
	virtual void Stop(bool waitForTermination = true);
	
	virtual bool RegisterInterface(INetInterface*);
	virtual bool RemoveInterface(INetInterface*);

	virtual bool KeepAlive();

private:
	bool ProxyConnection(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader);
	bool StubConnection(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader);

private:
	virtual void run();
	
private:
	void processRpc();
	// TODO: Create specialized interfaces for reading middleware RPCs and
	// reading RPCs that do not match any interface as a fail-safe.
	bool middlewareRpc(const RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader);
	bool readUnprocessedRpc(RpcIdentifier &rpc, NetworkLib::SerializeReader::Ptr &reader);

private:
	// TODO: Move these to INetMessageParser implementation?
	bool connectToHost(const InterfaceHandle &handle);
	bool bindToInterfaceHandle(InterfaceHandle &handle);
	
	void notifyRegisteredInterfaces(bool connected);
	void init();

private: 
	// debug 
	void print(std::ostream &ostr);

private:
	MapIdInterface				mapIdInterface_;
	INetMessageParser::Ptr 		messageParser_;

private:
	bool            			runThread_;
	mutable Mutex				mutex_;
	int							socketDescriptor_;
};

}; // namespace MicroMiddleware



#endif // MicroMiddleware_INetConnection_h_IsIncluded

