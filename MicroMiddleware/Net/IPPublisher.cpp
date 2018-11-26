#include"MicroMiddleware/Net/IPPublisher.h"
#include"MicroMiddleware/RmiInterface.h"

namespace MicroMiddleware
{

IPPublisher::IPPublisher(const InterfaceHandle &interfaceHandle, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, udpSocket_()
{
	if(autoStart) start();
}


void IPPublisher::initUdpSocket()
{
	MutexLocker lock(&clientMutex_);

    udpSocket_ = UdpSocket::Ptr(new UdpSocket(UdpSocket::MulticastSender) );

	rmiMonitor_.wakeAll();
}

// UDP: Bind to address, NOT, connect to server
void IPPublisher::run()
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
		IDEBUG() << "ERROR! RPC Client exception! ";
	}

	unlock();

	IDEBUG() << "RPC Client stopped";
}

void IPPublisher::StopClient(bool waitForTermination)
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

void IPPublisher::StartClient()
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

bool IPPublisher::WaitForUdpSocket()
{
	StartClient();
	
	MutexLocker lock(&clientMutex_);
	while(!udpSocket_)
		rmiMonitor_.wait(&clientMutex_, 1000);
	
	return true;
}

bool IPPublisher::WaitConnected()
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

bool IPPublisher::BindToInterfaceHandle()
{
	MutexLocker lock(&clientMutex_);
	return bindToInterfaceHandle();
}

bool IPPublisher::bindToInterfaceHandle()
{
	bool connected = IsBound();
	if(connected == false)
	{
		try
		{
			//IDEBUG() << "Connecting to " << interfaceHandle_<< endl;
			
			bool connected = udpSocket_->bindSender(interfaceHandle_.GetHostName(),interfaceHandle_.GetPortNumber());
			if(connected == false)
			{
				throw Exception("IPPublisher::bindToInterfaceHandle(): ERROR! binding error!");
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

}; // namespace MicroMiddleware




