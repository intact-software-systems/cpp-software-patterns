#ifndef MicroMiddleware_RouteManagerBase_h_IsIncluded
#define MicroMiddleware_RouteManagerBase_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/RmiInterface.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/INet/INetStub.h"
#include"MicroMiddleware/INet/INetProxy.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
/* -------------------------------------------------------
 class RouteManagerInterface
---------------------------------------------------------*/
class DLL_STATE RouteManagerInterface : public RmiInterface
{
public:
	RouteManagerInterface();
	virtual ~RouteManagerInterface();

	enum MethodId
	{
		RmiConnectToPeer = 10
	};

	enum InterfaceId
	{
		RouteManagerInterfaceId = 94
	};

	virtual InterfaceHandle ConnectToPeer(InterfaceHandle &hostHandle,InterfaceHandle &remoteHandle) = 0;
	
	virtual int GetInterfaceId() const;
	virtual std::string GetInterfaceName() const;
};

class DLL_STATE RouteManagerProxy : public INetProxy, public RouteManagerInterface
{
public:
	RouteManagerProxy(const std::string &serviceName, const std::string &connectedToName);
	virtual ~RouteManagerProxy();

	virtual InterfaceHandle ConnectToPeer(InterfaceHandle &hostHandle,InterfaceHandle &remoteHandle);
	
	virtual void SetConnection(HostInformation &info);
};

class DLL_STATE RouteManagerStub : public INetStub
{
public:
	RouteManagerStub(const std::string &serviceName, RouteManagerInterface* serverInterface);
	virtual ~RouteManagerStub();

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &reader);
	
	virtual void SetConnection(HostInformation &info);
	
private:
	RouteManagerInterface *routeManager_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_RouteManagerLib_h_IsIncluded

