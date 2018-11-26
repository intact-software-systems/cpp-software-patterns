#include"MicroMiddleware/INet/INetOverlayPublisher.h"
#include"MicroMiddleware/INet/INetBase.h"
#include"MicroMiddleware/Runtime.h"
#include"MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{

// Constructor only used when proxy never has any return values
INetOverlayPublisher::INetOverlayPublisher(const std::string &serviceName, const std::string &interfaceName, EndianConverter::Endian hostEndian) 
	: INetProxy(serviceName, string("publisher.no.connection"), interfaceName, HostInformation::PUBLISHER, hostEndian)
{ 

}

INetOverlayPublisher::~INetOverlayPublisher() 
{ 

}

// --------------------------------------------------
//	TODO: Read return value of RPC
//		-> If middleware acknowledgements of successfully sent packets then remove from HistoryCache
//		-> If failure sending packets then notify application
//		-> if packet is intended for application then add to filteredQueueRpcCalls
// 	TODO:
// 		producer/consumer list of topics to publish
// 		based on subscribers and QoSPolicies
// --------------------------------------------------
void INetOverlayPublisher::run()
{
	try
	{
		IDEBUG() << "started";

		while(runThread_)
		{
            RpcCallWrapper rpcCall = queueRpcCall_.consumer_pop_front();
			if(runThread_ == false) break;

			RpcIdentifier rpc 				= rpcCall.GetRpcId();
			//currentConnection_				= rpcCall.GetConnection();
			SerializeReader::Ptr readBuffer = rpcCall.GetReadBuffer();
			
			// -- start debug --
			ASSERT(rpc.IsValid());
			ASSERT(rpc.GetConnectionId() == RpcIdentifier::StubConnection || rpc.GetConnectionId() == RpcIdentifier::SubscriberConnection);
			//ASSERT(currentConnection_);
			// -- end debug --
					
			// --------------------------------------------------------
			//-> If middleware acknowledgements of successfully sent packets then remove from HistoryCache
			//-> TODO: Where do I add data to HistoryCache?
			//	-> Should this thread handle both IN/OUT messages?
			//-> If failure sending packets then notify application
			//-> if packet is intended for application then add to filteredQueueRpcCalls
			// --------------------------------------------------------
			
			queueFilteredRpcCalls_.producer_push_back(rpcCall);
		}
	}
	catch(Exception ex)
	{
		ICRITICAL() << "Exception caught! " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!" ;
	}
	
	// TODO: deregister?
	iCritical() << COMPONENT_FUNCTION << "Stub stopped!" ;
}

// --------------------------------------------------
// Adds message to output queue 
// TODO: Should this function call MessageParser directly?
// --------------------------------------------------
bool INetOverlayPublisher::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	// -- debug --
	ASSERT(rpc);
	ASSERT(obj);
	// -- debug --

	MutexLocker lock(&mutex_);
	if(inetConn_ == NULL) 
	{
		stringstream str;
		str << PRETTY_FUNCTION << "INetConnection is not connected!";
		throw Exception(str.str());
	}

	rpc->SetConnectionId(RpcIdentifier::SubscriberConnection);
	ASSERT(rpc->GetConnectionId() == RpcIdentifier::SubscriberConnection);

	// TODO: What is correct behavior if there are no subscribers?
	// - It is based on the QoS of the publisher?
	if(subscriberGroup_.IsEmpty())
	{
		IWARNING() << "Subscriber group is empty!";
		return false;
	}

	obj->SetOutNetAddresses(subscriberGroup_.GetNetAddresses());

	return inetConn_->ExecuteRpc(rpc, obj);
}

// --------------------------------------------------
//	OverlayPublisher overrides INetProxy's EndMarshal
// --------------------------------------------------
bool INetOverlayPublisher::EndMarshal(SerializeWriter::Ptr &writer)
{
	MutexLocker lock(&mutex_);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			if(inetConn_ == NULL)
			{
				throw Exception("ERROR! Please register INetProxy in INetConnection before initiating RPC calls!");
			}
			
			if(subscriberGroup_.IsEmpty())
			{
				IWARNING() << "Subscriber group is empty!";
				return false;
			}
			
			return inetConn_->EndMarshal(writer, subscriberGroup_.GetNetAddresses());
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return false;
}

std::string INetOverlayPublisher::GetConnectedToName() const
{
	ICRITICAL() << "PUBLISHER has no connected to service name!";
	return std::string();
}

bool INetOverlayPublisher::AddSubscriber(HostInformation &subscriberInfo)
{
	ASSERT(subscriberInfo.IsValid());
	subscriberGroup_.AddHost(subscriberInfo);
	return true;
}

bool INetOverlayPublisher::RemoveSubscriber(std::string &subscriberName)
{
	ASSERT(!subscriberName.empty());
	return subscriberGroup_.RemoveHost(subscriberName);
}

} // namespace MicroMiddleware


