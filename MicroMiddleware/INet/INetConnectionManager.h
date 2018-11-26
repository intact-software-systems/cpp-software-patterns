#ifndef MicroMiddleware_INetConnectionManager_h_IsIncluded
#define MicroMiddleware_INetConnectionManager_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/INet/INetInterfaces.h"

#include <map>
#include <set>

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class INetServerListener;
class INetConnection;

/*-----------------------------------------------------------------------
            class INetConnectionManager 
	TODO: 
		- RemoveConnection(const InterfaceHandle &handle);
		- RemoveAll();	
		- RemoveInterface(INetInterface*)
------------------------------------------------------------------------- */
class DLL_STATE INetConnectionManager : public ConnectionManagerInterface
										, public Thread
{
public:
	typedef map<InterfaceHandle, ServerListenerInterface*> 	MapHandleServerListener;
	typedef map<InterfaceHandle, ConnectionInterface*>  	MapHandleConnectionInterface;
	typedef map<int, ServerListenerInterface*>				MapSocketServerListener;
	typedef map<int, ConnectionInterface*>					MapSocketConnectionInterface;
	
	typedef set<ServerListenerInterface*>					SetServerListener;
	typedef set<ConnectionInterface*>						SetConnection;
	typedef map<ServerListenerInterface*, SetConnection > 	MapServerConnectionSet;

private:
	INetConnectionManager(const std::string &name = std::string("MicroMiddleware.Manager.Connection"));
	~INetConnectionManager();
	
private:
	virtual void run();

public:
	static INetConnectionManager* GetOrCreate();

public:
	// called from outside
	virtual INetConnection* 	GetOrCreateConnection(const InterfaceHandle &handle, ConnectionInterface::ConnectionType type = ConnectionInterface::TcpClient);
	virtual INetServerListener* GetOrCreateServerListener(bool useExistingServer = false, const InterfaceHandle &handle = InterfaceHandle());
	
	// For Proxy/Stub/Publisher/Subscriber
	virtual bool RegisterInterface(const InterfaceHandle &handle, INetInterface*);
	virtual bool RemoveInterface(const InterfaceHandle &handle, INetInterface*);

public:
	virtual ConnectionInterface* AddConnection(int socketDescriptor, ServerListenerInterface*); // callback from INetServerListener
	virtual void AddConnection(int socketDescriptor, ConnectionInterface*); 					// callback from INetConnection upon reconnection
	virtual void RemoveServerListener(int socketDescriptor, ServerListenerInterface*); 			// callback from INetServerListener
	virtual void RemoveConnection(int socketDescriptor, ConnectionInterface* connection); 		// callback from INetConnection
	virtual void RemoveConnection(ConnectionInterface *conn);

protected:
	// non thread safe functions
	INetConnection* addConnection(TcpSocket::Ptr, ConnectionInterface::ConnectionType type);
	TcpSocket* 		connectToHost(const InterfaceHandle &handle);

private:
	// not mutexed, not thread safe
	INetConnection*			getOrCreateConnection(const InterfaceHandle &handle, ConnectionInterface::ConnectionType type = ConnectionInterface::TcpClient);
	INetConnection*			getConnection(const InterfaceHandle &handle);
	ConnectionInterface*	removeConnection(const InterfaceHandle &handle);

private:
	mutable Mutex	mutex_;
	WaitCondition   waitCondition_;
	bool			runThread_;

private:
	MapSocketServerListener			mapSocketServerListener_;
	MapSocketConnectionInterface	mapSocketConnectionInterface_;
	
	MapHandleServerListener			mapHandleServerListener_;
	MapHandleConnectionInterface	mapHandleConnectionInterface_;

	MapServerConnectionSet			mapServerConnectionSet_;
	
	// invalid, to be deleted, connections and servers
	SetConnection					setInvalidConnections_;
	SetServerListener				setInvalidServers_;

private:
	static INetConnectionManager	*connectionManager_;
};

} // namespace MicroMiddleware

#endif

