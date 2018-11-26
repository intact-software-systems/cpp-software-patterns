#include"MicroMiddleware/INet/INetBase.h"
#include"MicroMiddleware/RmiObjectBase.h"

#define INET_KEEPALIVE_INTERVAL_SEC	10

namespace MicroMiddleware
{

// --------------------------------------------------------------
//		constructor/destructor
// --------------------------------------------------------------
INetBase::INetBase(SocketStream *socketStream, const InterfaceHandle &handle) 
try
	: aliveTimer_(INET_KEEPALIVE_INTERVAL_SEC)
	, socketStream_(SocketStream::Ptr(socketStream))
	, abstractSocket_()
	, handle_(handle)
	, readingFinished_(false)
	, myHostEndian(0)
{
	ASSERT(socketStream_);
	ASSERT(handle.IsValid());
	
	socketWriter_ = SerializeWriter::Ptr(new SocketWriter(socketStream_.get()));
	headerWriter_ = SerializeWriter::Ptr(new SocketWriter(socketStream_.get()));
	socketReader_ = SerializeReader::Ptr(new SocketReader(socketStream_.get()));
}
catch(Exception e)
{
	ICRITICAL() << "Exception caught:" << e.msg();
}

INetBase::INetBase(const InterfaceHandle &handle) 
	: aliveTimer_(INET_KEEPALIVE_INTERVAL_SEC)
	, abstractSocket_()
	, handle_(handle)
	, readingFinished_(false)
	, myHostEndian(0)
{

}

INetBase::~INetBase()
{
	MutexLocker lock(&rmiMutex_);
	cleanUp();
}

// --------------------------------------------------------------
//		private class functions
// --------------------------------------------------------------
void INetBase::cleanUp()
{
	readingFinished_ = false;
}

void INetBase::InitSocketObjects(AbstractSocket::Ptr abstractSocket)
{
	try
	{
		ASSERT(abstractSocket);
		
		MutexLocker lock(&rmiMutex_);
		MutexLocker readLock(&readMutex_);
		MutexLocker writeLock(&writeMutex_);
		
		cleanUp();

		handle_.SetPortNumber(abstractSocket->GetPortNumber());
		handle_.SetHostName(abstractSocket->GetHostName());

		socketStream_ = SocketStream::Ptr(new SocketStream(abstractSocket));
		socketWriter_ = SerializeWriter::Ptr(new SocketWriter(socketStream_.get()));
		headerWriter_ = SerializeWriter::Ptr(new SocketWriter(socketStream_.get()));
		socketReader_ = SerializeReader::Ptr(new SocketReader(socketStream_.get()));
		
		abstractSocket_ = abstractSocket;

		aliveTimer_.Restart();
	}
	catch(Exception ex)
	{
		IWARNING() << " Exception caught " << ex.msg();
	}
	catch(...)
	{
		IWARNING() << "WARNING! Unhandled exception caught!";
	}
}

// --------------------------------------------------------------
//	public class functions
// --------------------------------------------------------------
int INetBase::socketDescriptor() const
{
	MutexLocker lock(&rmiMutex_);
	if(!socketStream_) return -1;
	else if(socketStream_->IsConnected() == false) return -1;

	return socketStream_->socketDescriptor();
}

bool INetBase::IsConnected() const
{
	MutexLocker lock(&rmiMutex_);
	if(!socketStream_) return false;

	return socketStream_->IsConnected();
}

bool INetBase::IsBound() const
{
	MutexLocker lock(&rmiMutex_);
	if(!socketStream_) return false;

	return socketStream_->IsBound();
}

bool INetBase::IsBufferedRead() const
{
	MutexLocker lock(&rmiMutex_);
	if(!socketReader_) return false;

	return socketReader_->IsBuffered();
}

bool INetBase::IsBufferedWrite() const
{
	MutexLocker lock(&rmiMutex_);
	if(!socketWriter_) return false;

	return socketWriter_->IsBuffered();
}

void INetBase::Close()
{
	try
	{
		rmiMutex_.lock();
			cleanUp();
		rmiMutex_.unlock();
	}
	catch(...)
	{
		rmiMutex_.unlock();
		ICRITICAL() << " Exception occured when closing connection!"; 
	}

	NotifyReadWait();
}

bool INetBase::DoKeepAlive()
{
	MutexLocker lock(&rmiMutex_);
	// Only send keep alive if it has been more than xx seconds since last activity
	return aliveTimer_.HasExpired();
}

// --------------------------------------------------------------
//	public RPC functions
// --------------------------------------------------------------
bool INetBase::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	ASSERT(rpc);
	
