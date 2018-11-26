#include "MicroMiddleware/Net/AlmSubscriber.h"
#include "MicroMiddleware/Net/RpcMulticastServer.h"
#include "MicroMiddleware/RmiInterface.h"
#include "MicroMiddleware/Net/AlmMulticastPublisher.h"
#include "MicroMiddleware/Net/AlmForwarder.h"

namespace MicroMiddleware
{

AlmSubscriber::AlmSubscriber(const InterfaceHandle &interfaceHandle, AlmMulticastPublisher *client, bool autoStart) 
	: runThread_(true)
	, interfaceHandle_(interfaceHandle)
	, udpSocket_()
	, multicastPublisher_(client)
	, almForwarder_(0)
{
	if(autoStart) start();
}

AlmSubscriber::~AlmSubscriber()
{
	MutexLocker lock(&clientMutex_);
	cleanUp();
}

void AlmSubscriber::StopServer()
{
	MutexLocker lock(&clientMutex_);
	runThread_ = false;
	udpSocket_->close();
}

void AlmSubscriber::initAlmSubscriber()
{
	if(!udpSocket_)
		udpSocket_ = UdpSocket::Ptr(new UdpSocket(UdpSocket::UnicastReceiver));

	IDEBUG()  << " binding subscriber to " << interfaceHandle_ ;
	
	int port = interfaceHandle_.GetPortNumber();
	runThread_ = udpSocket_->bindReceiver(interfaceHandle_.GetHostName(), port);
	interfaceHandle_.SetPortNumber(port);

	IDEBUG()  << " binding subscriber to " << interfaceHandle_ ;
	if(runThread_ == false) //(runThread_ = udpSocket_->waitForConnected(50000)) == false)
	{
		IDEBUG() << "ERROR! Couldnt connect to " << interfaceHandle_.GetHostName() << " " << interfaceHandle_.GetPortNumber() ;
		throw Exception("Could not bind to to host/port");
	}
	
	InitSocketObjects(udpSocket_, interfaceHandle_.GetHostName(), interfaceHandle_.GetPortNumber());
}

void AlmSubscriber::run()
{
	try
	{
		clientMutex_.lock();
			initAlmSubscriber();
		clientMutex_.unlock();
	
		RpcIdentifier rpc;
		while(runThread_)
		{
			//bool ret = udpSocket_->waitForDatagramsAvailable();
			//if(ret == false)
			//{
			//	throw Exception(" udpSocket returned no datagrams available!");
			//}
			//while(udpSocket_->hasPendingDatagrams() == false)
			//	usleep(10000);

			BeginUnmarshal(&rpc);
			IDEBUG() << " Read: " << rpc ;
			
			switch(rpc.GetMethodId())
			{
				// TODO: AddTarget and RemoveTarget are never going to be called
				//  	because AlmSubscriber is using UDP, it is not necessary!
				//		This is not true!
				case RmiInterface::AddTarget:
				{
					GroupHandle handle;
					handle.Read(GetSocketReader());
					EndUnmarshal();

					IDEBUG() << " adding client " << handle ;
					AddTarget(handle);
					break;
				}
				case RmiInterface::RemoveTarget:
				{
					GroupHandle handle;
					handle.Read(GetSocketReader());
					EndUnmarshal();

					IDEBUG() << " removing client " << handle ;
					RemoveTarget(handle);
					break;
				}
				default:
					RPCall(rpc);
					break;
			}
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "ERROR! Exception caught! " << ex.msg() ;
	}
	catch(std::runtime_error msg)
	{
		IDEBUG() << "ERROR! Exception caught! " << msg.what() ;
	}
	catch(const string &msg)
	{
		IDEBUG() << "ERROR! Exception caught! " << msg ;
	}
	catch(const char *msg)
	{
		IDEBUG() << "ERROR! Exception caught! " << msg ;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Exception caught! " ;
	}
}

void AlmSubscriber::AddTarget(GroupHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	
	if(multicastPublisher_ == NULL)
	{
		IDEBUG() << "WARNING! multicast client == NULL! Returning!" ;
		return;
	}

	multicastPublisher_->AddMulticastClient(handle);
}

void AlmSubscriber::RemoveTarget(GroupHandle &handle)
{
	MutexLocker lock(&clientMutex_);

	if(multicastPublisher_ == NULL)
	{
		IDEBUG() << "WARNING! multicast client == NULL! Returning!" ;
		return;
	}

	multicastPublisher_->RemoveMulticastClient(handle);
}

// -> called from RpcMulticastServer
void AlmSubscriber::RPCall(const RpcIdentifier &rpc)
{
	MutexLocker lock(&clientMutex_);

	//IDEBUG() << " RpcIdentifier " << rpc ;

	switch(rpc.GetMethodId())
	{
		// TODO: Anything?
		default:
		{
			if(mapInterfaceIdAlmServer_.count(rpc.GetInterfaceId()) == 0)
			{
				IDEBUG() << "ERROR! rpc server to interface id " << rpc.GetInterfaceId() << " is not present!" ;
				break;
			}

			AlmServer *almServer = mapInterfaceIdAlmServer_[rpc.GetInterfaceId()];
			ASSERT(almServer != NULL);

			// TODO: Optimize by reading entire RPC and simultaneously call almServer and almForwarder
			RmiObjectBase *rmiObject = almServer->RPCall(rpc, this);
			if(almForwarder_)
			{
				// Forward UDP message to neighbors here
				//-> void AlmForwarder::ForwardRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
				almForwarder_->ForwardRpc((RpcIdentifier*)&rpc, rmiObject, almServer->GetGroupName());
			}
		}
	}
}

bool AlmSubscriber::RegisterInterface(int interfaceId, AlmServer *almServer)
{
	if(mapInterfaceIdAlmServer_.count(interfaceId) > 0)
	{
		IDEBUG() << "WARNING! RPC server to interface id " << interfaceId << " is already present!" ;
		return false;
	}
	ASSERT(almServer);

	mapInterfaceIdAlmServer_.insert(pair<int, AlmServer*>(interfaceId, almServer));
	return true;
}

void AlmSubscriber::SetAlmForwarder(AlmForwarder *almForwarder)
{
	MutexLocker lock(&clientMutex_);

	if(almForwarder_ != NULL)
	{
		IWARNING()  << "WARNING! AlmForwarder is already set! Returning without setting almForwarder!" ;
		return;
	}

	if(almForwarder == NULL)
		IWARNING()  << "AlmForwarder is NULL!" ;

	almForwarder_ = almForwarder;
}

void AlmSubscriber::cleanUp()
{
	// TODO: delete UdpSocket?
}

} // namespace end

