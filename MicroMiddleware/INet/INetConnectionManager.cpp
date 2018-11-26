#include "MicroMiddleware/INet/INetConnectionManager.h"
#include "MicroMiddleware/INet/INetServerListener.h"
#include "MicroMiddleware/INet/INetConnection.h"

namespace MicroMiddleware
{

INetConnectionManager* INetConnectionManager::connectionManager_ = NULL;

/*-----------------------------------------------------------------------
               INetConnectionManager constructor/destructor 
------------------------------------------------------------------------- */
INetConnectionManager::INetConnectionManager(const std::string &name) 
	: Thread(name)
	, runThread_(true)
{
	start();
}

INetConnectionManager::~INetConnectionManager()
{

}

/*-----------------------------------------------------------------------
              static create functions 
------------------------------------------------------------------------- */
INetConnectionManager* INetConnectionManager::GetOrCreate()
{
	static Mutex mutex;
	MutexLocker lock(&mutex);

	if(INetConnectionManager::connectionManager_ == NULL)
		INetConnectionManager::connectionManager_ = new INetConnectionManager();

	return INetConnectionManager::connectionManager_;
}

/*-----------------------------------------------------------------------
             main thread function 
		
		- Sends out periodic keep-alive messages
TODO:
	- Support for peerAddress/peerPort, private/public addresses 
	- Request these addresses from RouteManager/AddressManager
	- Possible approach
		- Each connection is responsible for filling out 
			- peerAddress/peerPort
			- private/public addresses
		- Makes this information available to the rest of the system
		- Benefits: All services have complete information of connections

------------------------------------------------------------------------- */
void INetConnectionManager::run()
{
	try
	{
		IDEBUG() << "running";
		mutex_.lock();

		while(runThread_)
		{
			bool ret = waitCondition_.wait(&mutex_, 1000);
			if(!runThread_) break;
			
			// Continuously check if INetConnections and INetServerListeners are still connected to their sockets
			// Send out KeepAlive messages!
			for(MapSocketConnectionInterface::iterator it = mapSocketConnectionInterface_.begin(), it_end = mapSocketConnectionInterface_.end(); it != it_end; ++it)
			{
				ConnectionInterface *conn = it->second;
				ASSERT(conn);
				bool ret = conn->KeepAlive();
				if(!ret)
					IDEBUG() << " Failed to send KeepAlive on handle " << conn->GetInterfaceHandle();
				
				// TODO: What to do if it fails?
			}
			
			// Remove old invalid connections
			for(SetConnection::iterator it = setInvalidConnections_.begin(), it_end = setInvalidConnections_.end(); it != it_end; ++it)
			{
				ConnectionInterface *conn = *it;
				ASSERT(conn);
				ASSERT(!conn->IsConnected());
		
				//conn->Stop(true);
				delete conn;
			}
			setInvalidConnections_.clear();

			// clear out old servers
			for(SetServerListener::iterator it = setInvalidServers_.begin(), it_end = setInvalidServers_.end(); it != it_end; ++it)
			{
				ServerListenerInterface *conn = *it;
				ASSERT(conn);
				
				delete conn;
			}
			setInvalidServers_.clear();
		}
	}
	catch(Exception ex)
	{
		iCritical() << PRETTY_FUNCTION << "ERROR! Exception caught " << ex.what();
	}
	catch(...)
	{
		iFatal("INetConnectionManager::run(): ERROR! Exception caught \n");
	}

	mutex_.unlock();
}

/*-----------------------------------------------------------------------
            Remove Connection
------------------------------------------------------------------------- */
void INetConnectionManager::RemoveConnection(ConnectionInterface *conn)
{
	ASSERT(conn);
	MutexLocker lock(&mutex_);

	if(conn->IsConnected())
		IDEBUG() << "WARNING! Removing " << conn->GetInterfaceHandle() << " but it is still connected!" ;
	
	ConnectionInterface *found = removeConnection(conn->GetInterfaceHandle());
	
	// -- start invariants --
	ASSERT(found);
	ASSERT(found == conn);
	// -- end invariants --

	setInvalidConnections_.insert(found);
}

/*-----------------------------------------------------------------------
            create server listener 
	TODO: 
		- Be able to take as input an empty handle, and listen to 
		arbitrary port
		- Assume TcpServer! 
------------------------------------------------------------------------- */
INetServerListener* INetConnectionManager::GetOrCreateServerListener(bool useExistingServer, const InterfaceHandle &handle)
{
	MutexLocker lock(&mutex_);

	for(MapSocketServerListener::iterator it = mapSocketServerListener_.begin(), it_end = mapSocketServerListener_.end(); it != it_end; ++it)
	{
		ServerListenerInterface *conn = it->second;
		ASSERT(conn);
		ASSERT(conn->GetInterfaceHandle().IsValid());

		if(conn->GetInterfaceHandle() == handle ||
		   (!handle.IsValid() && useExistingServer))
		{
			ASSERT(conn->socketDescriptor() > 0);
			
			INetServerListener* server = dynamic_cast<INetServerListener*>(conn);
			ASSERT(server);
			return server;
		}
	}

	// ServerListener was not found, creating
	INetServerListener* server = new INetServerListener(handle, this, true);
	bool ret = server->Start(true);
	ASSERT(ret);

	// the ServerListenerInterface object is ALSO automatically added through AddConnection if it receives a connection 
	if(mapSocketServerListener_.count(server->socketDescriptor()) <= 0)
	{
		IDEBUG() << "Added server listener on socket " << server->socketDescriptor() ;
		mapSocketServerListener_[server->socketDescriptor()] = server;
	}
	if(mapHandleServerListener_.count(server->GetInterfaceHandle()) <= 0)
	{
		IDEBUG() << "Added server listener on handle " << server->GetInterfaceHandle() ;
		mapHandleServerListener_[server->GetInterfaceHandle()] = server;
	}

	return server;
}

/*--------------------------------------------------------------------------------------
            get or create connection 
		TODO: 
			-> TcpClient, UdpSender, UdpReceiver, MulticastSender, MulticastReceiver
--------------------------------------------------------------------------------------*/
INetConnection* INetConnectionManager::GetOrCreateConnection(const InterfaceHandle &handle, ConnectionInterface::ConnectionType type)
{
	MutexLocker lock(&mutex_);
	return getOrCreateConnection(handle, type);
}

// -----------------------------------------------------------------------------------
//	Register and remove interfaces to/from INetConnections
// -----------------------------------------------------------------------------------
bool INetConnectionManager::RegisterInterface(const InterfaceHandle &handle, INetInterface *inet)
{
	ASSERT(handle.IsValid());
	ASSERT(inet);
	ASSERT(inet->IsValid());
	
	MutexLocker lock(&mutex_);

	switch(inet->GetInterfaceType())
	{
		case HostInformation::PROXY:
		{
			ConnectionInterface *conn = NULL;
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG()  << "WARNING! No connection matched to " << handle << " Creating!" ;
				conn = getOrCreateConnection(handle, ConnectionInterface::TcpClient);
			}
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::TcpClient);

			return conn->RegisterInterface(inet);
			break;
		}
		case HostInformation::STUB:
		{
			if(mapHandleServerListener_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No server listener matched to " << handle ;
				return false;
			}

			ServerListenerInterface *server= mapHandleServerListener_[handle];
			bool ret = server->RegisterInterface(inet);
			if(!ret)
				IDEBUG() << "WARNING! Could not add stub to serverListener!" ;

			if(mapServerConnectionSet_.count(server) > 0)
			{
				const SetConnection &setConn = mapServerConnectionSet_[server];
				for(SetConnection::const_iterator it = setConn.begin(), it_end = setConn.end(); it != it_end; ++it)
				{
					ConnectionInterface *conn = *it;
					conn->RegisterInterface(inet);
				}
			}
			else
			{
				IDEBUG() << " No connections on server " << server->GetInterfaceHandle() ;
			}
			return true;
			break;
		}
		case HostInformation::PUBLISHER:
		{
			ConnectionInterface *conn = NULL;
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No connection matched to " << handle << " Creating!" ;
				conn = getOrCreateConnection(handle, ConnectionInterface::UdpSender);
			}
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::UdpSender);

			return conn->RegisterInterface(inet);
			break;
		}
		case HostInformation::SUBSCRIBER:
		{
			ConnectionInterface *conn = NULL;
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No connection matched to " << handle << " Creating!" ;
				conn = getOrCreateConnection(handle, ConnectionInterface::UdpReceiver);
			}
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::UdpReceiver);

			return conn->RegisterInterface(inet);
			break;
		}
		default:
			IDEBUG() << "ERROR! Could not identify : " << inet->GetInterfaceType() ;
			break;
	}

	return false;
}


