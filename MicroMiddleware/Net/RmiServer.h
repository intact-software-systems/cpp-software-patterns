#ifndef MicroMiddleware_RmiServer_h_Included
#define MicroMiddleware_RmiServer_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{
class RmiServerListener;

class DLL_STATE RmiServer : public RmiBase, public Thread 
{
public:
	RmiServer(RmiServerListener *rmiSL, int socketDescriptor, bool autoStart = true);
	virtual ~RmiServer();

private:
	virtual void run();

protected:
	virtual void InvokeRMI(int methodId);
	
private:
	void initServer();
	void cleanUp();

protected:
	bool            	runThread_;
	InterfaceHandle 	interfaceHandle_;
	Mutex          		serverMutex_;
	TcpSocket::Ptr		tcpSocket_;
	int					socketDescriptor_;
	RmiServerListener 	*rmiListener_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_RmiServer_h_Included


