#include"MicroMiddleware/Net/RmiServerListener.h"
#include"MicroMiddleware/Net/RmiServer.h"
#include"MicroMiddleware/Net/RmiSslServer.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/Runtime.h"

namespace MicroMiddleware
{
RmiServerListener::RmiServerListener(const InterfaceHandle &interfaceHandle, bool autoStart, RmiBase::CommMode mode) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, commMode_(mode)
	, tcpServer_(NULL)
{
	if(commMode_ != RmiBase::UnencryptedMode && commMode_ != RmiBase::SslServerMode)
	{
		throw("RmiServerListener::RmiServerListener(): Illegal RmiBase::CommMode given in constructor!");
	}

	if(autoStart) start();
}

RmiServerListener::RmiServerListener(const string &serverName, const InterfaceHandle &interfaceHandle, bool autoStart, RmiBase::CommMode mode) 
	: serverName_(serverName)
	, runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, commMode_(mode)
	, tcpServer_(NULL)
{
	if(commMode_ != RmiBase::UnencryptedMode && commMode_ != RmiBase::SslServerMode)
	{
		throw("RmiServerListener::RmiServerListener(): Illegal RmiBase::CommMode given in constructor!");
	}

	if(autoStart) start();
}


RmiServerListener::~RmiServerListener()
{
	MutexLocker lock(&mutex_);
	cleanUp();
}

bool RmiServerListener::WaitStarted()
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

bool RmiServerListener::initServer()
{
	if(tcpServer_ == NULL)
		tcpServer_ = new TcpServer();

	bool ret = tcpServer_->listen(string(""), interfaceHandle_.GetPortNumber());
	if(!ret)
	{
		IDEBUG() << "ERROR! Couldnt listen to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber() ;
		return false;
	}
	else
	{
		interfaceHandle_.SetPortNumber(tcpServer_->GetPortNumber());
		interfaceHandle_.SetHostName(tcpServer_->GetHostName());
	}

	if(serverName_.empty())
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

	return true;
}

void RmiServerListener::run()
{
	try
	{
		mutex_.lock();
			runThread_ = initServer();
		waitCondition_.wakeAll();
		mutex_.unlock();
		
		if(runThread_) ServerStartedListening();

		while(runThread_)
		{
			IDEBUG() << "Wait for connections on " << interfaceHandle_ ;

			int socketDescriptor = tcpServer_->waitForIncomingConnection();
			if(socketDescriptor <= -1)
			{
				IDEBUG() << "ERROR! Tcp server wait for incoming connection returned " << socketDescriptor ;
				break;
			}

			MutexLocker lock(&mutex_);
			
			removeOldServers();

			IDEBUG() << "New connection on socket " << socketDescriptor ;
			
			switch(commMode_)
			{
				case RmiBase::UnencryptedMode:
				{
					RmiServer *rmiServer = new RmiServer(this, socketDescriptor);
					if(mapRmiServer_.count(socketDescriptor) > 0)
						IDEBUG() << "ERROR! Socket already present in map! " ;
					mapRmiServer_.insert(pair<int, RmiServer*>(socketDescriptor, rmiServer));
					break;
				}
#ifdef SSL_SUPPORT
				case RmiBase::SslClientMode:
				{
					IDEBUG() << "WARNING! Server cannot be in client mode! Fall-back to servermode." ;
				}
				case RmiBase::SslServerMode:
				{
					RmiSslServer *rmiSslServer = new RmiSslServer(this, socketDescriptor);
					if(mapRmiServer_.count(socketDescriptor) > 0)
						IDEBUG() << "ERROR! Socket already present in map! " ;
					mapRmiServer_.insert(pair<int, RmiSslServer*>(socketDescriptor, rmiSslServer));
					break;
				}
#endif
				default:
					IWARNING()  << "ERROR! Unknown communication mode ! " << commMode_ ;
					break;
			}
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

	ServerStoppedListening();
}

void RmiServerListener::RMInvocation(int methodId, RmiBase *rmiServerBase)
{
	switch(methodId)
	{
		// TODO add middleware exception handling etc.....
		default:
			IDEBUG() << "ERROR! function id not supported : " << methodId ;
			break;	
	}
}

void RmiServerListener::RemoveServer(int socket)
{
	MutexLocker lock(&mutex_);

	ASSERT(socket > 0);
	ASSERT(removeRmiServer_.count(socket) <= 0);
	
	removeRmiServer_.insert(socket);
	//mapRmiServer_.erase(socket);
}

void RmiServerListener::StartOrRestartServer(const InterfaceHandle &interfaceHandle)
{
	MutexLocker lock(&mutex_);
	
	interfaceHandle_ = interfaceHandle;
	startOrRestartServer();
}

void RmiServerListener::StopServer(bool waitForTermination)
{
	MutexLocker lock(&mutex_);
	if(isRunning())
	{
		runThread_ = false;
		tcpServer_->close();
		if(waitForTermination)
			wait();

		cleanUp();
	}
}

void RmiServerListener::startOrRestartServer()
{
	if(isRunning())
	{
		runThread_ = false;
		tcpServer_->close();
		wait();
	}

	cleanUp();
	
	runThread_ = true;
	//start();
}

void RmiServerListener::removeOldServers()
{
	if(removeRmiServer_.empty()) return;

	for(set<int>::iterator it = removeRmiServer_.begin(), it_end = removeRmiServer_.end(); it != it_end; ++it)
	{
		int socket = *it;
		if(mapRmiServer_.count(socket) <= 0)
			IDEBUG() << "WARNING! RmiServer for socket " << socket << " removed!" ;

		ASSERT(mapRmiServer_.count(socket) > 0);

		RmiBase *rmiBase = mapRmiServer_[socket];
		ASSERT(rmiBase);

		mapRmiServer_.erase(socket);

#ifdef SSL_SUPPORT
		// TODO: dynamic casting! Better way?
		RmiServer *rmiServer = dynamic_cast<RmiServer*>(rmiBase);
		if(rmiServer)
		{
			IDEBUG() << " Deleting rmi server !" ;
			delete rmiServer;
			continue;
		}

		RmiSslServer *rmiSslServer = dynamic_cast<RmiSslServer*>(rmiBase);
		ASSERT(rmiSslServer);
		delete rmiSslServer;
#endif
	}

	removeRmiServer_.clear();
}

void RmiServerListener::cleanUp()
{
	runThread_ = false;
	
	if(tcpServer_)
	{
		tcpServer_->close();
		delete tcpServer_;
		tcpServer_ = NULL;
	}

	for(MapRmiServer::iterator it = mapRmiServer_.begin(), it_end = mapRmiServer_.end(); it != it_end; ++it)
	{
		RmiBase *rmiBase = it->second;

#ifdef SSL_SUPPORT
		// TODO: dynamic casting! Better way?
		RmiServer *rmiServer = dynamic_cast<RmiServer*>(rmiBase);
		if(rmiServer)
		{
			delete rmiServer;
			continue;
		}

		RmiSslServer *rmiSslServer = dynamic_cast<RmiSslServer*>(rmiBase);
		ASSERT(rmiSslServer);
		delete rmiSslServer;
#endif
	}

	mapRmiServer_.clear();
	removeRmiServer_.clear();
}

}; // namespace MicroMiddleware