bool INetConnectionManager::RemoveInterface(const InterfaceHandle &handle, INetInterface *inet)
{
	ASSERT(handle.IsValid());
	ASSERT(inet);
	ASSERT(inet->IsValid());
	
	MutexLocker lock(&mutex_);

	switch(inet->GetInterfaceType())
	{
		case HostInformation::PROXY:
		{
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No connection matched to " << handle ;
				return false;
			}

			ConnectionInterface *conn = mapHandleConnectionInterface_[handle];
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::TcpClient);
			
			return conn->RemoveInterface(inet);
			break;
		}
		case HostInformation::STUB:
		{
			// TODO: What if serverlistener is down, but connections are still up?
			if(mapHandleServerListener_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No server listener matched to " << handle ;
				return false;
			}

			ServerListenerInterface *server= mapHandleServerListener_[handle];
			bool ret = server->RemoveInterface(inet);
			if(!ret)
				IDEBUG() << "WARNING! Could not add stub to serverListener!" ;

			if(mapServerConnectionSet_.count(server) > 0)
			{
				const SetConnection &setConn = mapServerConnectionSet_[server];
				for(SetConnection::const_iterator it = setConn.begin(), it_end = setConn.end(); it != it_end; ++it)
				{
					ConnectionInterface *conn = *it;
					ASSERT(conn);

					conn->RemoveInterface(inet);
				}
			}
			else
			{
				IDEBUG() << " No connections on server " << server->GetInterfaceHandle() ;
			}
			return true;
			break;
		}
		case HostInformation::PUBLISHER:
		{
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No connection matched to " << handle ;
				return false;
			}

			ConnectionInterface *conn = mapHandleConnectionInterface_[handle];
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::UdpSender);
			
			return conn->RemoveInterface(inet);
			break;
		}
		case HostInformation::SUBSCRIBER:
		{
			if(mapHandleConnectionInterface_.count(handle) <= 0)
			{
				IDEBUG() << "WARNING! No connection matched to " << handle ;
				return false;
			}

			ConnectionInterface *conn = mapHandleConnectionInterface_[handle];
			ASSERT(conn);
			ASSERT(conn->GetConnectionType() == ConnectionInterface::UdpReceiver);
			
			return conn->RemoveInterface(inet);
			break;
		}
		default:
			IDEBUG() << "ERROR! Could not identify: " << inet->GetInterfaceType() ;
			break;
	}

	return false;
}