	SerializeWriter::Ptr writer;
	
	try
	{
		bool ret = BeginMarshal(rpc, writer);
		if(ret == false) 
		{
			IWARNING() << " No valid SocketStream is initialized!";
			return false;
		}

		// -- debug --
		ASSERT(writer);
		// -- debug --

		if(obj != NULL)
		{
			obj->Write(writer.get());
			if(obj->HasOutAddresses())
			{
				return EndMarshal(writer, obj->GetOutNetAddresses());
			}
		}
	}
	catch(Exception ex)
	{
		IWARNING() << " Exception caught " << ex.msg();
	}
	catch(...)
	{
		IWARNING() << "WARNING! Unhandled exception caught!";
	}

	return EndMarshal(writer);
}
// --------------------------------------------------------------
//	public receive RPC object
// --------------------------------------------------------------
bool INetBase::ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &reader)
{
	ASSERT(obj);
	ASSERT(reader);

	try
	{
		obj->Read(reader.get());
		obj->SetInNetAddress(handle_.GetNetAddress());

		return true;
	}
	catch(Exception ex)
	{
		IWARNING() << " Exception caught " << ex.msg();
	}
	catch(...)
	{
		IWARNING() << "WARNING! Unhandled exception caught!";
	}
	return false;
}

// --------------------------------------------------------------
//	public BeginMarshal 
// --------------------------------------------------------------
bool INetBase::BeginMarshal(RpcIdentifier *obj, SerializeWriter::Ptr &writer) 
{
	ASSERT(!writer);

	try
	{
		writeMutex_.lock(); 
	
		if(socketWriter_ == NULL)
		{
			writeMutex_.unlock();
			return false;
		}

		ASSERT(socketWriter_);
		ASSERT(socketReader_);
		ASSERT(socketStream_);
		ASSERT(obj);

		if(socketWriter_->IsBuffered() == true)	
		{
			// save current rpc and write to socket when EndMarshal is called
			currentRpc_ = *obj;
		}
		else // write directly to socket
		{
			obj->Write(socketWriter_.get()); 
		}

		writer = socketWriter_;
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught " << ex.msg();
		writeMutex_.unlock(); 
		return false;
	}
	catch(...)
	{
		IWARNING() << "Unhandled exception caught!";
		writeMutex_.unlock(); 
		return false;
	}

	return true;
}

// --------------------------------------------------------------
//	public EndMarshal 
// --------------------------------------------------------------
bool INetBase::EndMarshal(SerializeWriter::Ptr &writer) 
{
	bool marshalState = true;
	
	try
	{
		ASSERT(writeMutex_.tryLock() == false);
		ASSERT(writer);
	
		if(writer == NULL)
		{
			writeMutex_.unlock();
			return false;
		}

		if(writer->IsBuffered() == true)
		{
			ASSERT(currentRpc_.IsValid());
			ASSERT(headerWriter_);

			currentRpc_.SetRpcSize(writer->GetLength());
			currentRpc_.Write(headerWriter_.get());
			
			bool ret = headerWriter_->Flush(socketStream_.get());
			ASSERT(ret);
		}

		bool ret = writer->Flush(socketStream_.get());
		ASSERT(ret);

		aliveTimer_.Update();
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught " << ex.msg();
		marshalState = false;
	}
	catch(...)
	{
		IWARNING() << "Unhandled exception caught!";
		marshalState = false;
	}

	writeMutex_.unlock(); 
	
	return marshalState;
}

