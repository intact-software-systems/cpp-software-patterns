#include "MicroMiddleware/INet/INetServerListener.h"
#include "MicroMiddleware/INet/INetConnectionManager.h"
#include "MicroMiddleware/Runtime.h"
#include "MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{
// -----------------------------------------------
// constructor/destructor
// -----------------------------------------------
INetServerListener::INetServerListener(const InterfaceHandle &interfaceHandle
									, ConnectionManagerInterface *conn
									, bool autoStart) 
	: ServerListenerInterface(interfaceHandle)
	, runThread_(true)
	, socketDescriptor_(-1)
	, tcpServer_(NULL)
	, connectionManager_(conn)
{
	if(autoStart) start();
}

INetServerListener::~INetServerListener()
{
	MutexLocker lock(&mutex_);
	if(tcpServer_)
	{
		delete tcpServer_;
		tcpServer_ = NULL;
	}
}
// -----------------------------------------------
// Start/Stop server
// -----------------------------------------------
void INetServerListener::Stop(bool waitStopped)
{
	mutex_.lock();

	runThread_ = false;
	if(tcpServer_ != NULL)
		tcpServer_->close();

	if(isRunning() && waitStopped)
	{
		mutex_.unlock();
		wait();
		return;
	}

	mutex_.unlock();
}

bool INetServerListener::Start(bool waitConnected)
{
	mutex_.lock();
	
	if(runThread_ == false) // is shutting down wait until it stops before starting
		wait();

	runThread_ = true;
	if(isRunning() == false) 
		start();
	
	if(waitConnected)
	{
		try
		{
			while(socketDescriptor_ <= -1 && runThread_)
			{
				IDEBUG() << " Server is not connected!";
				waitCondition_.wait(&mutex_, 2000);
			}
		}
		catch(...)
		{
			IDEBUG() << " ERROR while waiting for connection on " << interfaceHandle_;
		}
	}

	mutex_.unlock();

	return runThread_;
}

// -----------------------------------------------
// return socket descriptor of server
// -----------------------------------------------
int INetServerListener::socketDescriptor() 
{
	MutexLocker lock(&mutex_);

	if(isRunning() == false) return -1;
	if(runThread_ == false) return -1;
	if(tcpServer_ == NULL) return -1;
	
	return tcpServer_->socketDescriptor();
}

// -----------------------------------------------
// Initialize server
// -----------------------------------------------
bool INetServerListener::initINetServerListener()
{
	MutexLocker lock(&mutex_);
	
	if(tcpServer_ == NULL)
		tcpServer_ = new TcpServer();

	bool connected = tcpServer_->listen(string(""), interfaceHandle_.GetPortNumber());
	if(connected == false)
	{
		IDEBUG() << "ERROR! Couldnt listen to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber();
		socketDescriptor_ = -1;
		delete tcpServer_;
		tcpServer_ = NULL;
	}
	else
	{
		interfaceHandle_.SetPortNumber(tcpServer_->GetPortNumber());
		interfaceHandle_.SetHostName(tcpServer_->GetHostName());
		socketDescriptor_ = tcpServer_->socketDescriptor();

		if(serverName_.empty() == true)
		{
			stringstream name;
			name << "MicroMiddleware.INetServerListener." << Thread::threadId() << "." << interfaceHandle_;
			serverName_ = name.str();
		}

		serverInformation_ = Runtime::GetHostInformation();
		serverInformation_.SetComponentName(serverName_);
		serverInformation_.SetHostDescription(HostInformation::STUB);
		serverInformation_.SetPortNumber(interfaceHandle_.GetPortNumber());
		if(serverInformation_.GetHostName().empty())
			serverInformation_.SetHostName(interfaceHandle_.GetHostName());
	}

	return connected;
}

// -----------------------------------------------
// server thread
// -----------------------------------------------
void INetServerListener::run()
{
	try
	{
		runThread_ = initINetServerListener();
		waitCondition_.wakeAll();

		while(runThread_)
		{
			IDEBUG() << "Wait for connections";

			int socketDescriptor = tcpServer_->waitForIncomingConnection();
			if(socketDescriptor <= -1)
			{
				IDEBUG() << "ERROR! Tcp server wait for incoming connection returned " << socketDescriptor;
				break;
			}

			MutexLocker lock(&mutex_);
			
			if(runThread_ == false) 
			{
                UtilityFunctions::CloseSocket(socketDescriptor);
				break;
			}
			
			IDEBUG() << "New connection on socket " << socketDescriptor;
			ConnectionInterface *conn = connectionManager_->AddConnection(socketDescriptor, this);
			ASSERT(conn);
			
			// add stubs to new connection
			for(MapInterfaceStub::iterator it = mapInterfaceStub_.begin(), it_end = mapInterfaceStub_.end(); it != it_end; ++it)
			{
				INetInterface *stub = it->second;
				
				// -- start check --
				ASSERT(stub);
				ASSERT(stub->IsValid());
				// -- end check --

				conn->RegisterInterface(stub);
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! Exception caught! " << ex.msg();
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Exception caught! ";
	}

	connectionManager_->RemoveServerListener(socketDescriptor_, this);
}

// -----------------------------------------------
// register interface to server
// -----------------------------------------------
bool INetServerListener::RegisterInterface(INetInterface *stub)
{
	ASSERT(interfaceHandle_.IsValid());
	ASSERT(stub->IsValid());

	if(stub->GetInterfaceType() != HostInformation::STUB)
	{
		IDEBUG() << "ERROR! Can only register STUB in ServerListener! " << stub->GetInterfaceIdentifier();
		return false;
	}

	MutexLocker lock(&mutex_);

	if(mapInterfaceStub_.count(stub->GetInterfaceIdentifier()) > 0)
	{
		IDEBUG() << "WARNING! Stub interface already registered: " << stub->GetInterfaceIdentifier();
		return false;
	}
	
	HostInformation info = serverInformation_;
	info.SetOnlineStatus(HostInformation::ONLINE);
	info.SetHostDescription(HostInformation::STUB);
	
	cout << COMPONENT_FUNCTION << info << endl;
	
	stub->SetConnection(info);
	
	mapInterfaceStub_[stub->GetInterfaceIdentifier()] = stub;
	return true;
}

// -----------------------------------------------
// remove interface from server
// -----------------------------------------------
bool INetServerListener::RemoveInterface(INetInterface *stub)
{
	ASSERT(interfaceHandle_.IsValid());
	ASSERT(stub->IsValid());

	if(stub->GetInterfaceType() != HostInformation::STUB)
	{
		IDEBUG() << "ERROR! Can only remove STUB in ServerListener! " << stub->GetInterfaceIdentifier();
		return false;
	}

	MutexLocker lock(&mutex_);

	if(mapInterfaceStub_.count(stub->GetInterfaceIdentifier()) <= 0)
	{
		IDEBUG() << "WARNING! Stub interface not present: " << stub->GetInterfaceIdentifier();
		return false;
	}
	
	HostInformation info = serverInformation_;
	info.SetOnlineStatus(HostInformation::OFFLINE);
	stub->SetConnection(info);

	mapInterfaceStub_.erase(stub->GetInterfaceIdentifier());
	return true;
}

}; // namespace end




