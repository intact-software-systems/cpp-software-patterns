#ifndef IntactMiddleware_RpcMulticastManager_h_IsIncluded
#define IntactMiddleware_RpcMulticastManager_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{

class DLL_STATE RpcMulticastManager : public Thread
{
public:
	RpcMulticastManager(HostInformation hostInfo = HostInformation());
	~RpcMulticastManager();

private:
	virtual void run();

public:
	GroupInformation	GetGroupInformation();

	bool StartMulticast(const InterfaceHandle &multicastServerHandle = InterfaceHandle());
	bool StopMulticast();
	
	bool RegisterInterface(int interfaceId, RpcServer *rpcServer);
	bool JoinGroup(string groupName);

	inline RpcMulticastServerListener*	GetMulticastServer()	const { MutexLocker lock(&mutexUpdate_); return multicastServer_; }
	inline RpcMulticastClient*			GetMulticastClient()	const { MutexLocker lock(&mutexUpdate_); return multicastClient_; }
	GroupHandle							GetGroupHandle()		const { MutexLocker lock(&mutexUpdate_); return groupHandle_; }

private:
	bool StopAllThreads();
	
private:
	RpcMulticastServerListener	*multicastServer_;
	RpcMulticastClient			*multicastClient_;

	InterfaceHandle				multicastServerHandle_;
	GroupHandle					groupHandle_;
	HostInformation				myHostInformation_;
	
	WaitCondition				waitCondition_;
	mutable Mutex				mutexUpdate_;
	bool						runThread_;
	GroupInformation			groupInformation_;
};

} // namespace IntactMiddleware

#endif


