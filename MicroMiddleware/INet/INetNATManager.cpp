#include "MicroMiddleware/INet/INetNATManager.h"
#include "MicroMiddleware/MiddlewareSettings.h"
#include "MicroMiddleware/Interfaces/RouteManagerBase.h"
#include "MicroMiddleware/CommonDefines.h"

namespace MicroMiddleware
{
INetNATManager* INetNATManager::natManager_ = NULL;

/*---------------------------------------------------
	class INetNATManager
----------------------------------------------------*/
INetNATManager* INetNATManager::GetOrCreate()
{
	static Mutex mutex;
	MutexLocker lock(&mutex);

	if(natManager_ == NULL)
		natManager_ = new INetNATManager();

	return natManager_;
}

bool INetNATManager::TcpNatPunchToPeer(InterfaceHandle &handle)
{
	INetNATManager *manager = INetNATManager::GetOrCreate();
	return manager->RequestTcpConnection(handle);
}

bool INetNATManager::UdpNatPunchToPeer(InterfaceHandle &handle)
{
	INetNATManager *manager = INetNATManager::GetOrCreate();
	return manager->RequestUdpConnection(handle);
}

/*---------------------------------------------------
	class INetNATManager
----------------------------------------------------*/
INetNATManager::INetNATManager(std::string name) 
	: ComponentBase(name)
	, BaseLib::Thread(name)
	, routeManagerInterface_(NULL)
	, runThread_(true)
{
	start();
}

INetNATManager::~INetNATManager()
{
	runThread_ = false;
	if(routeManagerInterface_) delete routeManagerInterface_;

	routeManagerInterface_ = NULL;
}

/*---------------------------------------------------
	thread function
	1. Check if I am behind NAT -> Should probably be discovered upon start of the application

	TODO:
		- In INetConnection add support for Middleware messages UdpNatRequest, TcpNatRequest
		- Do I really need to use routeManagerProxy_ or can the middleware handle everything?
----------------------------------------------------*/
void INetNATManager::run()
{
	try
	{
		Initialize();

		while(runThread_)
		{
			// queue of requests for either UdpNat or TcpNat
			
			// prepare message
			// InterfaceHandle updatedHandle = routeManagerProxy_->ConnectToPeer(localInterfaceHandle_, remoteHandle);
			
			// notify waiting thread
		}
	}
	catch(...)
	{
	
	}
}

/*---------------------------------------------------
	setup connection to RouteManager
	TODO: 
		- request local InterfaceHandle with public and private address/port
			-> use libnice convenience functions
		- Support: Automatically updating public/private address/port if connection changes between WLAN/LAN, etc.
		- Use enet for reliable UDP delivery!
----------------------------------------------------*/
void INetNATManager::Initialize()
{
	MutexLocker lock(&mutex_);
	while(routeManagerInterface_ == NULL)
	{
		waitCondition_.wait(&mutex_, 5000);
		
		// -- debug --
		if(routeManagerInterface_ == NULL) 
			IDEBUG() << " Still waiting for route manager interface!";
		// -- debug --
	}

	/*std::list<string> listAddresses = NetworkFunctions::getLocalIPs(true);
	cout << COMPONENT_FUNCTION << listAddresses << endl;

	listAddresses = NetworkFunctions::getLocalIPs(false);
	cout << COMPONENT_FUNCTION << listAddresses << endl;

	listAddresses = NetworkFunctions::getLocalInterfaceAddresses();
	cout << COMPONENT_FUNCTION << listAddresses << endl;
	*/
}

void INetNATManager::SetRouteManager(RouteManagerInterface *routeManagerInterface)
{
	ASSERT(routeManagerInterface);

	MutexLocker lock(&mutex_);
	routeManagerInterface_ = routeManagerInterface;
	waitCondition_.wakeAll();
}

bool INetNATManager::RequestUdpConnection(InterfaceHandle &handle)
{
	mutex_.lock();

	// add request to thread
	waitCondition_.wakeAll();

	// wait for completion
	waitCondition_.wait(&mutex_);
	
	mutex_.unlock();

	return false;
}

bool INetNATManager::RequestTcpConnection(InterfaceHandle &handle)
{

	return false;
}

} // namespace

