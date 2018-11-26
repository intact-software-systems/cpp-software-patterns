#ifndef MicroMiddleware_RmiSharedBase_h_Included
#define MicroMiddleware_RmiSharedBase_h_Included

#include "MicroMiddleware/IncludeExtLibs.h"

#include"MicroMiddleware/Export.h"

#if 0
namespace MicroMiddleware
{
class RmiObjectBase;

// Thread-Safe scenarios:
// RmiSharedBase has to be made thread-safe if SocketStream/Writer/Reader are to be shared between Client/Server.
// 
// No thread-safety scenarios:
//  - SocketStream/Writer/Reader can be shared if Client/Server multicast through one way interfaces.
// 		That is: Client only writes data, Server only reads data.

class RmiSharedBase;

//-------------------------------------------------------------------------------
//		RMI Sender 
//-------------------------------------------------------------------------------
class DLL_STATE RmiSender : public RmiSenderBase, public Thread
{
public:
	RmiSender(RmiSharedBase* base) : rmiSharedBase_(base) 
	{ 
		start(); 
	}
	virtual ~RmiSender() {}

	virtual void run();
	
	virtual void ExecuteOneWayRMI(short methodId, RmiObjectBase *obj);
	virtual void ExecuteTwoWayRMI(short methodId, RmiObjectBase *obj);

private:
	RmiSharedBase *rmiSharedBase_;
	WaitCondition 	senderMonitor_;   
	Mutex			senderMutex_;
};

//-------------------------------------------------------------------------------
//		RMI Receiver 
//-------------------------------------------------------------------------------
class DLL_STATE RmiReceiver : public RmiReceiverBase, public Thread
{
public:
	RmiReceiver(RmiSharedBase* base) : rmiSharedBase_(base)
	{
		start();
	}
	virtual ~RmiReceiver() {}

	virtual void run();

	virtual void ReceiveRMI(RmiObjectBase *obj);
	virtual void ReplyRMI(RmiObjectBase *obj);

private:
	RmiSharedBase 	*rmiSharedBase_;
	WaitCondition 	receiverMonitor_;   
	Mutex			receiverMutex_;
};

//-------------------------------------------------------------------------------
//		RMI Shared Base 
//		RmiSenderReceiverBase
//		- Creates the RmiReceiver and RmiSender
//-------------------------------------------------------------------------------
class DLL_STATE RmiSenderReceiver : public Thread
{
protected:
	RmiSenderReceiver(const InterfaceHandle &handle);
	virtual ~RmiSenderReceiver();

	virtual void run();

public:
	void 			StartRmi(short methodId, RmiObjectBase *object);
	SocketReader* 	BeginUnmarshal(short methodId, short objectId);

private:
	void			initListener();

private:
	InterfaceHandle		interfaceHandle_;
	RmiSender 			*rmiSender_;
	RmiReceiver 		*rmiReceiver_;
	Mutex             	objectMutex;
	TcpServer			*tcpServer_;

	typedef std::map<int, RmiMulticastServer*>	MapRmiServer;
	MapRmiServer				mapRmiServer_;	};

}; // namespace MicroMiddleware

#endif

#endif // MicroMiddleware_RmiSharedBase_h_Included



