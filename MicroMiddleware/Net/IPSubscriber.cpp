#include"MicroMiddleware/Net/IPSubscriber.h"
#include"MicroMiddleware/Net/IPSubscriberListener.h"
#include"MicroMiddleware/RmiInterface.h"

#include"BaseLib/IncludeLibs.h"

namespace MicroMiddleware
{
IPSubscriber::IPSubscriber(IPSubscriberListener *rmiSL, const InterfaceHandle &handle, bool autoStart) 
	: RmiBase(),
	runThread_(autoStart),
	interfaceHandle_(handle),
	udpSocket_(),
	socketDescriptor_(-1),
	rmiListener_(rmiSL)
{
	if(autoStart) start();
}

IPSubscriber::~IPSubscriber()
{
	//MutexLocker locker(&serverMutex_);
	
	if(rmiListener_) rmiListener_->StopIPSubscriber(this);
	
	if(udpSocket_)
		udpSocket_->close();
}

void IPSubscriber::initIPSubscriber()
{
	udpSocket_ = UdpSocket::Ptr(new UdpSocket(UdpSocket::MulticastReceiver));
	
	int port = interfaceHandle_.GetPortNumber();
	runThread_ = udpSocket_->bindReceiver(interfaceHandle_.GetHostName(), port);
	if(!runThread_)
		IDEBUG() << "ERROR! Couldnt bind to " << interfaceHandle_ ;

	interfaceHandle_.SetPortNumber(port);
	IDEBUG()  << " Multicast receiver bound to : " << interfaceHandle_ ;

	InitSocketObjects(udpSocket_);
}

// client monitor wait on event -> socket closed, or runThread == false!
// wait for incoming RMI calls
// read type of RMI and 
void IPSubscriber::run()
{
	try
	{
		initIPSubscriber();
		
		while(runThread_)
		{
			int methodId = BeginUnmarshal();
			//IDEBUG() << " Read method id : " << methodId ;
			
			switch(methodId)
			{
				// RmiInterface::ReleaseInterface:
				// break;
				default:
					InvokeRMI(methodId);
					break;
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << " Exception : " << ex.msg() ;
	}
	catch(const char *str)
	{
		IDEBUG() << " Exception : " << str ;
	}
	catch(...)
	{
		IDEBUG() << " Unknown exception! " ;
	}
	
	rmiListener_->StopIPSubscriber(this);
	delete this;
}

void IPSubscriber::InvokeRMI(int methodId)
{
	try
	{
		ASSERT(rmiListener_);
		rmiListener_->RMInvocation(methodId, this);
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception! " << ex.msg() ;
		BeginMarshal(RmiInterface::ReturnException);
		GetSocketWriter()->WriteString("Exception message");
		EndMarshal();	
	}
	catch(...)
	{
		IDEBUG() << "Exception! " ;
		BeginMarshal(RmiInterface::ReturnException);
		GetSocketWriter()->WriteString("Exception message");
		EndMarshal();	
	}
}

void IPSubscriber::StartOrRestart(const InterfaceHandle &interfaceHandle)
{
	MutexLocker lock(&serverMutex_);
	
	interfaceHandle_ = interfaceHandle;
	startOrRestart();
}

void IPSubscriber::Stop(bool waitForTermination)
{
	lock(); 
		try
		{
			runThread_ = false;
			if(udpSocket_) udpSocket_->close();
			rmiMonitor_.wakeAll();
		}
		catch(...)
		{
			IDEBUG() << " Exception caught!" ;
		}
	unlock();

	if(waitForTermination)
		wait();
}

void IPSubscriber::startOrRestart()
{
	if(isRunning())
	{
		runThread_ = false;
		if(udpSocket_) udpSocket_->close();
		wait();
	}

	runThread_ = true;
	start();
}

}; // namespace MicroMiddleware


