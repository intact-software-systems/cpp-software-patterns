#include"MicroMiddleware/INet/INetProxy.h"
#include"MicroMiddleware/INet/INetBase.h"
#include"MicroMiddleware/Runtime.h"
#include"MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{
// --------------------------------------------------
// Constructor/Destructor
// TODO: Add a thread in Proxy class to handle incoming RPCs, similar to Stub?
// TODO: Add feedback mechanism to user-implementation if sending fails
// --------------------------------------------------
INetProxy::INetProxy(const std::string &serviceName, const std::string &connectedToName, const std::string &interfaceName, HostInformation::HostDescription desc, EndianConverter::Endian hostEndian) 
	: INetInterface(interfaceName, desc)
	, runThread_(true)
	, waitRpc_(true)
	, hostEndian_(hostEndian)
	, serviceName_(serviceName)
	, connectedToName_(connectedToName)
{ 
	start();
}

INetProxy::INetProxy(const std::string &serviceName, const std::string &connectedToName, const std::string &interfaceName) 
	: INetInterface(interfaceName, HostInformation::PROXY)
	, runThread_(true)
	, waitRpc_(true)
	, hostEndian_(EndianConverter::ENDIAN_LITTLE)
	, serviceName_(serviceName)
	, connectedToName_(connectedToName)
{ 
	start();
}

INetProxy::~INetProxy() 
{ 
	Stop();
}

// --------------------------------------------------
//	TODO: Read return value of RPC
//		-> If middleware acknowledgements of successfully sent packets then remove from HistoryCache
//		-> If failure sending packets then notify application
//		-> if packet is intended for application then add to filteredQueueRpcCalls
// --------------------------------------------------
void INetProxy::run()
{
	try
	{
		IDEBUG() << "started";

		while(runThread_)
		{
            RpcCallWrapper rpcCall = queueRpcCall_.consumer_pop_front();
			if(runThread_ == false) break;

			RpcIdentifier rpc 				= rpcCall.GetRpcId();
			//currentConnection_				= rpcCall.GetConnection();
			SerializeReader::Ptr readBuffer = rpcCall.GetReadBuffer();
			
			// -- start debug --
			ASSERT(rpc.IsValid());
			ASSERT(rpc.GetConnectionId() == RpcIdentifier::StubConnection || rpc.GetConnectionId() == RpcIdentifier::SubscriberConnection);
			//ASSERT(currentConnection_);
			// -- end debug --
			
			// --------------------------------------------------------
			//-> If middleware acknowledgements of successfully sent packets then remove from HistoryCache
			//-> If failure sending packets then notify application
			//-> if packet is intended for application then add to filteredQueueRpcCalls
			// --------------------------------------------------------
			
			
			queueFilteredRpcCalls_.producer_push_back(rpcCall);
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught! " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!" ;
	}
	
	// TODO: deregister?
	iCritical() << COMPONENT_FUNCTION << "Stub stopped!" ;
}

// --------------------------------------------------
// TODO: revise implementation
// --------------------------------------------------
void INetProxy::Stop()
{
	{
		MutexLocker lock(&mutex_);
	
		waitRpc_ = false;
		runThread_ = false;
	
		queueRpcCall_.producer_push_back(RpcCallWrapper());
		queueFilteredRpcCalls_.producer_push_back(RpcCallWrapper());
	}

	Thread::wait();
}

// --------------------------------------------------
// TODO: Evaluate the need for SetBase.
// --------------------------------------------------
void INetProxy::SetBase(INetMessageParser::Ptr &base)
{
	MutexLocker lock(&mutex_);
	inetConn_ = base;
}

// --------------------------------------------------
// Implement abstract functions
// --------------------------------------------------
bool INetProxy::IsConnected() const
{
	MutexLocker lock(&mutex_);
	if(!inetConn_)
		return false;

	return inetConn_->IsConnected();
}

bool INetProxy::IsRunning() const
{
	MutexLocker lock(&mutex_);
	if(!inetConn_)
		return false;

	return inetConn_->IsConnected();
}

std::string INetProxy::GetServiceName() const
{
	MutexLocker lock(&mutex_);
	return serviceName_;
}

std::string INetProxy::GetConnectedToName() const
{
	MutexLocker lock(&mutex_);
	return connectedToName_;
}

bool INetProxy::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	ASSERT(rpc);
	ASSERT(obj);

	MutexLocker lock(&mutex_);
	if(!inetConn_) 
	{
		stringstream str;
		str << PRETTY_FUNCTION << "INetMessageParser == NULL!";
		throw Exception(str.str());
	}

	rpc->SetConnectionId(RpcIdentifier::StubConnection);
	ASSERT(rpc->GetConnectionId() == RpcIdentifier::StubConnection);

	return inetConn_->ExecuteRpc(rpc, obj);
}	

