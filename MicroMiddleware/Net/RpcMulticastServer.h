#ifndef MicroMiddleware_RpcMulticastServer_h_Included
#define MicroMiddleware_RpcMulticastServer_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/Net/RpcServer.h"

#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RpcMulticastServerListener;

class DLL_STATE RpcMulticastServer : public RpcBase, public Thread
{
public:
	RpcMulticastServer(RpcMulticastServerListener *rmiSL, int socketDescriptor, bool autoStart = true);
	virtual ~RpcMulticastServer();	

	virtual void run();

	virtual void InvokeRMI(const RpcIdentifier &rpc);

	void Stop();

private:
	void initRpcServer();

private:
	RpcMulticastServerListener *rmiMCListener_;
	bool            			runThread_;
	InterfaceHandle 			interfaceHandle_;
	Mutex          				serverMutex_;
	TcpSocket::Ptr              tcpSocket_;
	int							socketDescriptor_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcMulticastServer_h_Included



