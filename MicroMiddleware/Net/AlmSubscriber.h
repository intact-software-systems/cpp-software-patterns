#ifndef MicroMiddleware_AlmSubscriber_h_Included
#define MicroMiddleware_AlmSubscriber_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/Net/AlmServer.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RpcMulticastServer;
class AlmServer;
class AlmMulticastPublisher;
class AlmForwarder;

class DLL_STATE AlmSubscriber : public RpcBase, public Thread
{
public:
	AlmSubscriber(const InterfaceHandle &interfaceHandle, AlmMulticastPublisher *client, bool autoStart = true);
	virtual ~AlmSubscriber();
	
	friend class RpcMulticastServer;
	
	void StopServer();
	bool RegisterInterface(int interfaceId, AlmServer *almServer);

	void SetAlmForwarder(AlmForwarder *almForwarder);
	//void RemoveAlmForwarder();	// TODO: Use SetAlmForwarder(NULL)?

private:
	virtual void run();

	virtual void RPCall(const RpcIdentifier &rpc);
	
	void AddTarget(GroupHandle &handle);
	void RemoveTarget(GroupHandle &handle);

private:
	void cleanUp();
	void initAlmSubscriber();

	inline bool isRunning()	{ return runThread_; }

protected:
	bool              		runThread_;
	InterfaceHandle			interfaceHandle_;
	mutable Mutex           clientMutex_;
	UdpSocket::Ptr			udpSocket_;
	AlmMulticastPublisher 	*multicastPublisher_;
	AlmForwarder			*almForwarder_;

	typedef std::map<int, AlmServer*>			MapInterfaceIdAlmServer;
	MapInterfaceIdAlmServer						mapInterfaceIdAlmServer_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_AlmSubscriber_h_Included




