#ifndef MicroMiddleware_RmiBase_h_Included
#define MicroMiddleware_RmiBase_h_Included

#include "MicroMiddleware/IncludeExtLibs.h"

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
class RmiObjectBase;

// Thread-Safe scenarios:
// RmiBase has to be made thread-safe if SocketStream/Writer/Reader are to be shared between Client/Server.
// 
// No thread-safety scenarios:
//  - SocketStream/Writer/Reader can be shared if Client/Server multicast through one way interfaces.
// 		That is: Client only writes data, Server only reads data.

class DLL_STATE RmiBase
{
public:	
	enum CommMode 
	{ 
		UnencryptedMode = 0,
		SslClientMode,
		SslServerMode
	};

protected:
	RmiBase(RmiBase::CommMode mode = RmiBase::UnencryptedMode);
	virtual ~RmiBase();

	void InitSocketObjects(AbstractSocket::Ptr);
	void InitSocketObjects(AbstractSocket::Ptr, string hostAddress, int port);
	virtual void CleanUp();

public:
	void ExecuteOneWayRMI(short methodId, RmiObjectBase *obj);
	void ExecuteTwoWayRMI(short methodId, RmiObjectBase *obj);
	
	void ReceiveRMI(RmiObjectBase *obj);
	void ReplyRMI(RmiObjectBase *obj);

public:
	bool IsConnected();
	bool IsBound();

	void BeginMarshal(short);   
	void EndMarshal();      	

	short BeginUnmarshal();  	
	void EndUnmarshal();    	

	SocketWriter *GetSocketWriter() { return socketWriter_; }
	SocketReader *GetSocketReader() { return socketReader_; }

	void 				SetCommMode(RmiBase::CommMode mode);
	RmiBase::CommMode	GetCommMode() const;

private:
	void MarshalObject(RmiObjectBase *obj);
	void UnmarshalObject(RmiObjectBase *obj);

	void cleanUp();

protected:
	SocketStream  	*socketStream_;
	SocketWriter 	*socketWriter_;
	SocketReader 	*socketReader_;
	
	short			myHostEndian;
	CommMode		commMode_;

	WaitCondition 	rmiMonitor_;   
	mutable Mutex	rmiMutex_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiBase_h_Included


