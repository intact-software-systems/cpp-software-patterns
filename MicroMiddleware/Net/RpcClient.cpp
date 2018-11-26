#include"MicroMiddleware/Net/RpcClient.h"
#include"MicroMiddleware/RmiInterface.h"


namespace MicroMiddleware
{

RpcClient::RpcClient(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
    , tcpSocket_()
{
	if(autoStart) start();
}

RpcClient::~RpcClient()
{
	if(isRunning()) StopClient();
}

void RpcClient::initTcpSocket()
{
	MutexLocker lock(&clientMutex_);

	if(!tcpSocket_)
		tcpSocket_ = TcpSocket::Ptr(new TcpSocket());
}

void RpcClient::run()
{
	IWARNING()  << "Rpc Client started " << interfaceHandle_ ;
	
	try
	{
		initTcpSocket();

		clientMutex_.lock();

		while(runThread_)
		{
			if(IsConnected())
			{
				rmiMonitor_.wait(&clientMutex_, 1000);
				
				if(runThread_ == false) break;
			}
			
			if(connectToHost() == false)
			{	
				clientMutex_.unlock();
					sleep(1);
				clientMutex_.lock();
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! RPC Client exception! " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! RPC Client exception! " ;
	}

	clientMutex_.unlock();

	IDEBUG() << "RPC Client stopped" ;
}

void RpcClient::StopClient(bool waitForTermination)
{
	try
	{
		clientMutex_.lock(); 
		
		runThread_ = false;
		tcpSocket_->close();
		rmiMonitor_.wakeAll();
	}
	catch(...)
	{
		IDEBUG() << " Exception caught!" ;
	}
	clientMutex_.unlock();

	if(waitForTermination)
		wait();
}

void RpcClient::StartClient()
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

bool RpcClient::ConnectToHost()
{
	MutexLocker lock(&clientMutex_);
	return connectToHost();
}

bool RpcClient::connectToHost()
{
	bool connected = IsConnected();
	if(connected == false)
	{
		try
		{
			tcpSocket_->close();
			connected = tcpSocket_->connectToHost(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			if(connected == false)
			{
				throw("ERROR! connecting!");
			}

			IDEBUG() << "Connected to " << interfaceHandle_;
			InitSocketObjects(tcpSocket_);

			rmiMonitor_.wakeAll();
		}
		catch(Exception ex)
		{
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_ ;
			tcpSocket_->disconnectFromHost();
		}
		catch(...)
		{
			//IDEBUG() << "ERROR! connecting to " << interfaceHandle_ ;
			tcpSocket_->disconnectFromHost();
			cerr << ";";
		}
	}

	return connected;
}

bool RpcClient::WaitConnected()
{
	try
	{
		clientMutex_.lock();
		while(IsConnected() == false)
		{
			//IDEBUG() << " Client is still not connected!" ;
			rmiMonitor_.wait(&clientMutex_, 1000);
		}
	}
	catch(...)
	{
		IDEBUG() << " ERROR while waiting for connection on " << interfaceHandle_ ;
	}
	
	clientMutex_.unlock();
	return IsConnected();
}



void RpcClient::AddTarget(GroupHandle &handle)
{
	rpcAddTarget_ = RpcIdentifier(RmiInterface::AddTarget, RmiInterface::MulticastGroupUpdate, 1);
	
	ExecuteRpc(&rpcAddTarget_, &handle);
	IWARNING()  << " " << rpcAddTarget_<< " added target : " << handle ;
}

void RpcClient::RemoveTarget(GroupHandle &handle)
{
	rpcRemoveTarget_ = RpcIdentifier(RmiInterface::RemoveTarget, RmiInterface::MulticastGroupUpdate, 1);
	IWARNING()  << " " << rpcRemoveTarget_ << " removing target : " << handle ;
	ExecuteRpc(&rpcRemoveTarget_, &handle);
}

}; // namespace MicroMiddleware