// -----------------------------------------------------------------------------------
// provided callback from INetServerListener object to add a new socketDescriptor
// -----------------------------------------------------------------------------------
ConnectionInterface* INetConnectionManager::AddConnection(int socketDescriptor, ServerListenerInterface *server)
{
	MutexLocker lock(&mutex_);
	
	if(mapSocketServerListener_.count(socketDescriptor) > 0)
	{
		stringstream ostream;
		ostream << PRETTY_FUNCTION << "ERROR! Stub/proxy socket " << socketDescriptor << " is found among serverlistener sockets!!!!";
		cout << ostream.str() ;
		throw Exception(ostream.str());
	}
	
	// -- start debug --
	ASSERT(socketDescriptor > 0);
	ASSERT(mapSocketServerListener_.count(socketDescriptor) <= 0);
	ASSERT(mapHandleServerListener_.count(server->GetInterfaceHandle()) > 0);
	// -- end debug --

	// create INetConnection
    TcpSocket::Ptr tcpSocket(new TcpSocket(socketDescriptor));
	INetConnection *conn = addConnection(tcpSocket, ConnectionInterface::TcpServer);
	
	// -- start add connection to ServerListener set of connections --
	if(mapServerConnectionSet_.count(server) <= 0)
	{
		mapServerConnectionSet_.insert(pair<ServerListenerInterface*, SetConnection>(server, SetConnection()));
	}
	ASSERT(mapServerConnectionSet_[server].count(conn) <= 0);
	mapServerConnectionSet_[server].insert(conn);
	// -- end add connection to ServerListener set of connections --
	
	return conn;
}

