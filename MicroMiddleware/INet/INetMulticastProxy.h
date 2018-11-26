#ifndef MicroMiddleware_INetMulticastProxy_h_Included
#define MicroMiddleware_INetMulticastProxy_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class INetProxy;
class RpcIdentifier;
class RmiObjectBase;

// Overall: similar strategy as in ClientDynamicsManager

// This: similar to INetMulticast
//	   - one INetMulticast for each group
//	   - keep list of INetProxy objects
//	   - list can be edited from outside -> group dynamics
//	   - list can be sent to from outside -> group multicast

// TODO: INetMulticast.h/cpp

// TODO: Use this in VivaldiClient
//  - Add INetProxy objects after registering in INetConnection
//  - Use to multicast vivaldi information to a group
class DLL_STATE INetMulticastProxy
{
public:
	INetMulticastProxy()
	{}
	virtual ~INetMulticastProxy()
	{}
	
	typedef map<InterfaceHandle, INetProxy*>	MapHandleProxy;
	typedef map<string, MapHandleProxy> 		MapGroupProxy;

	MapHandleProxy& GetGroup(const string &groupName);
	
	//void RegisterProxy(const GroupHandle &handle, INetProxy *proxy);
	//void RemoveProxy(const GroupHandle &handle);
	void RegisterOrRemoveProxies(GroupInformation &groupInfo, const std::string &interfaceName);

	void ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName);

protected:
	Mutex					clientMutex_;
	MapGroupProxy			mapGroupProxies_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetMulticastProxy_h_Included




