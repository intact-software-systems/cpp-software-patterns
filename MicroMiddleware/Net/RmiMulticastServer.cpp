#include "MicroMiddleware/Net/RmiMulticastServer.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/Net/RmiMulticastServerListener.h"

namespace MicroMiddleware
{
RmiMulticastServer::RmiMulticastServer(RmiMulticastServerListener *rmiSL, TcpSocket *tcpSocket, bool autoStart) 
	: RmiBase()
	, rmiMCListener_(rmiSL)
	, runThread_(true)
	, interfaceHandle_(tcpSocket->GetHostName(), tcpSocket->GetPortNumber())
	, tcpSocket_()
	, socketDescriptor_(tcpSocket->socketDescriptor())
{
	if(autoStart) start();
}

RmiMulticastServer::~RmiMulticastServer()
{
	MutexLocker locker(&serverMutex_);
	
	rmiMCListener_->RemoveServer(tcpSocket_->socketDescriptor());
	tcpSocket_->close();
}

void RmiMulticastServer::initRmiServer()
{
	tcpSocket_ = TcpSocket::Ptr(new TcpSocket());
	bool ret = tcpSocket_->setSocketDescriptor(socketDescriptor_);
	if(ret == false)
	{
		throw Exception("Invalid socket given to TcpSocket");
	}
	
	InitSocketObjects(tcpSocket_);
}


void RmiMulticastServer::run()
{
	try
	{
		IWARNING()  << " Waiting for data on interface : " << interfaceHandle_ ;
		
		initRmiServer();
		
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
	catch(...)
	{
		IDEBUG() << " Unknown exception! " ;
	}

	rmiMCListener_->RemoveServer(socketDescriptor_);

	//deleteLater();
}

void RmiMulticastServer::AddTarget(InterfaceHandle &handle)
{
	ExecuteOneWayRMI(RmiInterface::AddTarget, &handle);
}

void RmiMulticastServer::RemoveTarget(InterfaceHandle &handle)
{
	ExecuteOneWayRMI(RmiInterface::RemoveTarget, &handle);
}

void RmiMulticastServer::InvokeRMI(int methodId)
{
	//IWARNING()  << " Invoking method : " << methodId ;
	try
	{
		rmiMCListener_->RMInvocation(methodId, this);
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception!" << ex.msg() ;
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

void RmiMulticastServer::StartOrRestartServer(const InterfaceHandle &interfaceHandle)
{
	MutexLocker lock(&serverMutex_);
	
	interfaceHandle_ = interfaceHandle;
	startOrRestartServer();
}

void RmiMulticastServer::StopServer(bool waitForTermination)
{
	MutexLocker lock(&serverMutex_);
	if(isRunning())
	{
		runThread_ = false;
		tcpSocket_->close();
		if(waitForTermination)
			wait();
	}
}

void RmiMulticastServer::startOrRestartServer()
{
	if(isRunning())
	{
		runThread_ = false;
		tcpSocket_->close();
		wait();
	}

	runThread_ = true;
	start();
}

} // namespace end