// -----------------------------------------------------------------------------------
// provided callback from INetConnection object to add its connection upon reconnection
// -----------------------------------------------------------------------------------
void INetConnectionManager::AddConnection(int socketDescriptor, ConnectionInterface *conn)
{
	MutexLocker lock(&mutex_);
	
	if(mapSocketServerListener_.count(socketDescriptor) > 0)
	{
		stringstream ostream;
		ostream << PRETTY_FUNCTION << "ERROR! Stub/proxy socket " << socketDescriptor << " is found among connection sockets!!!!";
		cout << ostream.str() ;
		throw Exception(ostream.str());
	}
	
	ASSERT(socketDescriptor > 0);
	ASSERT(mapSocketConnectionInterface_.count(socketDescriptor) <= 0);
	//ASSERT(mapHandleConnectionInterface_.count(conn->GetInterfaceHandle()) <= 0);

	// the connection object is added
	if(mapSocketConnectionInterface_.count(socketDescriptor) <= 0)
	{
		IDEBUG() << "Added connection on socket " << socketDescriptor ;
		mapSocketConnectionInterface_[socketDescriptor] = conn;
	}
	if(mapHandleConnectionInterface_.count(conn->GetInterfaceHandle()) <= 0)
	{
		IDEBUG() << "Added connection on handle " << conn->GetInterfaceHandle() ;
		mapHandleConnectionInterface_[conn->GetInterfaceHandle()] = conn;
	}
}

//--------------------------------------------------------------------------------------
// 		callback remove functions
//--------------------------------------------------------------------------------------
void INetConnectionManager::RemoveServerListener(int socketDescriptor, ServerListenerInterface *server)
{
	MutexLocker lock(&mutex_);
	
	ASSERT(server);
	//ASSERT(socketDescriptor > 0);
	//server->Stop(true);

	if(socketDescriptor >= 0)
	{
		if(mapSocketServerListener_.count(socketDescriptor) <= 0)
			IDEBUG() << "ERROR! ServerListenerInterface socket " << socketDescriptor << " not found!" ;
		ASSERT(mapSocketServerListener_.count(socketDescriptor) == 1);
		mapSocketServerListener_.erase(socketDescriptor);
	
		if(mapHandleServerListener_.count(server->GetInterfaceHandle()) <= 0)
			IDEBUG() << "ERROR! ServerListenerInterface handle " << server->GetInterfaceHandle() << " not found!" ;
		ASSERT(mapHandleServerListener_.count(server->GetInterfaceHandle()) == 1);
		mapHandleServerListener_.erase(server->GetInterfaceHandle());
	}
	
	ASSERT(setInvalidServers_.count(server) <= 0);
	setInvalidServers_.insert(server);
}

// callback remove functions
void INetConnectionManager::RemoveConnection(int socketDescriptor, ConnectionInterface *connection)
{
	MutexLocker lock(&mutex_);

	ASSERT(connection);
	ASSERT(socketDescriptor > 0);

	//TODO: should I stop?
	// connection->Stop();
	
	if(mapSocketConnectionInterface_.count(socketDescriptor) <= 0)
		IDEBUG() << "ERROR! StubInterface socket " << socketDescriptor << " not found!" ;

	ASSERT(mapSocketConnectionInterface_.count(socketDescriptor) == 1);

	mapSocketConnectionInterface_.erase(socketDescriptor);
}

//--------------------------------------------------------------------------------------
// 		private functions
// 		- not thread safe
// 		- connect to server and return TcpSocket
//--------------------------------------------------------------------------------------
TcpSocket* INetConnectionManager::connectToHost(const InterfaceHandle &handle)
{
	for(int i = 0; i < 3; i++)
	{
		TcpSocket *tcpSocket = NULL;

		try
		{
			tcpSocket = new TcpSocket();

			bool connected = tcpSocket->connectToHost(handle.GetHostName(), handle.GetPortNumber());
			if(!connected)
			{
				throw Exception("ERROR! connecting!");
			}

			return tcpSocket;
		}
		catch(Exception ex)
		{
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << handle ;
			if(tcpSocket) delete tcpSocket;
		}
		catch(...)
		{
			IDEBUG() << "ERROR! connecting to " << handle ;
			if(tcpSocket) delete tcpSocket;
		}
	}

	return NULL;
}

