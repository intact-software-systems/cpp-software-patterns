#include "IntactMiddleware/INetNATManager.h"
#include "IntactMiddleware/ServiceConnectionManager.h"
#include "IntactMiddleware/IntactSettings/IntactSettings.h"
#include "IntactMiddleware/RouteManagerLib/RouteManagerBase.h"

namespace IntactMiddleware
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
	: IntactComponent(name)
	, BaseLib::Thread(name)
	, runThread_(true)
{
	start();
}

INetNATManager::~INetNATManager()
{

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
#if 0
	std::list<string> listAddresses = NetworkFunctions::getLocalIPs(true);
	cout << COMPONENT_FUNCTION << listAddresses << endl;

	listAddresses = NetworkFunctions::getLocalIPs(false);
	cout << COMPONENT_FUNCTION << listAddresses << endl;

	listAddresses = NetworkFunctions::getLocalInterfaceAddresses();
	cout << COMPONENT_FUNCTION << listAddresses << endl;

	cout << COMPONENT_FUNCTION << NetworkFunctions::getIpForInterface("knuthelv-laptop") << endl;

	ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();

	bool waitForConnection = true;
	bool connected = service->SubscribeService(IntactSettings::GetRouteManagerService(), routeManagerInformation_ ,waitForConnection);
	
	// -- start debug --
	cout << COMPONENT_FUNCTION << "Received routeManager info: " << routeManagerInformation_ << endl;
	ASSERT(connected);
	ASSERT(routeManagerInformation_.IsValid());
	ASSERT(routeManagerInformation_.GetComponentName() == IntactSettings::GetRouteManagerService());
	// -- end debug --
	
	routeManagerProxy_ = new RouteManagerProxy(routeManagerInformation_.GetComponentName());

	routeManagerHandle_ = InterfaceHandle(routeManagerInformation_.GetHostName(), routeManagerInformation_.GetPortNumber());
	bool registered = MicroMiddleware::INetConnectionManager::GetOrCreate()->RegisterInterface(routeManagerHandle_, routeManagerProxy_);
	
	// -- start debug --
	ASSERT(routeManagerHandle_.IsValid());
	ASSERT(registered);
	// -- end debug --
#endif
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

}

