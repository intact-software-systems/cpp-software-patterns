#include"MicroMiddleware/Net/RmiClient.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/Runtime.h"

namespace MicroMiddleware
{

RmiClient::RmiClient(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, tcpSocket_()
	, socketDescriptor_(-1)
{
	if(autoStart) start();
}

RmiClient::RmiClient(const std::string &clientName, const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, clientName_(clientName)
	, tcpSocket_()
	, socketDescriptor_(-1)
{
	if(autoStart) start();
}


RmiClient::RmiClient(int socketDescriptor, bool autoStart)
	: runThread_(true)
	, tcpSocket_()
	, socketDescriptor_(socketDescriptor)
{
	if(autoStart) start();
}

RmiClient::~RmiClient()
{
	if(isRunning()) StopClient();
}

bool RmiClient::initTcpSocket()
{
	MutexLocker lock(&mutex_);

	if(!tcpSocket_)
		tcpSocket_ = TcpSocket::Ptr( new TcpSocket() );

	bool connected = true;
	if(socketDescriptor_ > -1)
	{
		IWARNING()  << " initializing TcpSocket with descriptor : " << socketDescriptor_ ;
		
		tcpSocket_->setSocketDescriptor(socketDescriptor_);
		interfaceHandle_ = InterfaceHandle(tcpSocket_->peerAddress(), tcpSocket_->peerPort());
		InitSocketObjects(tcpSocket_);
		ASSERT(IsConnected());
	}
	else
	{
		connected = connectToHost();
	}

	if(clientName_.empty() == true)
	{
		stringstream name;
		name << "MicroMiddleware.RmiClient." << Thread::threadId() << "." << interfaceHandle_;
		clientName_ = name.str();
	}
	
	clientInformation_ = Runtime::GetHostInformation();
	clientInformation_.SetComponentName(clientName_);
	clientInformation_.SetHostDescription(HostInformation::PROXY);
	clientInformation_.SetPortNumber(interfaceHandle_.GetPortNumber());
	if(clientInformation_.GetHostName().empty())
		clientInformation_.SetHostName(interfaceHandle_.GetHostName());
	
	if(connected)
		clientInformation_.SetOnlineStatus(HostInformation::ONLINE);
	else
		clientInformation_.SetOnlineStatus(HostInformation::OFFLINE);
	
	SetConnection(clientInformation_);

	return connected;
}

void RmiClient::run()
{
	try
	{
		bool connected = initTcpSocket();
		IWARNING()  << "RMI Client started " << interfaceHandle_ ;
		
		mutex_.lock();
		
		while(runThread_)
		{
			bool prevState = connected;
			if((connected = connectToHost()) == false)
				clientInformation_.SetOnlineStatus(HostInformation::OFFLINE);
			else
				clientInformation_.SetOnlineStatus(HostInformation::ONLINE);
			
			if(prevState != connected)
				SetConnection(clientInformation_);

			rmiMonitor_.wait(&mutex_, 1000);
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! RMI Client exception! " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! RMI Client exception! " ;
	}

	clientInformation_.SetOnlineStatus(HostInformation::OFFLINE);
	SetConnection(clientInformation_);

	mutex_.unlock();
	
	IDEBUG() << "RMI Client stopped" ;
}

void RmiClient::StopClient(bool waitForTermination)
{
	mutex_.lock(); 
		try
		{
			runThread_ = false;
			if(tcpSocket_) tcpSocket_->close();
			
			rmiMonitor_.wakeAll();
		}
		catch(...)
		{
			IDEBUG() << " Exception caught!" ;
		}
	mutex_.unlock();

	if(waitForTermination)
		wait();
}

void RmiClient::StartClient()
{
	MutexLocker lock(&mutex_);
	if(isRunning() == false)
	{
		runThread_ = true;
		start();
	}
	else
	{
		rmiMonitor_.wakeAll();
	}
}

void RmiClient::StartOrRestartClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&mutex_);
	
	if(isRunning() == false)
	{
		runThread_ = true;
		start();
	}

	if(handle == interfaceHandle_) 
	{
		IDEBUG() << "WARNING! new " << handle << " == " << interfaceHandle_ << ". Returning!" ;
	}
	else
	{
		if(tcpSocket_) tcpSocket_->close();
		interfaceHandle_ = handle;
	}

	rmiMonitor_.wakeAll();
}

void RmiClient::StartOrRestartClient()
{
	MutexLocker lock(&mutex_);
	
	if(tcpSocket_)	tcpSocket_->close();
	
	if(isRunning() == false)
	{
		runThread_ = true;
		start();
	}
	else
	{
		rmiMonitor_.wakeAll();
	}
}

bool RmiClient::WaitConnected()
{
	try
	{
		mutex_.lock();
		while(IsConnected() == false)
		{
			rmiMonitor_.wait(&mutex_, 3000);
			IDEBUG() << " Client is still not connected!" ;
		}
	}
	catch(...)
	{
		IDEBUG() << " ERROR while waiting for connection on " << interfaceHandle_ ;
	}

	mutex_.unlock();
	return IsConnected();
}

bool RmiClient::ConnectToHost()
{
	MutexLocker lock(&mutex_);
	return connectToHost();
}

bool RmiClient::connectToHost()
{
	if(!tcpSocket_)
	{
		IDEBUG() << "WARNING! Thread must be started before attempting to connect!" ;
		return false;
	}

	bool connected = IsConnected();
	if(connected == false)
	{
		try
		{
			//IDEBUG() << "Connecting to " << interfaceHandle_;
			tcpSocket_->close();
			connected = tcpSocket_->connectToHost(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			if(connected == false)
			{
				throw ("ERROR! connecting!");
			}

			IDEBUG() << "Connected to " << interfaceHandle_;
			InitSocketObjects(tcpSocket_);
			ASSERT(IsConnected());

			rmiMonitor_.wakeAll();
		}
		catch(Exception ex)
		{
			CleanUp();
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_ ;
		}
		catch(...)
		{
			//IDEBUG() << "ERROR! connecting to " << interfaceHandle_ ;
			CleanUp();
		}
	}

	return connected;
}

RmiClient* RmiClient::FindProxy(const InterfaceHandle &handle, const SetRmiClient *setRmiClient)
{
	if(setRmiClient == NULL) return NULL;

	for(SetRmiClient::const_iterator it = setRmiClient->begin(), it_end = setRmiClient->end(); it != it_end; ++it)
	{
		if((*it)->GetInterfaceHandle() == handle) return (*it);
	}
	return NULL;
}

void RmiClient::AddTarget(InterfaceHandle &handle)
{
	IWARNING()  << " adding target : " << handle ;
	ExecuteOneWayRMI(RmiInterface::AddTarget, &handle);
}

void RmiClient::RemoveTarget(InterfaceHandle &handle)
{
	ExecuteOneWayRMI(RmiInterface::RemoveTarget, &handle);
}

} // namespace MicroMiddleware

