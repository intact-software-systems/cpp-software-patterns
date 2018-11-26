#ifndef MicroMiddleware_INetNATManager_h_IsIncluded
#define MicroMiddleware_INetNATManager_h_IsIncluded

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/ComponentBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RouteManagerInterface;

/*---------------------------------------------------
	- Check if I am behind NAT 
		- Should probably be discovered upon start of the application
	- Maintain connection to RouteManager
	- Receive incoming peer connection requests 
	- contact RouteManager through RMI and perform ICE (STUN/TURN)
	- Create new thread to handle request?

	TODO:
		- OK: Create a MicroMiddleware::MiddlewareSettings and a IntactMiddleware::IntactSettings
		- OK: Move RouteManagerBase.h/cpp from IntactMiddleware to MicroMiddleware
			- NO: Create a folder MicroMiddlware/Interfaces perhaps?
			- NO: IntactSettings inherit from MiddlewareSettings, makes some settings available from MicroMiddleware
		- OK: Make RouteManagerServiceName available from MicroMiddleware
		- OK: IntactSettings::Initialize() -> should initialize INetNATManager with serviceInformation regarding RouteManager
		- Not Tested: Continuous call-backs from ServiceConnectionManager to keep INetNATManager up-to-date with current HostInformation of RouteManager!
		- This way: INetConnectionManager/INetConnection can use INetNATManager when connecting to a remote peer!
		- Makes the implementation seam-less from anyone that wants a connection to a remote peer, for example, Components and IntactMiddleware services!
----------------------------------------------------*/
class DLL_STATE INetNATManager : public ComponentBase
							   , public BaseLib::Thread
{
private:
	enum NatPunchType
	{
		UdpNatPunch = 1,
		TcpNatPunch
	};

private:
	INetNATManager(std::string name = std::string("MicroMiddleware.INetNATManager"));
	virtual ~INetNATManager();

public:
	static INetNATManager* GetOrCreate();

public:
	static bool UdpNatPunchToPeer(InterfaceHandle &handle);
	static bool TcpNatPunchToPeer(InterfaceHandle &handle);

	void SetRouteManager(RouteManagerInterface *routeManager);

protected:
	virtual void run();

	virtual void Initialize();

	bool RequestUdpConnection(InterfaceHandle &handle);
	bool RequestTcpConnection(InterfaceHandle &handle);

private:
	InterfaceHandle			localInterfaceHandle_;
	RouteManagerInterface 	*routeManagerInterface_;
	
	bool					runThread_;
	BaseLib::WaitCondition	waitCondition_;
	BaseLib::Mutex			mutex_;

private:
	static INetNATManager	*natManager_;
};

}

#endif // MicroMiddleware_INetNATManager_h_IsIncluded

