#include"MicroMiddleware/Net/AlmForwarder.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/Net/AlmMulticastPublisher.h"

namespace MicroMiddleware
{

AlmForwarder::AlmForwarder(AlmMulticastPublisher *almMulticast) 
	: runThread_(true)
	, almMulticastPublisher_(almMulticast)
{
	start();
}

void AlmForwarder::run()
{
	IWARNING() << "running";
	
	try
	{
		clientMutex_.lock();
		while(runThread_)
		{
			if(queueRpcWrapper_.empty())
				this->waitCondition_.wait(&clientMutex_, 5000);
			
			if(queueRpcWrapper_.empty()) continue;
			
			// -- forward/multicast messages to neighbors --
			RpcWrapper &rpc = queueRpcWrapper_.front();

			IDEBUG() << " Forwarding : " << rpc ;
			
			// TODO: Should I really forward or is it old?
			// 1. Check if I am a leaf node -> then discard
			// 2. Do not send back to publisher -> infinite loop! 
			// 3. Do not forward already seen sequence number -> it is old?

			// if yes -> forward
			almMulticastPublisher_->ExecuteRpc(rpc.GetRpcIdentifier(), rpc.GetRmiObjectBase(), rpc.GetGroupName());
	
			// TODO: if no -> discard

			// if all goes well
			queueRpcWrapper_.pop_front();
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception! " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "Exception! " ;
	}

	clientMutex_.unlock();

	IDEBUG() << " stopped" ;
}

void AlmForwarder::Stop(bool waitForTermination)
{
}

void AlmForwarder::Start()
{
	if(!IsRunning())
		start();
}

void AlmForwarder::SetAlmMulticastPublisher(AlmMulticastPublisher *alm, bool startThread)	
{
	ASSERT(alm);
	
	clientMutex_.lock();
	
	if(almMulticastPublisher_)
		IWARNING()  << "WARNING! multicast publisher already set!" ;
	almMulticastPublisher_ = alm;
	
	if(!IsRunning() && startThread)
		start();

	clientMutex_.unlock();
}

// TODO: Use thread-safe queue!
void AlmForwarder::ForwardRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	queueRpcWrapper_.push_back(RpcWrapper(rpc, rpcObject, groupName));

	waitCondition_.wakeAll();
}

}; // namespace MicroMiddleware