void INetProxy::ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &reader)
{
	ASSERT(obj);
	ASSERT(reader);

	MutexLocker lock(&mutex_);
	if(!inetConn_) 
	{
		stringstream str;
		str << PRETTY_FUNCTION << "INetMessageParser== NULL!";
		throw Exception(str.str());
	}

	inetConn_->ReceiveRpcObject(obj, reader);
}

// --------------------------------------------------
// - Proxy initiates RPC call!
// - WriteLock()/WriteUnlock() is automatically called in BeginMarshal()/EndMarshal()
// --------------------------------------------------
SerializeWriter::Ptr INetProxy::BeginMarshal(const short &methodId) 
{
	ASSERT(IsValid());
	ASSERT(methodId >= 0);

	MutexLocker lock(&mutex_);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			if(inetConn_ == NULL)
			{
				throw Exception("ERROR! Please register INetProxy in INetConnection before initiating RPC calls!");
			}

			RpcIdentifier rpc(methodId, GetInterfaceName(), (short)hostEndian_, RpcIdentifier::StubConnection);
			
			// -- debug --
			ASSERT(rpc.GetConnectionId() == RpcIdentifier::StubConnection);
			ASSERT(rpc.GetInterfaceName() == GetInterfaceName());
			// -- debug --

			SerializeWriter::Ptr writer;
			bool ret = inetConn_->BeginMarshal(&rpc, writer);
			if(ret == false)
			{
				throw Exception("ERROR! INetProxy::BeginMarshal failed!");
			}
			return writer;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}
	return SerializeWriter::Ptr();
}

bool INetProxy::EndMarshal(SerializeWriter::Ptr &writer)
{
	MutexLocker lock(&mutex_);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			if(inetConn_ == NULL)
			{
				throw Exception("ERROR! Please register INetProxy in INetConnection before initiating RPC calls!");
			}

			return inetConn_->EndMarshal(writer);
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return false;
}

bool INetProxy::EndMarshal(SerializeWriter::Ptr &writer, const ListNetAddress &listAddresses)
{
	MutexLocker lock(&mutex_);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			if(inetConn_ == NULL)
			{
				throw Exception("ERROR! Please register INetProxy in INetConnection before initiating RPC calls!");
			}

			return inetConn_->EndMarshal(writer, listAddresses);
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return false;
}

// --------------------------------------------------
// - Proxy waits for RPC return value!
// - INetConnection wakes up Proxy and lets it read return value
// - Proxy then wakes up INetConnection after finished reading.
// - ReadUnlock is automatically called in EndUnmarshal()
// - TODO: Add reasonable timeout in queueRpcCall_.consumer_pop()
// --------------------------------------------------
SerializeReader::Ptr INetProxy::BeginUnmarshal()
{
	try
	{
        RpcCallWrapper rpcCall = queueFilteredRpcCalls_.consumer_pop_front();
		
		MutexLocker lock(&mutex_);
		if(waitRpc_ == false) 
		{
			throw CriticalException("InetProxy::BeginUnmarshal(): waitRpc_ == false!");
		}

		RpcIdentifier 			rpc 	= rpcCall.GetRpcId();
		INetMessageParser::Ptr 	base	= rpcCall.GetConnection();
		SerializeReader::Ptr 	reader 	= rpcCall.GetReadBuffer();

		// -- start debug --
		ASSERT(rpc.IsValid());
		ASSERT(rpc.GetConnectionId() == RpcIdentifier::ProxyConnection);
		ASSERT(rpc.GetInterfaceName() == GetInterfaceName());
		//ASSERT(rpc.GetInterfaceId() == this->interfaceId_);
		ASSERT(reader);
		ASSERT(base == inetConn_);
		// -- end debug --

		// already called: base->BeginUnmarshal();	// base->ReadLock() is called
		return reader;
	}
	catch(Exception ex)
	{
		ICRITICAL() << "Exception caught: " << ex.msg();
		// TODO: deregister?
		throw;
	}

	return SerializeReader::Ptr();
}

void INetProxy::EndUnmarshal(SerializeReader::Ptr &reader)
{
	MutexLocker lock(&mutex_);
	
	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			// notifies waiting thread in ReadCall
			ASSERT(inetConn_ != NULL);
			inetConn_->EndUnmarshal(reader);	// base->ReadUnlock() and NotifyWaitRead() is called
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}
}

void INetProxy::ReadCall(const RpcIdentifier &rpc, INetMessageParser::Ptr &base, SerializeReader::Ptr &buffer)
{
	// -- debug --
	ASSERT(base);
	ASSERT(buffer);
	ASSERT(rpc.IsValid());
	// -- debug --

	queueRpcCall_.producer_push_back(RpcCallWrapper(rpc, base, buffer));
}

}; // namespace MicroMiddleware


