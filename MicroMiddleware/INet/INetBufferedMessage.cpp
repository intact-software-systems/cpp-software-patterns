#include"MicroMiddleware/INet/INetBufferedMessage.h"
#include"MicroMiddleware/RmiObjectBase.h"

#define INET_KEEPALIVE_INTERVAL_SEC	10

namespace MicroMiddleware
{

// --------------------------------------------------------------
//		constructor/destructor
//	- If UDP use UDPBufferedMessages
//	- If TCP use TCPBufferedMessages
// --------------------------------------------------------------
INetBufferedMessage::INetBufferedMessage() 
	: abstractSocket_()
	, aliveTimer_(INET_KEEPALIVE_INTERVAL_SEC)
	, runThread_(true)
{
}

INetBufferedMessage::~INetBufferedMessage()
{
	Close();
}

// --------------------------------------------------------------
//		private class functions
// --------------------------------------------------------------
bool INetBufferedMessage::newSerializeWriter(SerializeWriter::Ptr &writer)
{
	if(AbstractSocket::UdpSocket == socketStream_->GetSocketType())
	{
		writer = SerializeWriter::Ptr(new ByteArray());
	}
	else if(AbstractSocket::TcpSocket == socketStream_->GetSocketType())
	{
		writer = SerializeWriter::Ptr(new ByteArray());
	}
	else
	{
		ICRITICAL() << "Could not recognize socketStream type: " << socketStream_->GetSocketType();
		writer = SerializeWriter::Ptr(new ByteArray());
	}

	return true;
}

// --------------------------------------------------------------
//	- If UDP use UDPBufferedMessages
//	- If TCP use TCPBufferedMessages
//	- Restarts thread also!
//	- TODO: queueIncomingRpcMessages_.Clear() it?
//	- TODO: race-condition may occur if more than one thread accesses this object
//		- INetConnection and INetProxy/INetStub!
// --------------------------------------------------------------
void INetBufferedMessage::InitSocketObjects(AbstractSocket::Ptr abstractSocket)
{
	ASSERT(abstractSocket);
	
	try
	{
		MutexLocker lock(&mutex_);
		
		runThread_ = false;
		
		if(Thread::isRunning())
			Thread::wait();
			
		if(socketStream_) 			socketStream_->Close();
		if(bufferedSocketReader_) 	bufferedSocketReader_->Stop();
		if(bufferedSocketWriter_) 	bufferedSocketWriter_->Stop();
		
		handle_.SetPortNumber(abstractSocket->GetPortNumber());
		handle_.SetHostName(abstractSocket->GetHostName());

		socketStream_ = SocketStream::Ptr(new SocketStream(abstractSocket));
	
//		if(AbstractSocket::UdpSocket == socketStream_->GetSocketType())
//		{
//			bufferedSocketReader_ = BufferedSocketReader::Ptr(new BufferedUdpSocketReader(socketStream_.get()));
//			bufferedSocketWriter_ = BufferedSocketWriter::Ptr(new BufferedUdpSocketWriter(socketStream_.get()));
//		}
		if(AbstractSocket::TcpSocket == socketStream_->GetSocketType())
		{
			bufferedSocketReader_ = BufferedSocketReader::Ptr(new BufferedTcpSocketReader(socketStream_.get()));
			bufferedSocketWriter_ = BufferedSocketWriter::Ptr(new BufferedTcpSocketWriter(socketStream_.get()));
		}
		else
		{
			ICRITICAL() << "Could not recognize socketStream type: " << socketStream_->GetSocketType();
			bufferedSocketReader_ = BufferedSocketReader::Ptr(new BufferedTcpSocketReader(socketStream_.get()));
			bufferedSocketWriter_ = BufferedSocketWriter::Ptr(new BufferedTcpSocketWriter(socketStream_.get()));
		}
	
		abstractSocket_ = abstractSocket;
		runThread_ = true;
		
		ASSERT(!Thread::isRunning());

		start();

		aliveTimer_.Restart();
	}
	catch(Exception ex)
	{
		IWARNING() << " Exception caught " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!";
	}
}

// --------------------------------------------------------------
//	thread function
// TODO: Check if object is initialized
// TODO: the bufferedInMessage can also be used as a feedback mechanism for successfully acknowledged packets
// 		-> How should this be implemented?
// 		-> Associate a MessageType to the bufferedInMessage.
// --------------------------------------------------------------
void INetBufferedMessage::run()
{
	try
	{
		IDEBUG() << "started";

		while(runThread_)
		{
			// TODO:
			// This thread should be woken up when the Reader is "asking" for data
			// 	-> Implement this functionality using a WaitSet with Conditions?
			// If condition is Read -> Then Read from bufferedSocketReader_
			// If condition is Take -> Then Take from bufferedSocketReader_
			// ISSUE: BeginUnmarshal is currently only called from INetConnection!

			// 1. get a ByteArray from buffered NetworkLib::SeralizeWriter
			ASSERT(bufferedSocketReader_);
			NetworkLib::BufferedInMessage::Ptr bufferedInMessage = bufferedSocketReader_->PopMessage();
			
			// stop thread?
			if(runThread_ == false) break;

			ASSERT(bufferedInMessage);
			if(!bufferedInMessage) 
			{
				ICRITICAL() << "bufferedSocketReader_->PopMessage() returned NULL pointer!";
				continue;
			}
 
			// TODO: Check if message is a middleware (RTPS) message: ACK, etc.

			// 2. Parse ByteArray and retrieve RpcIdentifier. (if necessary)
			SerializeReader::Ptr serializeReader = bufferedInMessage->GetSerializeReader();
			RpcIdentifier rpc;
			rpc.Read(serializeReader.get());
			
			// 3. Push output queue to INetConnection waiting in BeginUnmarshal()
			PairRpcMessagePtr pairRpcMessagePtr = PairRpcMessagePtr(new PairRpcMessage(rpc, serializeReader));
			queueIncomingRpcMessages_.producer_push_back(pairRpcMessagePtr);
		}
	}
	catch(Exception ex)
	{
		IFATAL() << "Exception caught : " << ex.msg();
	}
}

// --------------------------------------------------------------
//	public RPC functions
// --------------------------------------------------------------
bool INetBufferedMessage::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	SerializeWriter::Ptr writer;
	bool ret = newSerializeWriter(writer);
	
