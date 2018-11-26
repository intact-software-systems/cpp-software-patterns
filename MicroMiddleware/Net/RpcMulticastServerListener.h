#ifndef MicroMiddleware_RpcMulticastServerListener_h_Included
#define MicroMiddleware_RpcMulticastServerListener_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/Net/RpcServer.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RpcMulticastServer;
class RpcServer;
class RpcMulticastClient;

class DLL_STATE RpcMulticastServerListener : public RpcBase, public Thread
{
public:
	RpcMulticastServerListener(const InterfaceHandle &interfaceHandle, RpcMulticastClient *client = NULL, bool autoStart = true);
	//RpcMulticastServerListener(const string &serverName, const InterfaceHandle &interfaceHandle = InterfaceHandle(), RpcMulticastClient *client, bool autoStart = true);
	virtual ~RpcMulticastServerListener();
	
	friend class RpcMulticastServer;
	
	void StopServer();
	bool RegisterInterface(int interfaceId, RpcServer *rpcServer);
		
	InterfaceHandle GetInterfaceHandle()	const { MutexLocker lock(&mutex_); return interfaceHandle_; }
	HostInformation GetServerInformation()	const { MutexLocker lock(&mutex_); return serverInformation_; }
	string			GetServerName()			const { MutexLocker lock(&mutex_); return serverName_; }

	void			SetMulticastClient(RpcMulticastClient *multicastClient);

	bool WaitStarted();

private:
	virtual void run();

	virtual void RPCall(const RpcIdentifier &rpc, RpcMulticastServer *rmiServer);
	
	void AddTarget(GroupHandle &handle);
	void RemoveTarget(GroupHandle &handle);
	void RemoveServer(int socketDescriptor);

private:
	void cleanUp();
	void removeOldServers();
	bool initServer();

	inline bool isRunning()	{ return runThread_; }

protected:
	string				serverName_;
	HostInformation		serverInformation_;	
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	
	mutable Mutex       mutex_;
	WaitCondition		waitCondition_;

	TcpServer			*tcpServer_;
	RpcMulticastClient  *multicastClient_;

	typedef std::map<int, RpcMulticastServer*>	MapRpcMulticastServer;
	MapRpcMulticastServer						mapRpcMulticastServer_;	
	set<int>									removeRpcMulticastServer_;

	typedef std::map<int, RpcServer*>			MapInterfaceIdRpcServer;
	MapInterfaceIdRpcServer						mapInterfaceIdRpcServer_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcMulticastServerListener_h_Included



