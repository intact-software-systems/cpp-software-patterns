#ifndef IntactMiddleware_INetMulticastManager_h_IsIncluded
#define IntactMiddleware_INetMulticastManager_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/IntactComponent.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{

class DLL_STATE INetMulticastManager : public Thread
{
public:
	INetMulticastManager(IntactComponent *intactComponent, HostInformation hostInfo = HostInformation());
	~INetMulticastManager();

private:
	virtual void run();

public:
	GroupInformation	GetGroupInformation();

	bool StartMulticast(INetStub *stub, const std::string &interfaceName, const InterfaceHandle &multicastServerHandle = InterfaceHandle());
	bool StopMulticast();
	
	bool JoinGroup(string groupName);

	inline INetServerListener*			GetMulticastServer()	const { MutexLocker lock(&mutexUpdate_); return multicastServer_; }
	inline INetMulticastProxy*			GetMulticastProxy()		const { MutexLocker lock(&mutexUpdate_); return multicastProxy_; }
	GroupHandle							GetGroupHandle()		const { MutexLocker lock(&mutexUpdate_); return groupHandle_; }

private:
	bool StopAllThreads();
	
private:
	IntactComponent 			*intactComponent_;
	INetStub					*multicastStub_;
	INetServerListener			*multicastServer_;
	INetMulticastProxy			*multicastProxy_;

	InterfaceHandle				multicastServerHandle_;
	GroupHandle					groupHandle_;
	HostInformation				myHostInformation_;
	
	WaitCondition				waitCondition_;
	mutable Mutex				mutexUpdate_;
	bool						runThread_;
	GroupInformation			groupInformation_;
	std::string 				interfaceName_;
};

} // namespace IntactMiddleware

#endif


