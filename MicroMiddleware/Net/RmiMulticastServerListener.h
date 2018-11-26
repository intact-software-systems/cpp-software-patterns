#ifndef MicroMiddleware_RmiMulticastServerListener_h_Included
#define MicroMiddleware_RmiMulticastServerListener_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/Net/RmiMulticast.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/Net/RmiServer.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RmiMulticastServer;

class DLL_STATE RmiMulticastServerListener : public RmiBase, public Thread
{
public:
	RmiMulticastServerListener(const InterfaceHandle &interfaceHandle, bool autoStart = false);
	virtual ~RmiMulticastServerListener();
	
	virtual void run();
	
	virtual void RMInvocation(int methodId, RmiMulticastServer *rmiServer);
	void RemoveServer(int socketDescriptor);

	virtual void AddTarget(InterfaceHandle &handle);
	virtual void RemoveTarget(InterfaceHandle &handle);

public:
	inline void SetMulticastClient(RmiMulticast *rmiMulticast) 	
	{	
		MutexLocker lock(&clientMutex_); 
		rmiMulticastClient_ = rmiMulticast; 
	}

private:
	void cleanUp();
	void initRmiMulticastServerListener();
	void addMulticastClient(const InterfaceHandle &handle);
	void removeMulticastClient(const InterfaceHandle &handle);

	inline bool isRunning()	{ return runThread_; }

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	Mutex             	clientMutex_;
	TcpServer			*tcpServer_;
	RmiMulticast		*rmiMulticastClient_;

	typedef std::map<int, RmiMulticastServer*>	MapRmiServer;
	MapRmiServer				mapRmiServer_;	
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiMulticastServerListener_h_Included


