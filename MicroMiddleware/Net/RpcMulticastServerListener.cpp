#include "MicroMiddleware/Net/RpcMulticastServerListener.h"
#include "MicroMiddleware/Net/RpcMulticastServer.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/Net/RpcMulticastClient.h"
#include "MicroMiddleware/Runtime.h"

namespace MicroMiddleware
{

RpcMulticastServerListener::RpcMulticastServerListener(const InterfaceHandle &interfaceHandle, RpcMulticastClient *client, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, tcpServer_(NULL)
	, multicastClient_(client)
{
	if(autoStart) start();
}

RpcMulticastServerListener::~RpcMulticastServerListener()
{
	MutexLocker lock(&mutex_);
	cleanUp();
}

void RpcMulticastServerListener::StopServer()
{
	MutexLocker lock(&mutex_);
	runThread_ = false;
	tcpServer_->close();
}

bool RpcMulticastServerListener::WaitStarted()
{
	mutex_.lock();
	if(isRunning() && interfaceHandle_.IsValid()) 
	{
		mutex_.unlock();
		return true;
	}

	waitCondition_.wait(&mutex_);
	ASSERT(isRunning());
	bool ret = interfaceHandle_.IsValid();
	
	mutex_.unlock();
	return ret;
}

bool RpcMulticastServerListener::initServer()
{
	if(tcpServer_ == NULL)
		tcpServer_ = new TcpServer();

	bool ret = tcpServer_->listen(string(""), interfaceHandle_.GetPortNumber());
	if(ret == false)
	{
		IDEBUG() << "ERROR! Couldnt listen to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber() ;
		return false;
	}
	else
	{
		interfaceHandle_.SetPortNumber(tcpServer_->GetPortNumber());
		interfaceHandle_.SetHostName(tcpServer_->GetHostName());
	}

	if(serverName_.empty() == true)
	{
		stringstream name;
		name << "MicroMiddleware.RmiServerListener." << Thread::threadId() << "." << interfaceHandle_;
		serverName_ = name.str();
	}
	
	serverInformation_ = Runtime::GetHostInformation();
	serverInformation_.SetComponentName(serverName_);
	serverInformation_.SetHostDescription(HostInformation::STUB);
	serverInformation_.SetPortNumber(interfaceHandle_.GetPortNumber());
	if(serverInformation_.GetHostName().empty())
		serverInformation_.SetHostName(interfaceHandle_.GetHostName());

	for(MapInterfaceIdRpcServer::iterator it = mapInterfaceIdRpcServer_.begin(), it_end = mapInterfaceIdRpcServer_.end(); it != it_end; ++it)
	{
		RpcServer *server = it->second;
		ASSERT(server);
		server->SetServerInformation(serverInformation_);
		server->ServerStartedListening();
	}

	return true;
}

void RpcMulticastServerListener::run()
{
	try
	{
		mutex_.lock();
			runThread_ = initServer();
		waitCondition_.wakeAll();
		mutex_.unlock();
		
		while(runThread_)
		{
			IDEBUG() << "Wait for connections" ;

			int socketDescriptor = tcpServer_->waitForIncomingConnection();
			if(socketDescriptor <= -1)
			{
				IDEBUG() << "ERROR! Tcp server wait for incoming connection returned " << socketDescriptor ;
				break;
			}

			MutexLocker lock(&mutex_);
			
			if(runThread_ == false) break;
			
			removeOldServers();

			IDEBUG() << "New connection on socket " << socketDescriptor ;
			
			RpcMulticastServer *rmiServer = new RpcMulticastServer(this, socketDescriptor);
			
			if(mapRpcMulticastServer_.count(socketDescriptor) > 0)
				IDEBUG() << "ERROR! Socket already present in map! " ;

			mapRpcMulticastServer_.insert(pair<int, RpcMulticastServer*>(socketDescriptor, rmiServer));
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! Exception caught! " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Exception caught! " ;
	}

	mutex_.lock();
	cleanUp();
	waitCondition_.wakeAll();
	mutex_.unlock();
}

// -> called from RpcMulticastServer
void RpcMulticastServerListener::AddTarget(GroupHandle &handle)
{
	MutexLocker lock(&mutex_);
	
	if(multicastClient_ == NULL)
	{
		IDEBUG() << "WARNING! multicast client == NULL! Returning!" ;
		return;
	}

	multicastClient_->AddMulticastClient(handle);
}

// -> called from RpcMulticastServer
void RpcMulticastServerListener::RemoveTarget(GroupHandle &handle)
{
	MutexLocker lock(&mutex_);

	if(multicastClient_ == NULL)
	{
		IDEBUG() << "WARNING! multicast client == NULL! Returning!" ;
		return;
	}

	multicastClient_->RemoveMulticastClient(handle);
}

// -> called from RpcMulticastServer
void RpcMulticastServerListener::RPCall(const RpcIdentifier &rpc, RpcMulticastServer *rmiServer)
{
	MutexLocker lock(&mutex_);

	//IDEBUG() << " RpcIdentifier " << rpc ;

	switch(rpc.GetMethodId())
	{
		// TODO: Anything?
		default:
		{
			if(mapInterfaceIdRpcServer_.count(rpc.GetInterfaceId()) <= 0)
				IDEBUG() << "ERROR! rpc server to interface id " << rpc.GetInterfaceId() << " is not present!" ;
			
			ASSERT(mapInterfaceIdRpcServer_.count(rpc.GetInterfaceId()) > 0);
			
			RpcServer *rpcServer = mapInterfaceIdRpcServer_[rpc.GetInterfaceId()];
			ASSERT(rpcServer != NULL);

			rpcServer->RPCall(rpc, rmiServer);
		}
	}
}

bool RpcMulticastServerListener::RegisterInterface(int interfaceId, RpcServer *rpcServer)
{
	MutexLocker lock(&mutex_);

	if(mapInterfaceIdRpcServer_.count(interfaceId) > 0)
	{
		IDEBUG() << "ERROR! rpc server to interface id " << interfaceId << " is already present!" ;
		return false;
	}

	if(isRunning() && interfaceHandle_.IsValid())
	{
		rpcServer->SetServerInformation(serverInformation_);
		rpcServer->ServerStartedListening();
	}

	mapInterfaceIdRpcServer_.insert(pair<int, RpcServer*>(interfaceId, rpcServer));
	return true;
}

void RpcMulticastServerListener::RemoveServer(int socket)
{
	MutexLocker lock(&mutex_);

	ASSERT(socket > 0);
	ASSERT(removeRpcMulticastServer_.count(socket) <= 0);
	
	removeRpcMulticastServer_.insert(socket);
}


void RpcMulticastServerListener::SetMulticastClient(RpcMulticastClient *multicastClient)
{
	MutexLocker lock(&mutex_);
	
	ASSERT(multicastClient_ == NULL);
	multicastClient_ = multicastClient;
}

void RpcMulticastServerListener::removeOldServers()
{
	if(removeRpcMulticastServer_.empty() == true) return;

	for(set<int>::iterator it = removeRpcMulticastServer_.begin(), it_end = removeRpcMulticastServer_.end(); it != it_end; ++it)
	{
		int socket = *it;
		if(mapRpcMulticastServer_.count(socket) <= 0)
			IDEBUG() << "WARNING! RmiServer for socket " << socket << " removed!" ;

		ASSERT(mapRpcMulticastServer_.count(socket) > 0);

		RpcMulticastServer *server = mapRpcMulticastServer_[socket];
		ASSERT(server);
		delete server;

		mapRpcMulticastServer_.erase(socket);
	}

	removeRpcMulticastServer_.clear();
}

void RpcMulticastServerListener::cleanUp()
{
	if(tcpServer_) 
	{
		delete tcpServer_;
		tcpServer_ = NULL;
	}

	for(MapRpcMulticastServer::iterator it = mapRpcMulticastServer_.begin(), it_end = mapRpcMulticastServer_.end(); it != it_end; ++it)
	{
		RpcMulticastServer *rmiServer = it->second;
		rmiServer->Stop();
		rmiServer->wait();

		delete rmiServer;
	}
	mapRpcMulticastServer_.clear();

	for(MapInterfaceIdRpcServer::iterator it = mapInterfaceIdRpcServer_.begin(), it_end = mapInterfaceIdRpcServer_.end(); it != it_end; ++it)
	{
		RpcServer *server = it->second;
		ASSERT(server);
		server->ServerStoppedListening();
	}
	
	mapInterfaceIdRpcServer_.clear();
	removeRpcMulticastServer_.clear();
}

}; // namespace end