	// -- debug --
	ASSERT(writer);
	ASSERT(ret);
	// -- debug --
	
	try
	{
		ASSERT(rpc);
		bool ret = BeginMarshal(rpc, writer);
		if(ret == false) 
		{
			IWARNING() << " No valid SocketStream is initialized!";
			return false;
		}

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
		IWARNING() << "Exception caught: " << ex.msg();
	}
	catch(...)
	{
		IWARNING()<< "Unhandled exception caught!";
	}

	return EndMarshal(writer);
}

// --------------------------------------------------------------
//	public receive RPC object
// --------------------------------------------------------------
bool INetBufferedMessage::ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &reader)
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
		IWARNING() << "Unhandled exception caught!";
	}

	return false;
}

// --------------------------------------------------------------
// public BeginMarshal function	called by Proxy/Stub
// -> Create new ByteArray and insert RpcIdentifier
// --------------------------------------------------------------
bool INetBufferedMessage::BeginMarshal(RpcIdentifier *obj, SerializeWriter::Ptr &writer) 
{
	ASSERT(obj);
	ASSERT(!writer);

	try
	{
		bool ret = newSerializeWriter(writer);
		ASSERT(ret);

		obj->Write(writer.get()); 
		
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
// public EndMarshal function called by Proxy/Stub
// -> Write ByteArray to NetworkLib buffered out messages
// --------------------------------------------------------------
bool INetBufferedMessage::EndMarshal(SerializeWriter::Ptr &writer) 
{
	try
	{
		ASSERT(writer);
	
		BufferedOutMessage::Ptr msg = BufferedOutMessage::Ptr(new BufferedOutMessage(handle_.GetNetAddress(), writer));
		
		bool isPushed = bufferedSocketWriter_->PushMessage(msg);

		aliveTimer_.Update();
		
		return isPushed;
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
// public EndMarshal function called by Proxy/Stub
// -> Write ByteArray to NetworkLib buffered out messages
// --------------------------------------------------------------
bool INetBufferedMessage::EndMarshal(SerializeWriter::Ptr &writer, const ListNetAddress &listAddresses) 
{
	try
	{
		ASSERT(writer);
	
		BufferedOutMessage::Ptr msg = BufferedOutMessage::Ptr(new BufferedOutMessage(listAddresses, writer));
		
		bool isPushed = bufferedSocketWriter_->PushMessage(msg);

		aliveTimer_.Update();
		
		return isPushed;
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
// protected BeginUnmarshal function called INetConnection 
// -> Buffered: retrieve message from output-queue populated by main thread
// --------------------------------------------------------------
bool INetBufferedMessage::BeginUnmarshal(RpcIdentifier *rpc, SerializeReader::Ptr &reader) 
{
	try
	{
		// -- debug --
		ASSERT(rpc);
		ASSERT(reader);
		// -- debug --

        PairRpcMessagePtr pairRpcMessage = queueIncomingRpcMessages_.consumer_pop_front();
		if(!pairRpcMessage) 
		{
			IWARNING() << "queueIncomingRpcMessages_ returned NULL! Connection lost?";
			return false;
		}
	
		*rpc = pairRpcMessage->first;
		reader = pairRpcMessage->second;
		
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
// protected EndUnmarshal function called Proxy/Stub
// TODO: statistics about reader? Time reading speed?
// --------------------------------------------------------------
void INetBufferedMessage::EndUnmarshal(SerializeReader::Ptr &)
{
	aliveTimer_.Update();
}

// --------------------------------------------------------------
//	public class functions
// --------------------------------------------------------------
int INetBufferedMessage::socketDescriptor() const
{
	MutexLocker lock(&mutex_);
	if(socketStream_ == NULL) return -1;
	else if(socketStream_->IsConnected() == false) return -1;

	return socketStream_->socketDescriptor();
}

bool INetBufferedMessage::IsConnected() const
{
	MutexLocker lock(&mutex_);
	if(socketStream_ == NULL) return false;

	return socketStream_->IsConnected();
}

bool INetBufferedMessage::IsBound() const
{
	MutexLocker lock(&mutex_);
	if(socketStream_ == NULL) return false;

	return socketStream_->IsBound();
}

bool INetBufferedMessage::IsBufferedRead() const
{
	return true;
}

bool INetBufferedMessage::IsBufferedWrite() const
{
	return true;
}

void INetBufferedMessage::Close()
{
	try
	{
		{
			MutexLocker lock(&mutex_);
			runThread_ = false;
		}
		Thread::wait();
	}
	catch(...)
	{
		ICRITICAL() << " Exception occured when closing connection!"; 
	}
}

bool INetBufferedMessage::DoKeepAlive()
{
	MutexLocker lock(&mutex_);
	// Only send keep alive if it has been more than xx seconds since last activity
	return aliveTimer_.HasExpired();
}

}; // namespace MicroMiddleware


