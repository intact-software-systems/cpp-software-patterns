#ifndef MicroMiddleware_RmiMulticastClient_h_Included
#define MicroMiddleware_RmiMulticastClient_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/Net/RmiMulticast.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

/*
 * class RmiMulticastClient
 * 		- Connect to 
 *
 */
class DLL_STATE RmiMulticastClient : public RmiMulticast, public RmiBase, public Thread
{
public:
	RmiMulticastClient(const InterfaceHandle &interfaceHandle, bool autoStart = false)
		: runThread_(autoStart)
		, interfaceHandle_(interfaceHandle)
	{
		if(runThread_) start();
	}
	virtual ~RmiMulticastClient()
	{}
	
	void StopMulticastClient(bool waitForTermination = true);
	void StartOrRestartMulticastClient(const InterfaceHandle &handle);
	void StartOrRestartMulticastClient();

	void AddMulticastClients(GroupInformation &groupInfo);

protected:
	virtual void run();

private:
	bool connectToServer();
	virtual void RMInvocation(short methodId) = 0;

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	TcpSocket::Ptr		tcpSocket_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiMulticastClient_h_Included


