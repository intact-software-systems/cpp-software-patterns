#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/RmiObjectBase.h"

namespace MicroMiddleware
{

RmiBase::RmiBase(RmiBase::CommMode mode) 
	: socketStream_(0)
	, socketWriter_(0)
	, socketReader_(0)
	, myHostEndian(0)
	, commMode_(mode)
{

}
RmiBase::~RmiBase()
{
	//MutexLocker lock(&rmiMutex_);
	cleanUp();
}

void RmiBase::cleanUp()
{
	if(socketStream_) delete socketStream_;
	if(socketWriter_) delete socketWriter_;
	if(socketReader_) delete socketReader_;

	socketStream_ = NULL;
	socketWriter_ = NULL;
	socketReader_ = NULL;
}

void RmiBase::CleanUp()
{
	MutexLocker lock(&rmiMutex_);
	cleanUp();
}

void RmiBase::InitSocketObjects(AbstractSocket::Ptr abstractSocket)
{
	MutexLocker lock(&rmiMutex_);
	
	cleanUp();

	socketStream_ = new SocketStream(abstractSocket);
	socketWriter_ = new SocketWriter(socketStream_);
	socketReader_ = new SocketReader(socketStream_);
}

void RmiBase::InitSocketObjects(AbstractSocket::Ptr abstractSocket, string hostAddress, int port)
{
	MutexLocker lock(&rmiMutex_);
	
	cleanUp();

	socketStream_ = new SocketStream(abstractSocket);
	socketWriter_ = new SocketWriter(socketStream_, hostAddress, port);
	socketReader_ = new SocketReader(socketStream_, hostAddress, port);
}

bool RmiBase::IsConnected()
{
	MutexLocker lock(&rmiMutex_);
	if(socketStream_ == NULL) return false;

	ASSERT(socketStream_);
	return socketStream_->IsConnected();
}

bool RmiBase::IsBound()
{
	MutexLocker lock(&rmiMutex_);
	if(socketStream_ == NULL) return false;

	return socketStream_->IsBound();
}


void RmiBase::BeginMarshal(short methodID) 
{
	if(socketWriter_ == NULL) 
	{
		throw("not connected");
	}

	try
	{
		ASSERT(socketWriter_);
	
		int ret = socketWriter_->WriteInt16(myHostEndian);
		if(ret < 0) // not connected
		{
			cleanUp();
			throw("RmiBase::BeginMarshal(): not connected");
		}
		ret = socketWriter_->WriteInt16(methodID);
		if(ret < 0) // not connected
		{
			cleanUp();
			throw("RmiBase::BeginMarshal(): not connected");
		}
	}
	catch(Exception io)
	{
		cleanUp();
		throw;
	}
}

void RmiBase::EndMarshal() 
{
	socketWriter_->Flush();
}

short RmiBase::BeginUnmarshal() 
{
	try
	{
		ASSERT(socketReader_);
		short messageEndian = socketReader_->ReadInt16();
		short returnType 	= socketReader_->ReadInt16();
	
		if(returnType < RmiInterface::ReturnValue) 
		{
			IDEBUG() << "Exception occured on remote side! " << returnType ;
			string message = socketReader_->ReadString();
			throw Exception("Exception occured on remote side: " + message);
		}
		return returnType;
	}
	catch(Exception io)
	{
		cleanUp();
		throw;
	}
	return -1;
}

void RmiBase::EndUnmarshal()
{
}

void RmiBase::MarshalObject(RmiObjectBase *obj)
{
	ASSERT(socketWriter_);
	obj->Write(socketWriter_);
}

void RmiBase::UnmarshalObject(RmiObjectBase *obj)
{
	ASSERT(socketReader_);
	obj->Read(socketReader_);
}

void RmiBase::ExecuteOneWayRMI(short methodId, RmiObjectBase *obj)
{
	BeginMarshal(methodId);
	MarshalObject(obj);
	EndMarshal();
}

void RmiBase::ExecuteTwoWayRMI(short methodId, RmiObjectBase *obj)
{
	BeginMarshal(methodId);
	MarshalObject(obj);
	EndMarshal();

	BeginUnmarshal();
	UnmarshalObject(obj);
	EndUnmarshal();
}

void RmiBase::ReceiveRMI(RmiObjectBase *obj)
{

	UnmarshalObject(obj);
	EndUnmarshal();
}

void RmiBase::ReplyRMI(RmiObjectBase *obj)
{
	BeginMarshal(RmiInterface::ReturnValue);
	MarshalObject(obj);
	EndMarshal();
}

void RmiBase::SetCommMode(RmiBase::CommMode mode) 	
{ 
	MutexLocker lock(&rmiMutex_); 
	commMode_ = mode; 
}

RmiBase::CommMode RmiBase::GetCommMode() const 
{ 
	MutexLocker lock(&rmiMutex_); 
	return commMode_; 
}

} // namespace MicroMiddleware


