#include"MicroMiddleware/Net/RmiServer.h"
#include"MicroMiddleware/Net/RmiServerListener.h"
#include"MicroMiddleware/RmiInterface.h"

namespace MicroMiddleware
{
RmiServer::RmiServer(RmiServerListener *rmiSL, int socketDescriptor, bool autoStart) 
	: RmiBase(),
	runThread_(autoStart),
	interfaceHandle_(),
	tcpSocket_(),
	socketDescriptor_(socketDescriptor),
	rmiListener_(rmiSL)
{
	if(autoStart) start();
}

RmiServer::~RmiServer()
{
	MutexLocker locker(&serverMutex_);
	
	if(tcpSocket_) tcpSocket_->close();
}

void RmiServer::cleanUp()
{
	RmiBase::CleanUp();

	if(tcpSocket_) tcpSocket_->close();
}

void RmiServer::initServer()
{
	tcpSocket_ = TcpSocket::Ptr(new TcpSocket());
	bool ret = tcpSocket_->setSocketDescriptor(socketDescriptor_);
	if(ret == false)
	{
		throw Exception("Invalid socket given to TcpSocket");
	}

	interfaceHandle_ = InterfaceHandle(tcpSocket_->GetHostName(), tcpSocket_->GetPortNumber());
	
	InitSocketObjects(tcpSocket_);
}

// client monitor wait on event -> socket closed, or runThread == false!
// wait for incoming RMI calls
// read type of RMI and 
void RmiServer::run()
{
	try
	{
		initServer();
		
		while(runThread_)
		{
			int methodId = BeginUnmarshal();
			if(methodId < 0) break;
			
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
	
	cleanUp();
	rmiListener_->RemoveServer(socketDescriptor_);
}

void RmiServer::InvokeRMI(int methodId)
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

} // namespace MicroMiddleware

