#if 0
#include"MicroMiddleware/RmiSharedBase.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/RmiObjectBase.h"

namespace MicroMiddleware
{

//-------------------------------------------------------------------------------
//		RMI Sender
//-------------------------------------------------------------------------------
void RmiSender::run()
{
	while(true)
	{
		// dequeue out-going remote method invocations (request or response)
		// and write to socketWriter_->WriteObject();

	}
}

//-------------------------------------------------------------------------------
//		RMI Receiver 
//-------------------------------------------------------------------------------
void RmiReceiver::run()
{
	while(true)
	{
		// read in-coming remote method invocations (request or response)
		// and distribute to correct channel.	
	}
}

//-------------------------------------------------------------------------------
//		RMI Shared Base 
//-------------------------------------------------------------------------------
RmiSenderReceiver::RmiSenderReceiver(const InterfaceHandle &handle) 
	: rmiSender_(NULL)
	, rmiReceiver_(NULL)
	, interfaceHandle_(handle)
	, tcpServer_(NULL)
{

}

RmiSenderReceiver::~RmiSenderReceiver()
{
}

void RmiSenderReceiver::initListener()
{
	if(tcpServer_ == NULL)
		tcpServer_ = new TcpServer();

	runThread_ = tcpServer_->listen(interfaceHandle_.GetHostName(), interfaceHandle_.GetPortNumber());
	if(runThread_ == false)
		IDEBUG() << "ERROR! Couldnt listen to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber() << endl;
}

void RmiSenderReceiver::run()
{
	try
	{
		objectMutex_.lock();
			initListener();
		objectMutex_.unlock();
		
		while(runThread_)
		{
			IDEBUG() << "Wait for connections" << endl;

			bool newConnection = tcpServer_->waitForNewConnection(-1);
			if(newConnection == false || runThread_ == false) break;

			TcpSocket *tcpSocket = (TcpSocket*)tcpServer_->nextPendingConnection();
			if(tcpSocket == NULL)
			{
				IDEBUG() << "ERROR! Tcp socket next pending connection returned tcpSocket == NULL!" << endl;
				break;
			}
			
			MutexLocker lock(&objectMutex_);
			
			IDEBUG() << "New connection from " << tcpSocket->peerAddress().toString().toStdString() << endl;
			RmiMulticastServer *rmiServer = new RmiMulticastServer(this, tcpSocket);
			
			if(mapRmiServer_.count(tcpSocket->socketDescriptor()) > 0)
				IDEBUG() << "ERROR! Socket already present in map! " << endl;

			mapRmiServer_.insert(pair<int, RmiMulticastServer*>(tcpSocket->socketDescriptor(), rmiServer));
		
			addMulticastClient(tcpSocket);
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! Exception caught! " << ex.msg() << endl;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Exception caught! " << endl;
	}
}



void RmiSenderReceiver::StartRmi(short methodId, RmiObjectBase *object)
{
	// enqueue to RmiSender
}

SocketReader* RmiSenderReceiver::BeginUnmarshal(short methodId, short objectId)
{
	// dequeue from RmiReceiver
}



} // namespace MicroMiddleware


#endif