// -- not thread safe
INetConnection* INetConnectionManager::addConnection(TcpSocket::Ptr tcpSocket, ConnectionInterface::ConnectionType type)
{
	int socketDescriptor = tcpSocket->socketDescriptor();

	ASSERT(mapSocketServerListener_.count(socketDescriptor) <= 0);
	ASSERT(mapSocketConnectionInterface_.count(socketDescriptor) <= 0);

	SocketStream *socketStream = NULL;
	INetConnection *connection = NULL;

	try
	{
		socketStream = new SocketStream(tcpSocket);
		
		// TODO: Which handle should I use?
		InterfaceHandle handle(tcpSocket->peerAddress(), tcpSocket->peerPort());

		IDEBUG() << " Adding handle " << handle ;

		connection = new INetConnection(socketStream, handle, type, true);
		
		ASSERT(mapSocketConnectionInterface_.count(socketDescriptor) <= 0);
		mapSocketConnectionInterface_[socketDescriptor] = connection;
		
		ASSERT(mapHandleConnectionInterface_.count(handle) <= 0);
		mapHandleConnectionInterface_[handle] = connection;
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! Exception caught! " << ex.msg() ;
		
		if(connection) delete connection;
		if(socketStream) delete socketStream;

		connection = NULL;
		socketStream = NULL;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Unhandled cxception caught!" ;
		if(socketStream) delete socketStream;
		if(connection) delete connection;

		connection = NULL;
		socketStream = NULL;
	}

	return connection;
}

//--------------------------------------------------------------------------------------
//          not thread safe: get or create connection
//--------------------------------------------------------------------------------------
INetConnection* INetConnectionManager::getOrCreateConnection(const InterfaceHandle &handle, ConnectionInterface::ConnectionType type)
{
	INetConnection *conn = getConnection(handle);
	if(conn) return conn;  // TODO: Check if found connection is of ConnectionType == type! 
	
	conn = new INetConnection(handle, type, true);
	ASSERT(mapHandleConnectionInterface_.count(handle) <= 0);

    // FIXME: InterfaceHandle may have invalid port-number == -1, which is valid to let OS pick available port!
    // TODO: INetConnection is already using a call-back from INetConnection to INetConnectionManager to AddConnection(handle, conn)

	mapHandleConnectionInterface_.insert(pair<InterfaceHandle, ConnectionInterface*>(handle, conn));

	return conn;
}

//--------------------------------------------------------------------------------------
//           not thread safe: get existing connection 
//--------------------------------------------------------------------------------------
INetConnection* INetConnectionManager::getConnection(const InterfaceHandle &handle)
{
	ASSERT(handle.IsValid());

	for(MapHandleConnectionInterface::iterator it = mapHandleConnectionInterface_.begin(), it_end = mapHandleConnectionInterface_.end(); it != it_end; ++it)
	{
		const InterfaceHandle &connHandle = it->first;
        ConnectionInterface *conn = it->second;

        // -- debug --
		ASSERT(conn);
		ASSERT(conn->GetInterfaceHandle() == connHandle);
        // -- debu g--

		if(conn->GetInterfaceHandle() == handle)
		{
			ASSERT(conn->IsConnected());

			INetConnection* server = dynamic_cast<INetConnection*>(conn);
			ASSERT(server);
			return server;
		}
	}
	
	return NULL;
}

ConnectionInterface* INetConnectionManager::removeConnection(const InterfaceHandle &handle)
{
	ASSERT(handle.IsValid());

	bool foundSocket = false;
	for(MapSocketConnectionInterface::iterator it = mapSocketConnectionInterface_.begin(), it_end = mapSocketConnectionInterface_.end(); it != it_end; ++it)
	{
		ConnectionInterface *conn = it->second;
		ASSERT(conn);
		
		if(conn->GetInterfaceHandle() == handle)
		{
			foundSocket = true;
			mapSocketConnectionInterface_.erase(it);
			break;
		}
	}

	ConnectionInterface *inetConnection = NULL;
	for(MapHandleConnectionInterface::iterator it = mapHandleConnectionInterface_.begin(), it_end = mapHandleConnectionInterface_.end(); it != it_end; ++it)
	{
		const InterfaceHandle &connHandle = it->first;
		ConnectionInterface *conn = it->second;
		ASSERT(conn);
		ASSERT(conn->GetInterfaceHandle() == connHandle);

		if(conn->GetInterfaceHandle() == handle)
		{
			inetConnection = conn;
			mapHandleConnectionInterface_.erase(it);
			break;
		}
	}

	// -- start debug --
	if(foundSocket) ASSERT(inetConnection);
	// -- end debug --

	return inetConnection;
}


} // namespace MicroMiddleware

