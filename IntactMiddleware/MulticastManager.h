#ifndef IntactMiddleware_MulticastManager_h_IsIncluded
#define IntactMiddleware_MulticastManager_h_IsIncluded

#include "IntactMiddleware/IncludeExtLibs.h"
#include "IntactMiddleware/Export.h"

namespace IntactMiddleware
{
// --------------------------------------------------------
//			class MulticastManager
//			 - one for each client
// --------------------------------------------------------
class DLL_STATE MulticastManager : public Thread
{
public:
	MulticastManager(const InterfaceHandle &multicastServerHandle, bool autorun = false);
	virtual ~MulticastManager() {}

	virtual void run();

	bool RegisterInterface(int interfaceId, RpcServer *rpcServer);
	void AddOrRemoveMulticastClients(GroupInformation groupInfo);
	void RemoveMulticastClient(GroupHandle &handle);
	void AddMulticastClient(GroupHandle &handle);

	RpcMulticastServerListener* GetRpcMulticastServer() 		{ return multicastServer_; }
	RpcMulticastClient*			GetRpcMulticastClient()			{ return multicastClient_; }

	InterfaceHandle				GetInterfaceHandle()			{ return multicastServerHandle_; }

private:
	inline void lock()			{ mutexUpdate_.lock(); }
	inline void unlock()		{ mutexUpdate_.unlock(); }

private:
	WaitCondition				waitForChanges_;
	Mutex						mutexUpdate_;
	InterfaceHandle				multicastServerHandle_;

	RpcMulticastServerListener	*multicastServer_;
	RpcMulticastClient			*multicastClient_;
};

}

#endif


