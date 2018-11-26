#ifndef MicroMiddleware_INetMulticast_h_Included
#define MicroMiddleware_INetMulticast_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class INetConnection;
class RpcIdentifier;
class RmiObjectBase;

// Keep track of neighbors to publish/forward data too
// This class is accessed from AlmForwarder and the publisher implementation of an interface.

class DLL_STATE INetMulticast : public Thread
{
public:
	INetMulticast(const InterfaceHandle &multicastServer);
	virtual ~INetMulticast();

	typedef std::map<InterfaceHandle, INetConnection*>		MapHandleConnection;
	typedef std::map<std::string, MapHandleConnection> 		MapGroupConnection;

private:
	virtual void run();

public:
	MapHandleConnection& GetMulticastGroup(const string &groupName);
	
	void ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName);
	
	void RegisterOrRemoveConnections(const GroupInformation &groupInfo);
	void RegisterConnection(const GroupHandle &handle);
	void RemoveConnection(const GroupHandle &handle);

private:
	// not thread safe
	void registerConnection(const GroupHandle &handle);
	void removeConnection(const GroupHandle &handle);

protected:
	InterfaceHandle				multicastServerInterface_;
	
	mutable Mutex				mutex_;
	WaitCondition				waitCondition_;
	MapGroupConnection			mapGroupConnection_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetMulticast_h_Included



