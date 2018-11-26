#ifndef IntactMiddleware_RouteManagerLib_h_IsIncluded
#define IntactMiddleware_RouteManagerLib_h_IsIncluded

#include "IntactMiddleware/IncludeExtLibs.h"

namespace IntactMiddleware
{
/* -------------------------------------------------------
 class RouteManagerInterface
---------------------------------------------------------*/
class RouteManagerInterface : public MicroMiddleware::RmiInterface
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

	virtual MicroMiddleware::InterfaceHandle ConnectToPeer(MicroMiddleware::InterfaceHandle &hostHandle,
														MicroMiddleware::InterfaceHandle &remoteHandle) = 0;
	
	virtual int GetInterfaceId() const;
	virtual std::string GetInterfaceName() const;
};

class RouteManagerProxy : public MicroMiddleware::INetProxy, public RouteManagerInterface
{
public:
	RouteManagerProxy(const std::string &serviceName);
	virtual ~RouteManagerProxy();

	virtual MicroMiddleware::InterfaceHandle ConnectToPeer(MicroMiddleware::InterfaceHandle &hostHandle,
													MicroMiddleware::InterfaceHandle &remoteHandle);
	
	virtual void SetConnection(HostInformation &info);
};

class RouteManagerStub : public INetStub
{
public:
	RouteManagerStub(const std::string &serviceName, RouteManagerInterface* serverInterface, bool autoStart);
	virtual ~RouteManagerStub();

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer);
	
	virtual void SetConnection(HostInformation &info) override ;
	
private:
	RouteManagerInterface *routeManager_;
};

}

#endif // IntactMiddleware_RouteManagerLib_h_IsIncluded

