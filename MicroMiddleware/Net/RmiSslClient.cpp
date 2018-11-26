#ifdef SSL_SUPPORT

#include"MicroMiddleware/Net/RmiSslClient.h"
#include"MicroMiddleware/RmiInterface.h"

namespace MicroMiddleware
{

RmiSslClient::RmiSslClient(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: RmiBase(RmiBase::SslClientMode)
	, runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, sslSocket_()
	, socketDescriptor_(-1)
{
	if(autoStart) start();
}

RmiSslClient::RmiSslClient(int socketDescriptor, bool autoStart)
	: RmiBase(RmiBase::SslClientMode)
	, runThread_(true)
	, sslSocket_()
	, socketDescriptor_(socketDescriptor)
{
	if(autoStart) start();
}

void RmiSslClient::initSslSocket()
{
	MutexLocker lock(&clientMutex_);

	if(!sslSocket_)
	{
		sslSocket_ = SslSocket::Ptr(new SslSocket());
		//sslSocket_->ignoreSslErrors();
		//sslSocket_->addCaCertificates("/etc/ssl/certs");	
	}

	if(socketDescriptor_ > -1)
	{
		IWARNING()  << " initializing SslSocket with descriptor : " << socketDescriptor_ << endl;
		
		sslSocket_->setSocketDescriptor(socketDescriptor_);
		interfaceHandle_ = InterfaceHandle(sslSocket_->peerAddress(), sslSocket_->peerPort());
		InitSocketObjects(sslSocket_);
		ASSERT(IsConnected());
	}
}

void RmiSslClient::run()
{
	IWARNING()  << "RMI Client started " << interfaceHandle_ << endl;
	
	try
	{
		initSslSocket();

		lock();

		while(runThread_)
		{
			if(IsConnected())
			{
				rmiMonitor_.wait(&clientMutex_);
				if(runThread_ == false) break;
			}

			while(connectToHost() == false)
			{	
				if(runThread_ == false) break;
				
				unlock();
					sleep(1);
				lock();
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! RMI Client exception! " << ex.msg() << endl;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! RMI Client exception! " << endl;
	}

	unlock();

	IDEBUG() << "RMI Client stopped" << endl;
}

void RmiSslClient::StopClient(bool waitForTermination)
{
	lock(); 
		try
		{
			runThread_ = false;
			if(sslSocket_) sslSocket_->close();
			rmiMonitor_.wakeAll();
		}
		catch(...)
		{
			IDEBUG() << " Exception caught!" << endl;
		}
	unlock();

	if(waitForTermination)
		wait();
}

void RmiSslClient::StartClient()
{
	MutexLocker lock(&clientMutex_);
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

void RmiSslClient::StartOrRestartClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	
	if(sslSocket_) sslSocket_->close();
	
	interfaceHandle_ = handle;
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

void RmiSslClient::StartOrRestartClient()
{
	MutexLocker lock(&clientMutex_);
	
	if(sslSocket_) sslSocket_->close();
	
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

bool RmiSslClient::ConnectToHost()
{
	MutexLocker lock(&clientMutex_);
	return connectToHost();
}

bool RmiSslClient::connectToHost()
{
	if(!sslSocket_)
	{
		IDEBUG() << "WARNING! Thread must be started before attempting to connect!" << endl;
		return false;
	}

	bool connected = IsConnected();
	if(connected == false)
	{
		try
		{
			//IDEBUG() << "Connecting to " << interfaceHandle_<< endl;
			
			connected = sslSocket_->connectToHostEncrypted(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			//connected = sslSocket_->waitForEncrypted();
			if(connected == false)
			{
				ASSERT(sslSocket_->isEncrypted() == false);
				cout << "waitForEncrypted(): Error! " << sslSocket_->errorString() << endl;
				throw("waitForEncrypted(): ERROR! cannot complete SSL handshake!");
			}

			IDEBUG() << "Connected to " << interfaceHandle_ <<  " mode: " << (int)sslSocket_->mode() << endl;
			InitSocketObjects(sslSocket_);
			ASSERT(sslSocket_->isEncrypted() == true);
			rmiMonitor_.wakeAll();
		}
		catch(Exception ex)
		{
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_ << endl;
			sslSocket_->disconnectFromHost();
		}
		catch(const char *str)
		{
			IDEBUG() << "ERROR! connecting to " << interfaceHandle_ << " " << str << endl;
			sslSocket_->disconnectFromHost();
		}
		catch(...)
		{
			IDEBUG() << "ERROR! connecting to " << interfaceHandle_ << endl;
			cout << ".";
			sslSocket_->disconnectFromHost();
		}
	}

	return connected;
}

RmiSslClient* RmiSslClient::FindProxy(const InterfaceHandle &handle, const SetRmiSslClient *setRmiSslClient)
{
	if(setRmiSslClient == NULL) return NULL;

	for(SetRmiSslClient::const_iterator it = setRmiSslClient->begin(), it_end = setRmiSslClient->end(); it != it_end; ++it)
	{
		if((*it)->GetInterfaceHandle() == handle) return (*it);
	}
	return NULL;
}

void RmiSslClient::AddTarget(InterfaceHandle &handle)
{
	IWARNING()  << " adding target : " << handle << endl;
	ExecuteOneWayRMI(RmiInterface::AddTarget, &handle);
}

void RmiSslClient::RemoveTarget(InterfaceHandle &handle)
{
	ExecuteOneWayRMI(RmiInterface::RemoveTarget, &handle);
}

} // namespace MicroMiddleware

#endif
