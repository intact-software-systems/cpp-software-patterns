#include "MicroMiddleware/Net/RpcMulticastServer.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/Net/RpcMulticastServerListener.h"

namespace MicroMiddleware
{
RpcMulticastServer::RpcMulticastServer(RpcMulticastServerListener *rmiSL, int socketDescriptor, bool autoStart) 
	: RpcBase()
	, rmiMCListener_(rmiSL)
	, runThread_(true)
	, tcpSocket_()
	, socketDescriptor_(socketDescriptor)
{
	if(autoStart) start();
}

RpcMulticastServer::~RpcMulticastServer()
{
	MutexLocker locker(&serverMutex_);
	
	if(tcpSocket_)
		tcpSocket_->close();
}

void RpcMulticastServer::Stop()
{
	MutexLocker locker(&serverMutex_);
	
	if(isRunning() == false) return;

	runThread_ = false;

	if(tcpSocket_)
		tcpSocket_->close();
}

void RpcMulticastServer::initRpcServer()
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


void RpcMulticastServer::run()
{
	try
	{
		initRpcServer();
		
		IWARNING()  << " Waiting for data on interface : " << interfaceHandle_ ;
		
		RpcIdentifier rpc;
		while(runThread_)
		{
			BeginUnmarshal(&rpc);
			//IDEBUG() << " Read: " << rpc ;
			
			switch(rpc.GetMethodId())
			{
				case RmiInterface::AddTarget:
				{
					GroupHandle handle;
					handle.Read(GetSocketReader());
					EndUnmarshal();

					IDEBUG() << " adding client " << handle ;
					rmiMCListener_->AddTarget(handle);
					break;
				}
				case RmiInterface::RemoveTarget:
				{
					GroupHandle handle;
					handle.Read(GetSocketReader());
					EndUnmarshal();

					IDEBUG() << " removing client " << handle ;
					rmiMCListener_->RemoveTarget(handle);
					break;
				}
				default:
					InvokeRMI(rpc);
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

void RpcMulticastServer::InvokeRMI(const RpcIdentifier &rpc)
{
	//IWARNING()  << " Invoking: " << rpc ;
	
	try
	{
		rmiMCListener_->RPCall(rpc, this);
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception!" << ex.msg() ;
		//BeginMarshal(RpcInterface::ReturnException);
		//GetSocketWriter()->WriteString("Exception message");
		//EndMarshal();	
	}
	catch(...)
	{
		IDEBUG() << "Exception! " ;
		//BeginMarshal(RpcInterface::ReturnException);
		//GetSocketWriter()->WriteString("Exception message");
		//EndMarshal();	
	}
}

}; // namespace end


