#ifndef MicroMiddleware_IPPublisher_h_Included
#define MicroMiddleware_IPPublisher_h_Included

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#define IPPUBLISHER_COMMON(IPPublisherClass)													\
	IPPublisherClass(const MicroMiddleware::InterfaceHandle &handle, bool autoStart = true)		\
			: IPPublisher(handle, autoStart) 													\
	{} 																							\
	virtual ~IPPublisherClass() {}															


#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class DLL_STATE IPPublisher : public RmiBase, protected Thread
{
public:
	IPPublisher(const InterfaceHandle &interfaceHandle, bool autoStart = true);
	virtual ~IPPublisher()
	{}

	void StopClient(bool waitForTermination = true);
	void StartClient();

	bool BindToInterfaceHandle();
	bool WaitConnected();
	bool WaitForUdpSocket();
	
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
	Mutex             	clientMutex_;
	UdpSocket::Ptr		udpSocket_;
};


}; // namespace MicroMiddleware

#endif // MicroMiddleware_IPPublisher_h_Included


