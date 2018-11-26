#include"MicroMiddleware/Net/AlmPublisher.h"
#include"MicroMiddleware/RmiInterface.h"

namespace MicroMiddleware
{

AlmPublisher::AlmPublisher(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
    , udpSocket_()
{
	if(autoStart) start();
}

void AlmPublisher::initUdpSocket()
{
	MutexLocker lock(&clientMutex_);

	if(!udpSocket_)
		udpSocket_ = UdpSocket::Ptr(new UdpSocket(UdpSocket::UnicastSender));

	rmiMonitor_.wakeAll();
}

// UDP: Bind to address, NOT, connect to server
void AlmPublisher::run()
{
	IWARNING()  << "Rpc Client started " << interfaceHandle_;
	
	try
	{
		initUdpSocket();

		lock();
		
		bindToInterfaceHandle();

		while(runThread_)
		{
			rmiMonitor_.wait(&clientMutex_, 1000);

			//if(IsBound())
			//{
			//	rmiMonitor_.wait(&clientMutex_, 1000);
			//	if(runThread_ == false) break;
			//}
			
			//if(bindToInterfaceHandle() == false)
			//{	
			//	unlock();
			//		sleep(1);
			//	lock();
			//}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! RPC Client exception! " << ex.msg();
	}
	catch(...)
	{
		IDEBUG() << "ERROR! RPC Client exception!";
	}

	unlock();

	IDEBUG() << "RPC Client stopped";
}

void AlmPublisher::StopClient(bool waitForTermination)
{
	lock(); 
		try
		{
			runThread_ = false;
			udpSocket_->close();
			rmiMonitor_.wakeAll();
		}
		catch(...)
		{
			IDEBUG() << " Exception caught!";
		}
	unlock();

	if(waitForTermination)
		wait();
}

void AlmPublisher::StartClient()
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

bool AlmPublisher::WaitForUdpSocket()
{
	StartClient();
	
	MutexLocker lock(&clientMutex_);
	while(!udpSocket_)
		rmiMonitor_.wait(&clientMutex_, 1000);
	
	return true;
}

bool AlmPublisher::WaitConnected()
{
	MutexLocker lock(&clientMutex_);
	bool connected = IsBound();
	while(connected == false)
	{
		IDEBUG() << " Client is still not connected!";
		rmiMonitor_.wait(&clientMutex_, 1000);
		connected = IsBound();
	}
	return connected;
}

bool AlmPublisher::BindToInterfaceHandle()
{
	MutexLocker lock(&clientMutex_);
	return bindToInterfaceHandle();
}

bool AlmPublisher::bindToInterfaceHandle()
{
	bool connected = IsBound();
	if(connected == false)
	{
		try
		{
			//IDEBUG() << "Connecting to " << interfaceHandle_;
			
			bool connected = udpSocket_->bindSender(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			//if((connected = udpSocket_->waitForConnected(50000)) == false)
			if(connected == false)
			{
				throw Exception("AlmPublisher::bindToInterfaceHandle(): ERROR! binding error!");
			}

			IDEBUG() << " bound to " << interfaceHandle_;
			InitSocketObjects(udpSocket_, interfaceHandle_.GetHostName(), interfaceHandle_.GetPortNumber());
			
			rmiMonitor_.wakeAll();
		}
		catch(Exception ex)
		{
			IDEBUG() << "ERROR! " << ex.msg() << " connecting to " << interfaceHandle_;
			udpSocket_->close(); //disconnectFromHost();
		}
		catch(...)
		{
			//IDEBUG() << "ERROR! connecting to " << interfaceHandle_;
			udpSocket_->close(); //disconnectFromHost();
			cerr << ";";
		}
	}

	return connected;
}

void AlmPublisher::AddTarget(GroupHandle &handle)
{
	rpcAddTarget_ = RpcIdentifier(RmiInterface::AddTarget, RmiInterface::MulticastGroupUpdate, 1);
	
	ExecuteRpc(&rpcAddTarget_, &handle);
	IWARNING()  << " " << rpcAddTarget_<< " added target : " << handle;
}

void AlmPublisher::RemoveTarget(GroupHandle &handle)
{
	rpcRemoveTarget_ = RpcIdentifier(RmiInterface::RemoveTarget, RmiInterface::MulticastGroupUpdate, 1);
	IWARNING()  << " " << rpcRemoveTarget_ << " removing target : " << handle;
	ExecuteRpc(&rpcRemoveTarget_, &handle);
}

} // namespace MicroMiddleware



