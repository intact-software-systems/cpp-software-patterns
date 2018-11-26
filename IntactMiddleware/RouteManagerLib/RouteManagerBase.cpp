#include "IntactMiddleware/RouteManagerLib/RouteManagerBase.h"
#include "IntactMiddleware/ServiceConnectionManager.h"

namespace IntactMiddleware
{
/* -------------------------------------------------------
 class RouteManagerInterface
---------------------------------------------------------*/
RouteManagerInterface::RouteManagerInterface() 
{
}

RouteManagerInterface::~RouteManagerInterface() 
{

}
/* -------------------------------------------------------
	functions
---------------------------------------------------------*/
int RouteManagerInterface::GetInterfaceId() const
{ 
	return RouteManagerInterfaceId; 
}

std::string RouteManagerInterface::GetInterfaceName() const
{ 
	return RPC_FUNCTION_STRING; 
}

/* -------------------------------------------------------
	Proxy
---------------------------------------------------------*/

RouteManagerProxy::RouteManagerProxy(const std::string &serviceName)
		: INetProxy(serviceName, RouteManagerInterface::GetInterfaceName())
{

}
RouteManagerProxy::~RouteManagerProxy()
{

}

MicroMiddleware::InterfaceHandle RouteManagerProxy::ConnectToPeer(MicroMiddleware::InterfaceHandle &hostHandle,
													MicroMiddleware::InterfaceHandle &remoteHandle)
{
	NetworkLib::SerializeWriter::Ptr writer = BeginMarshal(RouteManagerInterface::RmiConnectToPeer);
	hostHandle.Write(writer.get());
	remoteHandle.Write(writer.get());
	EndMarshal(writer);

	SerializeReader::Ptr reader = BeginUnmarshal();
	MicroMiddleware::InterfaceHandle returnHandle;
	returnHandle.Read(reader.get());
	EndUnmarshal(reader);
}

void RouteManagerProxy::SetConnection(HostInformation &info) 
{
	//MutexLocker lock(&mutex_);
	cout << COMPONENT_FUNCTION << GetServiceName() << endl;

	ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
	if(info.GetOnlineStatus() == HostInformation::ONLINE)
	{
		info.SetComponentName(GetServiceName());
		service->RegisterService(info);
	}
	else
	{
		ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
		service->RemoveService(GetServiceName());
	}
}
/* -------------------------------------------------------
	Stub
---------------------------------------------------------*/
RouteManagerStub::RouteManagerStub(const std::string &serviceName, RouteManagerInterface* serverInterface, bool autoStart)
		: INetStub(serviceName, serverInterface->GetInterfaceName(), EndianConverter::ENDIAN_LITTLE, autoStart)
		, routeManager_(serverInterface)
{

}
RouteManagerStub::~RouteManagerStub() 
{

}

bool RouteManagerStub::RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer)
{
	switch(rpc.GetMethodId())
	{
		case RouteManagerInterface::RmiConnectToPeer:
		{
			MicroMiddleware::InterfaceHandle hostHandle, remoteHandle;
			ReceiveRpcObject(&hostHandle);
			ReceiveRpcObject(&remoteHandle);
			EndUnmarshal();
			
			MicroMiddleware::InterfaceHandle returnHandle = routeManager_->ConnectToPeer(hostHandle, remoteHandle);
			NetworkLib::SerializeWriter *writer = BeginMarshal(RouteManagerInterface::RmiConnectToPeer);
			returnHandle.Write(writer);
			return EndMarshal();
		}
		default:
			cerr << COMPONENT_FUNCTION << " RMI not implemented " << rpc.GetMethodId()<< endl;
			return true;
	}

	return true;
}

void RouteManagerStub::SetConnection(HostInformation &info) 
{
	//MutexLocker lock(&mutex_);
	cout << COMPONENT_FUNCTION << GetServiceName() << endl;

	ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
	if(info.GetOnlineStatus() == HostInformation::ONLINE)
	{
		info.SetComponentName(GetServiceName());
		service->RegisterService(info);
	}
	else
	{
		ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
		service->RemoveService(GetServiceName());
	}
}

}

