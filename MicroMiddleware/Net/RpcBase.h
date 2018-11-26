#ifndef MicroMiddleware_RpcBase_h_Included
#define MicroMiddleware_RpcBase_h_Included

#include "MicroMiddleware/IncludeExtLibs.h"

#include"MicroMiddleware/Export.h"

// TODO include these functions to increase efficiency:
//void WriteLock() {}
//void WriteUnlock() {}
//void ReadLock() {}
//void ReadUnlock() {}

// include try/catch in public functions that use socketwriter/socketreader to avoid dead-lock 
// when Lock() is used
namespace MicroMiddleware
{

class RmiObjectBase;
class RpcIdentifier;

class DLL_STATE RpcBase
{
protected:
	RpcBase();
	virtual ~RpcBase();

	void InitSocketObjects(AbstractSocket::Ptr );
	void InitSocketObjects(AbstractSocket::Ptr, string hostAddress, int port);

	virtual void CleanUp();

public:
	void Lock() 	{ rmiMutex_.lock(); }
	void Unlock() 	{ rmiMutex_.unlock(); }
	void Wait()		{ rmiMonitor_.wait(&rmiMutex_); }

	bool IsConnected();
	bool IsBound();
	
	void ExecuteRpc(RpcIdentifier *, RmiObjectBase *);

	void ReceiveRpc(RpcIdentifier *, RmiObjectBase *);
	void ReceiveRpcObject(RmiObjectBase *);

	void BeginMarshal(RpcIdentifier *);
	void EndMarshal();

	void BeginUnmarshal(RpcIdentifier *);
	void EndUnmarshal();

public:
	inline SocketWriter *GetSocketWriter() { return socketWriter_; }
	inline SocketReader *GetSocketReader() { return socketReader_; }

private:
	void cleanUp();

protected:
	SocketStream  	*socketStream_;
	SocketWriter 	*socketWriter_;
	SocketReader 	*socketReader_;
	short			myHostEndian;
	WaitCondition 	rmiMonitor_;   
	Mutex			rmiMutex_;

private:
	bool			isClean_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcBase_h_Included



