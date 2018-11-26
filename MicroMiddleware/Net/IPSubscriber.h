#ifndef MicroMiddleware_IPSubscriber_h_Included
#define MicroMiddleware_IPSubscriber_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{
class IPSubscriberListener;

class DLL_STATE IPSubscriber : public RmiBase, public Thread 
{
public:
	IPSubscriber(IPSubscriberListener *rmiSL, const InterfaceHandle &handle, bool autoStart = true);
	virtual ~IPSubscriber();

	virtual void run();

	virtual void InvokeRMI(int methodId);
	
	void StartOrRestart(const InterfaceHandle &);
	void Stop(bool waitForTermination = true);
	
	InterfaceHandle GetInterfaceHandle()	const { MutexLocker lock(&serverMutex_); return interfaceHandle_; }

private:
	void startOrRestart();
	void initIPSubscriber();
	
	inline void lock()				{ serverMutex_.lock(); }
	inline void unlock()			{ serverMutex_.unlock(); }

protected:
	bool            		runThread_;
	InterfaceHandle 		interfaceHandle_;
	mutable Mutex          	serverMutex_;
	UdpSocket::Ptr			udpSocket_;
	int						socketDescriptor_;
	IPSubscriberListener 	*rmiListener_;
};
}; // namespace MicroMiddleware

#endif // MicroMiddleware_IPSubscriber_h_Included



