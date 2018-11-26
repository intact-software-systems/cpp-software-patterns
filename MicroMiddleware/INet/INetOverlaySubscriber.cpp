#include"MicroMiddleware/INet/INetOverlaySubscriber.h"
#include"MicroMiddleware/CommonDefines.h"
#include"MicroMiddleware/Runtime.h"

namespace MicroMiddleware
{

INetOverlaySubscriber::INetOverlaySubscriber(const std::string &serviceName, const std::string &subscribeToName, const std::string &interfaceName, EndianConverter::Endian hostEndian, bool autoStart) 
	: INetStub(serviceName, interfaceName, HostInformation::SUBSCRIBER, hostEndian, autoStart)
	, subscribeToName_(subscribeToName)
{ 

}

INetOverlaySubscriber::~INetOverlaySubscriber() 
{ 

}

std::string INetOverlaySubscriber::GetConnectedToName() const
{
	MutexLocker lock(&mutex_);
	return subscribeToName_;
}

// --------------------------------------------------
//  - Wait for rpc calls 
//  - RpcCalls are added to rpcQueue by INetConnection
//  - When woken up call RPCall(rpc), which is reimplemented by ComponentStub
// 	- If unbuffered: ReadUnlock is automatically called in EndUnmarshal()
// 
// TODO: producer/consumer list of incoming data-topics/CacheChanges
// depending on QoSPolicies what to do
// - Frequency of subscriptions, etc
// - communicate CacheChanges to HistoryCache
// - refer to INetStub's implementation 
// --------------------------------------------------
void INetOverlaySubscriber::run()
{
	try
	{
		IDEBUG() << "started";

		while(runThread_)
		{
            RpcCallWrapper rpcCall = queueRpcCall_.consumer_pop_front();
			if(runThread_ == false) break;

			RpcIdentifier rpc 				= rpcCall.GetRpcId();
			currentConnection_				= rpcCall.GetConnection();
			SerializeReader::Ptr readBuffer = rpcCall.GetReadBuffer();
			
			// -- start debug --
			ASSERT(rpc.IsValid());
			ASSERT(rpc.GetConnectionId() == RpcIdentifier::StubConnection || rpc.GetConnectionId() == RpcIdentifier::SubscriberConnection);
			ASSERT(currentConnection_);
			// -- end debug --
		
			ReadRpc(rpc, readBuffer);
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught! " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!" ;
	}
	
	// TODO: deregister?
	iCritical() << COMPONENT_FUNCTION << "Subscriber stopped!" ;
}

// --------------------------------------------------
// Start unmarshalling rpc
// --------------------------------------------------
void INetOverlaySubscriber::ReadRpc(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer) 
{
	try
	{
		//IDEBUG() << " Stub " << GetName() << " rpc: " << rpc << endl;
		bool ret = RPCall(rpc, readBuffer);
		if(ret == false)
			cerr << COMPONENT_FUNCTION << "WARNING! RPCall failed: " << rpc << endl;
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught! " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!";
	}
}

} // namespace MicroMiddleware


