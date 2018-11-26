#include "MicroMiddleware/NetObjects/RpcIdentifier.h"

namespace MicroMiddleware
{

std::ostream& operator<<(std::ostream &ostr, const RpcIdentifier::ConnectionId &id)
{
	switch(id)
	{
#define CASE(t) case t: ostr << #t; break;
	CASE(RpcIdentifier::NoConnectionId)
	CASE(RpcIdentifier::StubConnection)
	CASE(RpcIdentifier::ProxyConnection)
	CASE(RpcIdentifier::SubscriberConnection)
	CASE(RpcIdentifier::PublisherConnection)
#undef CASE
	default:
		IDEBUG() << "ERROR! Could not identify RpcIdentifier::ConnectionId" ;
		break;
	}
	return ostr;
}

std::ostream& operator<<(std::ostream &ostr, const RpcIdentifier &rpc)
{
	ostr << "(" << rpc.GetInterfaceId()
		 << "," << rpc.GetInterfaceName()
		 << "," << rpc.GetMethodId() 
		 << "," << rpc.GetHostEndian() 
		 << "," << rpc.GetRpcSize()
		 << "," << rpc.GetConnectionId() 
		 << ")" ;
	return ostr;
}

}

