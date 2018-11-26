#ifndef MicroMiddleware_RpcMulticastClient_h_Included
#define MicroMiddleware_RpcMulticastClient_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RpcClient;
class RpcIdentifier;
class RmiObjectBase;

class DLL_STATE RpcMulticastClient
{
public:
	RpcMulticastClient(const InterfaceHandle &multicastServer);
	virtual ~RpcMulticastClient();
	
	typedef map<ClientTriplet, RpcClient*>				MapClientTripletRpcClient;
	typedef map<string, MapClientTripletRpcClient> 		MapGroupNameRcpClient;

	MapClientTripletRpcClient& GetRpcGroup(const string &groupName);
	
	void ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName);
	void AddOrRemoveMulticastClients(GroupInformation &groupInfo);

	void AddMulticastClient(const GroupHandle &handle);
	void RemoveMulticastClient(const GroupHandle &handle);

protected:
	InterfaceHandle			multicastServerInterface_;
	Mutex					clientMutex_;
	MapGroupNameRcpClient	mapGroupNameRpcClient_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcMulticastClient_h_Included



