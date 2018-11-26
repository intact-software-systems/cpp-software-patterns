#ifndef MicroMiddleware_RmiMulticastServer_h_Included
#define MicroMiddleware_RmiMulticastServer_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/Net/RmiServer.h"

#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RmiMulticastServerListener;

class DLL_STATE RmiMulticastServer : public RmiBase, public Thread
{
public:
	RmiMulticastServer(RmiMulticastServerListener *rmiSL, TcpSocket *tcpSocket, bool autoStart = true);
	virtual ~RmiMulticastServer();	

	virtual void run();

	virtual void InvokeRMI(int methodId);

	virtual void AddTarget(InterfaceHandle &handle);
	virtual void RemoveTarget(InterfaceHandle &handle);
	
	void StartOrRestartServer(const InterfaceHandle &);
	void StopServer(bool waitForTermination = true);
	
private:
	void startOrRestartServer();
	void initRmiServer();

private:
	RmiMulticastServerListener *rmiMCListener_;
	bool            			runThread_;
	InterfaceHandle 			interfaceHandle_;
	Mutex          				serverMutex_;
	TcpSocket::Ptr				tcpSocket_;
	int							socketDescriptor_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiMulticastServer_h_Included


