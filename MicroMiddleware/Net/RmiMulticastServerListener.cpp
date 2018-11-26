#include "MicroMiddleware/Net/RmiMulticastServerListener.h"
#include "MicroMiddleware/Net/RmiMulticastServer.h"
#include "MicroMiddleware/RmiInterface.h"

namespace MicroMiddleware
{

RmiMulticastServerListener::RmiMulticastServerListener(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(autoStart),
	interfaceHandle_(interfaceHandle),
	tcpServer_(NULL), 
	rmiMulticastClient_(NULL)
{
	if(runThread_) start();
}

RmiMulticastServerListener::~RmiMulticastServerListener()
{
	MutexLocker lock(&clientMutex_);
	cleanUp();
}

void RmiMulticastServerListener::initRmiMulticastServerListener()
{
	if(tcpServer_ == NULL)
		tcpServer_ = new TcpServer();

    //runThread_ = tcpServer_->listen(interfaceHandle_.GetHostName(), interfaceHandle_.GetPortNumber());
    runThread_ = tcpServer_->listen("", interfaceHandle_.GetPortNumber());
	if(runThread_ == false)
		IDEBUG() << "ERROR! Couldnt listen to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber() ;
}	


void RmiMulticastServerListener::run()
{
	try
	{
		clientMutex_.lock();
			initRmiMulticastServerListener();
		clientMutex_.unlock();
		
		while(runThread_)
		{
			IDEBUG() << "ERROR! Does not work anymore!" ;

			/*bool newConnection = tcpServer_->waitForNewConnection(-1);
			if(newConnection == false || runThread_ == false) break;

			TcpSocket *tcpSocket = (TcpSocket*)tcpServer_->nextPendingConnection();
			if(tcpSocket == NULL)
			{
				IDEBUG() << "ERROR! Tcp socket next pending connection returned tcpSocket == NULL!" ;
				break;
			}
			
			MutexLocker lock(&clientMutex_);
			
			IDEBUG() << "New connection from " << tcpSocket->peerAddress().toString().toStdString() ;
			RmiMulticastServer *rmiServer = new RmiMulticastServer(this, tcpSocket);
			
			if(mapRmiServer_.count(tcpSocket->socketDescriptor()) > 0)
				IDEBUG() << "ERROR! Socket already present in map! " ;

			mapRmiServer_.insert(pair<int, RmiMulticastServer*>(tcpSocket->socketDescriptor(), rmiServer));
			*/
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
}

void RmiMulticastServerListener::addMulticastClient(const InterfaceHandle &handle)
{
	if(rmiMulticastClient_ == NULL) 
	{
		IDEBUG() << "WARNING! No rmiMulticastClient defined for server " << interfaceHandle_ ;
		return;
	}

	rmiMulticastClient_->StartAndAddClient(handle);
}

void RmiMulticastServerListener::removeMulticastClient(const InterfaceHandle &handle)
{
	if(rmiMulticastClient_ == NULL) 
	{
		IDEBUG() << "WARNING! No rmiMulticastClient defined for server " << interfaceHandle_ ;
		return;
	}

	rmiMulticastClient_->StopAndRemoveClient(handle);
}

void RmiMulticastServerListener::RMInvocation(int methodId, RmiMulticastServer *rmiServer)
{
	MutexLocker lock(&clientMutex_);

	IDEBUG() << " receiving method id " << methodId ;

	switch(methodId)
	{
		case RmiInterface::AddTarget:
			{
				InterfaceHandle handle;
				rmiServer->ReceiveRMI(&handle);
				
				IDEBUG() << " adding client " << handle ;
				addMulticastClient(handle);
				break;
			}
		case RmiInterface::RemoveTarget:
			{
				InterfaceHandle handle;
				rmiServer->ReceiveRMI(&handle);

				IDEBUG() << " removing client " << handle ;
				removeMulticastClient(handle);
				break;
			}
		default:
			IDEBUG() << "ERROR! function id not supported : " << methodId ;
			break;	
	}
}

void RmiMulticastServerListener::AddTarget(InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	for(MapRmiServer::iterator it = mapRmiServer_.begin(), it_end = mapRmiServer_.end(); it != it_end; ++it)
	{
		RmiMulticastServer *rmiServer = it->second;
		rmiServer->AddTarget(handle);
	}
}

void RmiMulticastServerListener::RemoveTarget(InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	for(MapRmiServer::iterator it = mapRmiServer_.begin(), it_end = mapRmiServer_.end(); it != it_end; ++it)
	{
		RmiMulticastServer *rmiServer = it->second;
		rmiServer->RemoveTarget(handle);
	}
}

void RmiMulticastServerListener::RemoveServer(int socketDescriptor)
{
	MutexLocker lock(&clientMutex_);
	mapRmiServer_.erase(socketDescriptor);
}

void RmiMulticastServerListener::cleanUp()
{
	for(MapRmiServer::iterator it = mapRmiServer_.begin(), it_end = mapRmiServer_.end(); it != it_end; ++it)
	{
		RmiMulticastServer *rmiServer = it->second;
		delete rmiServer;
	}
}
}; // namespace end


