#ifndef MicroMiddleware_RpcClient_h_Included
#define MicroMiddleware_RpcClient_h_Included

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class DLL_STATE RpcClient : public RpcBase, protected Thread
{
public:
	RpcClient(const InterfaceHandle &interfaceHandle, bool autoStart = true);
	virtual ~RpcClient();

	void StopClient(bool waitForTermination = true);
	void StartClient();

	bool ConnectToHost();
	bool WaitConnected();
	
	virtual void AddTarget(GroupHandle &handle);
	virtual void RemoveTarget(GroupHandle &handle);

public:
	inline InterfaceHandle 	GetInterfaceHandle()		{ return interfaceHandle_; }
	inline bool 			IsRunning()					{ return isRunning(); }
	
protected:
	virtual void run();

private:
	bool connectToHost();
	void initTcpSocket();

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	mutable Mutex       clientMutex_;
	TcpSocket::Ptr		tcpSocket_;
	RpcIdentifier		rpcAddTarget_;
	RpcIdentifier		rpcRemoveTarget_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcClient_h_Included



