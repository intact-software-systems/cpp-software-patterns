#ifndef MicroMiddleware_AlmForwarder_h_Included
#define MicroMiddleware_AlmForwarder_h_Included

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/GroupHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class AlmMulticastPublisher;

class DLL_STATE RpcWrapper
{
public:
	RpcWrapper(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
			: rpcIdentifier_(rpc)
			, rpcObject_(rpcObject)
			, groupName_(groupName)
	{ 
		IWARNING()  << "WARNING! This constructor is unsafe! Use shared_ptr";
	}
	~RpcWrapper() 
	{ }

	inline RpcIdentifier* 	GetRpcIdentifier()	const { return rpcIdentifier_; }
	inline RmiObjectBase* 	GetRmiObjectBase()	const { return rpcObject_; }
	inline std::string		GetGroupName()		const { return groupName_; }

	friend std::ostream& operator<<(std::ostream &ostr, const RpcWrapper &rpc)
	{
		ostr << rpc.GetRpcIdentifier() << " group name : " << rpc.GetGroupName() ;
		return ostr;
	}

protected:
	RpcIdentifier 	*rpcIdentifier_;
	RmiObjectBase 	*rpcObject_;
	std::string		groupName_;
};

class DLL_STATE AlmForwarder : protected Thread
{
public:
	AlmForwarder(AlmMulticastPublisher *almMulticast);
	virtual ~AlmForwarder()
	{}
	
	// TODO: Use thread-safe queue!
	typedef list<RpcWrapper>		VectorRpcWrapper;
	
	void Stop(bool waitForTermination = true);
	void Start();
	
	void ForwardRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName);
	void SetAlmMulticastPublisher(AlmMulticastPublisher *alm, bool startThread = true);

public:
	inline bool 			IsRunning()					{ return Thread::isRunning(); }

    bool lockedWait(int milliseconds = 1000)
	{
		clientMutex_.lock();
		bool ret = waitCondition_.wait(&clientMutex_, milliseconds);

		return ret;
	}

protected:
	virtual void run();

protected:
	bool              		runThread_;
	mutable Mutex           clientMutex_;
	WaitCondition			waitCondition_;
	
	AlmMulticastPublisher	*almMulticastPublisher_;
	VectorRpcWrapper		queueRpcWrapper_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_AlmForwarder_h_Included





