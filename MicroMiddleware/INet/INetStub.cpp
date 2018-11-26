#include"MicroMiddleware/INet/INetStub.h"
#include"MicroMiddleware/INet/INetBase.h"
#include"MicroMiddleware/CommonDefines.h"
#include"MicroMiddleware/Runtime.h"

namespace MicroMiddleware
{
// --------------------------------------------------
//	initialize
//  serviceName == the name of one instance of the interface
//  interfaceName == the name of the implementation of the Stub class
// --------------------------------------------------
INetStub::INetStub(const std::string &serviceName, const std::string &interfaceName, HostInformation::HostDescription hostDescription, EndianConverter::Endian hostEndian, bool autoStart) 
	: INetInterface(interfaceName, hostDescription)
	, Thread(serviceName)
	, runThread_(true)
	, hostEndian_(hostEndian)
	, serviceName_(serviceName)
{ 
	if(autoStart) start();
}

INetStub::INetStub(const std::string &serviceName, const std::string &interfaceName) 
	: INetInterface(interfaceName, HostInformation::STUB)
	, Thread(serviceName)
	, runThread_(true)
	, hostEndian_(EndianConverter::ENDIAN_LITTLE)
	, serviceName_(serviceName)
{ 
	start();
}

INetStub::~INetStub() 
{ 
	Stop();
}

// --------------------------------------------------
//	state
// --------------------------------------------------
void INetStub::Stop()
{
	{
		MutexLocker lock(&mutex_);
		runThread_ = false;
		queueRpcCall_.producer_push_back(RpcCallWrapper());
	}

	Thread::wait();
}

bool INetStub::IsRunning() const
{
	return Thread::isRunning();
}

std::string INetStub::GetServiceName() const
{
	MutexLocker lock(&mutex_);
	return serviceName_;
}

std::string INetStub::GetConnectedToName() const
{
	return std::string();
}

// --------------------------------------------------
//  - Wait for rpc calls 
//  - RpcCalls are added to rpcQueue by INetConnection
//  - When woken up call RPCall(rpc), which is reimplemented by ComponentStub
// 	- If unbuffered: ReadUnlock is automatically called in EndUnmarshal()
// --------------------------------------------------
void INetStub::run()
{
	try
	{
		IDEBUG() << "started";

		while(runThread_)
		{
            RpcCallWrapper rpcCall = queueRpcCall_.consumer_pop_front();
			if(runThread_ == false) break;

			RpcIdentifier rpc 				= rpcCall.GetRpcId();
			currentConnection_				= rpcCall.GetConnection();
			SerializeReader::Ptr readBuffer = rpcCall.GetReadBuffer();
			
			// -- start debug --
			ASSERT(rpc.IsValid());
			ASSERT(rpc.GetConnectionId() == RpcIdentifier::StubConnection || rpc.GetConnectionId() == RpcIdentifier::SubscriberConnection);
			ASSERT(currentConnection_);
			// -- end debug --
		
			ReadRpc(rpc, readBuffer);
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
// Start unmarshalling rpc
// --------------------------------------------------
void INetStub::ReadRpc(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer) 
{
	try
	{
		//IDEBUG() << " Stub " << GetName() << " rpc: " << rpc << endl;
		bool ret = RPCall(rpc, readBuffer);
		if(ret == false)
			cerr << COMPONENT_FUNCTION << "WARNING! RPCall failed: " << rpc << endl;
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught! " << ex.msg();
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled exception caught!";
	}
}
// --------------------------------------------------
// SerializeReader buffer is used to read RPC
//  -> read synchronization is handled outside class in INetConnection
// --------------------------------------------------
void INetStub::ReadCall(const RpcIdentifier &rpc, INetMessageParser::Ptr &base, SerializeReader::Ptr &buffer)
{
	// -- debug --
	ASSERT(base);
	ASSERT(buffer);
	ASSERT(rpc.IsValid());
	// -- debug --
	
	queueRpcCall_.producer_push_back(RpcCallWrapper(rpc, base, buffer));
}

// --------------------------------------------------
//	marshalling/unmarshalling
//  -> Do I need to synchronize these functions in the Stub?
//		- Probably not, protected functions are only accessed by Stub's own thread
// --------------------------------------------------
SerializeWriter::Ptr INetStub::BeginMarshal(const short &methodId)
{
	ASSERT(IsValid());
	ASSERT(methodId >= 0);

	RpcIdentifier rpc(methodId, GetInterfaceName(), (short)hostEndian_, RpcIdentifier::ProxyConnection);
	ASSERT(rpc.IsValid());

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			SerializeWriter::Ptr writer;

			ASSERT(currentConnection_);
			bool ret = currentConnection_->BeginMarshal(&rpc, writer);
			if(ret == false)
			{
				throw Exception("INetStub::BeginMarshal(): Could not BeginMarshal()");
			}
			return writer;
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return SerializeWriter::Ptr();
}

bool INetStub::EndMarshal(SerializeWriter::Ptr &writer)
{
	ASSERT(writer);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			ASSERT(currentConnection_);
			return currentConnection_->EndMarshal(writer);
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return false;
}

bool INetStub::EndMarshal(SerializeWriter::Ptr &writer, const ListNetAddress &listAddresses)
{
	ASSERT(writer);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			ASSERT(currentConnection_);
			return currentConnection_->EndMarshal(writer, listAddresses);
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}

	return false;
}


void INetStub::EndUnmarshal(SerializeReader::Ptr &reader)
{
	ASSERT(reader);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			ASSERT(currentConnection_);
			currentConnection_->EndUnmarshal(reader);
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}
}
	
bool INetStub::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj)
{
	ASSERT(rpc);
	ASSERT(obj);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			ASSERT(currentConnection_);
			return currentConnection_->ExecuteRpc(rpc, obj);
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}
	return false;
}

void INetStub::ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &reader)
{
	ASSERT(obj);
	ASSERT(reader);

	switch(GetSerializeType())
	{
		case INetInterface::BufferedType:
		case INetInterface::UnbufferedType:
		{
			ASSERT(currentConnection_);
			bool ret = currentConnection_->ReceiveRpcObject(obj, reader);
			if(ret == false)
			{
				throw Exception("INetStub::ReceiveRpcObject: Failed to receive rpc object");
			}
			break;
		}
		default:
			IFATAL() << "ERROR! Could not recognize serialize type!";
			break;
	}
}

}; // namespace MicroMiddleware


