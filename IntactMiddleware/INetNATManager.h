#ifndef IntactMiddleware_INetNATManager_h_IsIncluded
#define IntactMiddleware_INetNATManager_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/IntactComponent.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{

class RouteManagerProxy;

/*---------------------------------------------------
	- Check if I am behind NAT 
		- Should probably be discovered upon start of the application
	- Maintain connection to RouteManager
	- Receive incoming peer connection requests 
	- contact RouteManager through RMI and perform ICE (STUN/TURN)
	- Create new thread to handle request?

	TODO:
		- Create a MicroMiddleware::MiddlewareSettings and a IntactMiddleware::IntactSettings
		- Move RouteManagerBase.h/cpp from IntactMiddleware to MicroMiddleware
		- Create a folder MicroMiddlware/Interfaces perhaps?
		- IntactSettings inherit from MiddlewareSettings, makes some settings available from MicroMiddleware
		- Make RouteManagerServiceName available from MicroMiddleware
		- IntactMiddleware::Initialize() -> should initialize INetNATManager with serviceInformation regarding RouteManager
		- Continuous call-backs from ServiceConnectionManager to keep INetNATManager up-to-date with current information!
		- This way: INetConnectionManager/INetConnection can use INetNATManager when connecting to a remote peer!
		- Makes the implementation seam-less from anyone that wants a connection to a remote peer, for example, Components and IntactMiddleware services!
----------------------------------------------------*/
class INetNATManager : public IntactComponent
					 , public BaseLib::Thread
{
private:
	enum NatPunchType
	{
		UdpNatPunch = 1,
		TcpNatPunch
	};

private:
	INetNATManager(std::string name = std::string("IntactMiddleware.INetNATManager"));
	~INetNATManager();

	friend class MiddlewareSettings;

public:
	static bool UdpNatPunchToPeer(MicroMiddleware::InterfaceHandle &handle);
	static bool TcpNatPunchToPeer(MicroMiddleware::InterfaceHandle &handle);

protected:
	static INetNATManager* GetOrCreate();

protected:
	virtual void run();

	virtual void Initialize();

	bool RequestUdpConnection(MicroMiddleware::InterfaceHandle &handle);
	bool RequestTcpConnection(MicroMiddleware::InterfaceHandle &handle);

private:
	MicroMiddleware::HostInformation 	routeManagerInformation_;
	MicroMiddleware::InterfaceHandle	routeManagerHandle_;
	MicroMiddleware::InterfaceHandle	localInterfaceHandle_;

	RouteManagerProxy 					*routeManagerProxy_;
	
	bool								runThread_;
	BaseLib::WaitCondition				waitCondition_;
	BaseLib::Mutex						mutex_;

private:
	static INetNATManager				*natManager_;
};

}

#endif // IntactMiddleware_INetNATManager_h_IsIncluded

