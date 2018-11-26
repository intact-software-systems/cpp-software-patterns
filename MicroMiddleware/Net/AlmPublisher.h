#ifndef MicroMiddleware_AlmPublisher_h_Included
#define MicroMiddleware_AlmPublisher_h_Included

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

// TODO: Having one publisher for each subscriber is a waste for UDP connections!
// 	-> instead use writeDatagram using hostAddress
//  -> Specialize AlmBase to do this!

class DLL_STATE AlmPublisher : public RpcBase, protected Thread
{
public:
	AlmPublisher(const InterfaceHandle &interfaceHandle, bool autoStart = true);
	virtual ~AlmPublisher()
	{}

	void StopClient(bool waitForTermination = true);
	void StartClient();

	bool BindToInterfaceHandle();
	bool WaitConnected();
	bool WaitForUdpSocket();
	
	virtual void AddTarget(GroupHandle &handle);
	virtual void RemoveTarget(GroupHandle &handle);

public:
	inline InterfaceHandle 	GetInterfaceHandle()		{ return interfaceHandle_; }
	inline bool 			IsRunning()					{ return isRunning(); }
	
	inline void lock()				{ clientMutex_.lock(); }
	inline void unlock()			{ clientMutex_.unlock(); }

protected:
	virtual void run();

private:
	bool bindToInterfaceHandle();
	void initUdpSocket();

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	mutable Mutex       clientMutex_;
	UdpSocket::Ptr		udpSocket_;
	RpcIdentifier		rpcAddTarget_;
	RpcIdentifier		rpcRemoveTarget_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_AlmPublisher_h_Included