// --------------------------------------------------------------
//	public EndMarshal 
// --------------------------------------------------------------
bool INetBase::EndMarshal(SerializeWriter::Ptr &writer, const ListNetAddress &listAddresses) 
{
	bool marshalState = true;
	
	try
	{
		ASSERT(writeMutex_.tryLock() == false);
		ASSERT(writer);
	
		if(writer == NULL)
		{
			writeMutex_.unlock();
			return false;
		}

		for(ListNetAddress::const_iterator it = listAddresses.begin(), it_end = listAddresses.end(); it != it_end; ++it)
		{
			const NetAddress& netAddress = *it;

			if(writer->IsBuffered() == true)
			{
				ASSERT(currentRpc_.IsValid());
				ASSERT(headerWriter_);

				currentRpc_.SetRpcSize(writer->GetLength());
				currentRpc_.Write(headerWriter_.get());
				
				bool ret = headerWriter_->Flush(socketStream_.get(), netAddress.GetHostName(), netAddress.GetPortNumber());
				if(ret == false)
				{
					throw Exception("Flush failed!");
				}
			}

			bool ret = writer->Flush(socketStream_.get(), netAddress.GetHostName(), netAddress.GetPortNumber());
			if(ret == false)
			{
				throw Exception("Flush failed!");
			}

			aliveTimer_.Update();
		}

		writer->Clear();
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught " << ex.msg();
		marshalState = false;
	}
	catch(...)
	{
		IWARNING() << "Unhandled exception caught!";
		marshalState = false;
	}

	writeMutex_.unlock(); 
	
	return marshalState;
}

// --------------------------------------------------------------
//	protected BeginUnmarshal accessed only from INetConnection
// --------------------------------------------------------------
bool INetBase::BeginUnmarshal(RpcIdentifier *rpc, SerializeReader::Ptr &reader) 
{
	ASSERT(!reader);

	if(!socketReader_)
	{
		readingFinished_ = true;
		NotifyReadWait();
		return false;
	}

	ASSERT(socketReader_);
	ASSERT(rpc);
	
	try
	{
		while(readMutex_.tryLock() == false)
		{
			IWARNING() << "Cannot get readMutex!";
            Thread::usleep(2000);
		}
		
		// -- debug --
		ASSERT(readingFinished_ == true);
		// -- debug --

		readingFinished_ = false;
		rpc->Read(socketReader_.get());

		reader = socketReader_;

		return true;
	}
	catch(Exception ex)
	{
		readMutex_.unlock(); 	
		IWARNING() << " Exception caught " << ex.msg();
	}
	catch(...)
	{
		readMutex_.unlock(); 	
		IWARNING() << "WARNING! Unhandled exception caught!";
	}
	return false;
}

// --------------------------------------------------------------
//	public EndUnmarshal 
// --------------------------------------------------------------
void INetBase::EndUnmarshal(SerializeReader::Ptr &reader)
{
	ASSERT(readingFinished_ == false);
	readingFinished_ = true;

	ASSERT(readMutex_.tryLock() == false);
	readMutex_.unlock();
	
	NotifyReadWait();

	aliveTimer_.Update();
}

void INetBase::ReadLock()
{
	RpcReadingFinishedLock();
}

void INetBase::ReadUnlock()
{
	bool ret = WaitReadFinished();
	ASSERT(ret);
	
	RpcReadingFinishedUnlock();
}

void INetBase::NotifyReadWait()
{
	//	rpcReadingFinishedMutex_.lock();
	while(rpcReadingFinishedMutex_.tryLock() == false)
	{
		IWARNING() << "Cannot get rpcReadingFinishedMutex_!";
        Thread::usleep(2000);
	}
	ASSERT(rpcReadingFinishedMutex_.tryLock() == false);

	waitRpcReadingFinished_.wakeAll();
	rpcReadingFinishedMutex_.unlock();
}

bool INetBase::WaitReadFinished(int ms)
{
	ASSERT(rpcReadingFinishedMutex_.tryLock() == false);
	ASSERT(ms >= 0);

	while(!readingFinished_)
	{
		bool signalled = waitRpcReadingFinished_.wait(&rpcReadingFinishedMutex_, ms);
		if(readingFinished_ == true) break;

		if(IsConnected() == false)
		{
			IWARNING() << "WARNING! Connection lost!";
			return false;
		}
		if(signalled == false)
			IWARNING() << "Still waiting for Stub to finish reading!";
	}
	return true;
}

}; // namespace MicroMiddleware




