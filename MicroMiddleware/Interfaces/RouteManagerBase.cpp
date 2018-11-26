#include "MicroMiddleware/Interfaces/RouteManagerBase.h"
#include "MicroMiddleware/CommonDefines.h"
//#include "MicroMiddleware/MiddlewareSettings.h"

namespace MicroMiddleware
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
int RouteManagerInterface::GetInterfaceId()	const
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

RouteManagerProxy::RouteManagerProxy(const std::string &serviceName, const std::string &connectedToName)
		: INetProxy(serviceName, connectedToName, RouteManagerInterface::GetInterfaceName())
{

}
RouteManagerProxy::~RouteManagerProxy()
{

}

InterfaceHandle RouteManagerProxy::ConnectToPeer(InterfaceHandle &hostHandle, InterfaceHandle &remoteHandle)
{
	NetworkLib::SerializeWriter::Ptr writer = BeginMarshal(RouteManagerInterface::RmiConnectToPeer);
	hostHandle.Write(writer.get());
	remoteHandle.Write(writer.get());
	EndMarshal(writer);

	SerializeReader::Ptr reader = BeginUnmarshal();
	InterfaceHandle returnHandle;
	returnHandle.Read(reader.get());
	EndUnmarshal(reader);

	return returnHandle;
}

void RouteManagerProxy::SetConnection(HostInformation &info) 
{
	iDebug() << COMPONENT_FUNCTION << "WARNING! Not implemented : " << GetServiceName();
	cout << COMPONENT_FUNCTION << "HostInformation from INetConnection: " << info << endl;
	
	//MutexLocker lock(&mutex_);
	/*cout << COMPONENT_FUNCTION << GetServiceName() << endl;

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
	}*/
}

/* -------------------------------------------------------
	Stub
---------------------------------------------------------*/
RouteManagerStub::RouteManagerStub(const std::string &serviceName, RouteManagerInterface* serverInterface)
		: INetStub(serviceName, serverInterface->GetInterfaceName())
		, routeManager_(serverInterface)
{

}
RouteManagerStub::~RouteManagerStub() 
{

}

bool RouteManagerStub::RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &reader)
{
	switch(rpc.GetMethodId())
	{
		case RouteManagerInterface::RmiConnectToPeer:
		{
			InterfaceHandle hostHandle, remoteHandle;
			ReceiveRpcObject(&hostHandle, reader);
			ReceiveRpcObject(&remoteHandle, reader);
			EndUnmarshal(reader);
			
			InterfaceHandle returnHandle = routeManager_->ConnectToPeer(hostHandle, remoteHandle);
			NetworkLib::SerializeWriter::Ptr writer = BeginMarshal(RouteManagerInterface::RmiConnectToPeer);
			returnHandle.Write(writer.get());
			return EndMarshal(writer);
		}
		default:
			cerr << COMPONENT_FUNCTION << " RMI not implemented " << rpc.GetMethodId()<< endl;
			return true;
	}

	return true;
}

void RouteManagerStub::SetConnection(HostInformation &info) 
{
	iDebug() << COMPONENT_FUNCTION << "WARNING! Not implemented : " << GetServiceName();
	cout << COMPONENT_FUNCTION << "HostInformation from INetConnection: " << info << endl;

	/*//MutexLocker lock(&mutex_);
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
	}*/
}

}

