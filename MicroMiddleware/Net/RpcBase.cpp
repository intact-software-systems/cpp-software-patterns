#include"MicroMiddleware/Net/RpcBase.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"

namespace MicroMiddleware
{

RpcBase::RpcBase() : socketStream_(0), socketWriter_(0), socketReader_(0), myHostEndian(0), isClean_(true)
{

}
RpcBase::~RpcBase()
{
	MutexLocker lock(&rmiMutex_);
	cleanUp();
}

void RpcBase::cleanUp()
{
	if(isClean_ == true) return;
	
	delete socketStream_;
	delete socketWriter_;
	delete socketReader_;

	socketStream_ = NULL;
	socketWriter_ = NULL;
	socketReader_ = NULL;

	isClean_ = true;
}

void RpcBase::CleanUp()
{
	MutexLocker lock(&rmiMutex_);
	cleanUp();
}

void RpcBase::InitSocketObjects(AbstractSocket::Ptr abstractSocket)
{
	MutexLocker lock(&rmiMutex_);
	
	if(isClean_ == false)
		cleanUp();

	socketStream_ = new SocketStream(abstractSocket);
	socketWriter_ = new SocketWriter(socketStream_);
	socketReader_ = new SocketReader(socketStream_);

	isClean_ = false;
}

void RpcBase::InitSocketObjects(AbstractSocket::Ptr abstractSocket, string hostAddress, int port)
{
	MutexLocker lock(&rmiMutex_);
	
	if(isClean_ == false)
		cleanUp();

	socketStream_ = new SocketStream(abstractSocket);
	socketWriter_ = new SocketWriter(socketStream_, hostAddress, port);
	socketReader_ = new SocketReader(socketStream_, hostAddress, port);

	isClean_ = false;
}

bool RpcBase::IsConnected()
{
	MutexLocker lock(&rmiMutex_);
	if(socketStream_ == NULL) return false;

	return socketStream_->IsConnected();
}

bool RpcBase::IsBound()
{
	MutexLocker lock(&rmiMutex_);
	if(socketStream_ == NULL) return false;

	return socketStream_->IsBound();
}

void RpcBase::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	try
	{
		// Lock() ?
		BeginMarshal(rpc);

		//char* buf = obj->toRaw();
		//socketWriter_->Write(buf, obj->GetLength());
		obj->Write(socketWriter_);
		
		EndMarshal();
		// Unlock() ?
	}
	catch(Exception ex)
	{
		IWARNING()  << " Exception caught " << ex.msg() ;
		// Unlock()?
	}
}

/*void RpcBase::ExecuteRpc(RpcIdentifier *rpc, char *buf, int length)
{
	try
	{
		// Lock() ?
		BeginMarshal(rpc);
		socketWriter_->Write(buf, length);
		EndMarshal();
		// Unlock() ?
	}
	catch(Exception ex)
	{
		IWARNING()  << " Exception caught " << ex.msg() ;
		// Unlock()?
	}
}*/

void RpcBase::ReceiveRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	try
	{
		// Lock() ?
		BeginUnmarshal(rpc);
		obj->Read(socketReader_);
		EndUnmarshal();
		// Unlock() ?
	}
	catch(Exception ex)
	{
		IWARNING()  << " Exception caught " << ex.msg() ;
		// Unlock() ?
	}
}

void RpcBase::ReceiveRpcObject(RmiObjectBase *obj)
{
	try
	{
		// Lock() ?
		obj->Read(socketReader_);
		EndUnmarshal();
		// Unlock() ?
	}
	catch(Exception ex)
	{
		IWARNING()  << " Exception caught " << ex.msg() ;
		// Unlock() ?
	}
}

void RpcBase::BeginMarshal(RpcIdentifier *obj) 
{
	//ASSERT(IsConnected() || IsBound());
	obj->Write(socketWriter_);
	//char* buf = obj->toRaw();
	//socketWriter_->Write(buf, obj->GetLength());
}

void RpcBase::EndMarshal() 
{
	socketWriter_->Flush();
}

void RpcBase::BeginUnmarshal(RpcIdentifier *rpc) 
{
	ASSERT(socketReader_);
	rpc->Read(socketReader_);
}

void RpcBase::EndUnmarshal()
{

}


}; // namespace MicroMiddleware



