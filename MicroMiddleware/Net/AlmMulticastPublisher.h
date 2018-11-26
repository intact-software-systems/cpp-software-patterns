#ifndef MicroMiddleware_AlmMulticastPublisher_h_Included
#define MicroMiddleware_AlmMulticastPublisher_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class AlmPublisher;
class RpcIdentifier;
class RmiObjectBase;


// Keep track of neighbors to publish/forward data too
// This class is accessed from AlmForwarder and the publisher implementation of an interface.

class DLL_STATE AlmMulticastPublisher
{
public:
	AlmMulticastPublisher(const InterfaceHandle &multicastServer)
		: multicastServerInterface_(multicastServer)
	{ }
	virtual ~AlmMulticastPublisher()
	{}
	
	typedef map<ClientTriplet, AlmPublisher*>				MapClientTripletAlmPublisher;
	typedef map<string, MapClientTripletAlmPublisher> 		MapGroupNameAlmPublisher;

	MapClientTripletAlmPublisher& GetRpcGroup(const string &groupName);
	
	void ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName);
	void AddOrRemoveMulticastClients(GroupInformation &groupInfo);

	void AddMulticastClient(const GroupHandle &handle);
	void RemoveMulticastClient(const GroupHandle &handle);

protected:
	InterfaceHandle				multicastServerInterface_;
	mutable Mutex				clientMutex_;
	MapGroupNameAlmPublisher	mapGroupNameAlmPublisher_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_AlmMulticastPublisher_h_Included




