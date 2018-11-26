#ifndef MicroMiddleware_RmiServerListener_h_Included
#define MicroMiddleware_RmiServerListener_h_Included

#include<vector>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class RmiServer;
class RmiSslServer;

class DLL_STATE RmiServerListener : public Thread
{
public:
	RmiServerListener(const InterfaceHandle &interfaceHandle, bool autoStart = true, RmiBase::CommMode mode = RmiBase::UnencryptedMode);
	RmiServerListener(const string &serverName, const InterfaceHandle &interfaceHandle = InterfaceHandle(), bool autoStart = true, RmiBase::CommMode mode = RmiBase::UnencryptedMode);
	virtual ~RmiServerListener();

public:
	void StartOrRestartServer(const InterfaceHandle &in);
	void StopServer(bool waitForTermination = false);
	bool WaitStarted();

	virtual void RMInvocation(int methodId, RmiBase *rmiBase);

	void RemoveServer(int socketDescriptor);

	InterfaceHandle GetInterfaceHandle()	const { MutexLocker lock(&mutex_); return interfaceHandle_; }
	HostInformation GetServerInformation()	const { MutexLocker lock(&mutex_); return serverInformation_; }
	string			GetServerName()			const { MutexLocker lock(&mutex_); return serverName_; }

protected:
	// reimplement these in Stub and register with ServiceManager!!!
	virtual void ServerStartedListening() { }
	virtual void ServerStoppedListening() { }

private:
	virtual void run();

private:
	void startOrRestartServer();
	void cleanUp();
	void removeOldServers();
	bool initServer();
	
	inline bool isRunning()	{ return runThread_; }

protected:
	string				serverName_;
	HostInformation		serverInformation_;
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	RmiBase::CommMode	commMode_;
	
	mutable Mutex       mutex_;
	WaitCondition		waitCondition_;

	TcpServer			*tcpServer_;

protected:
	typedef std::map<int, RmiBase*>	MapRmiServer;
	
	MapRmiServer		mapRmiServer_;
	set<int>			removeRmiServer_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiServerListener_h_Included


