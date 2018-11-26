#include"IntactMiddleware/ServiceManagerBase.h"
#include"IntactMiddleware/ServiceConnectionManager.h"

namespace IntactMiddleware
{

ServiceConnectionManager* ServiceConnectionManager::serviceConnectionManager_ = NULL;

/*--------------------------------------------------------------------------
I cannot connect until service has registered and given InterfaceHandle
	-> ConnectToService is a separate thread that waits until service is connected.
----------------------------------------------------------------------------*/
class ConnectToService : public Thread
{
public:
	ConnectToService(ServiceConnectionManager *manager, INetInterface *inetInterface)
		: manager_(manager)
		, inetInterface_(inetInterface)
		, givenUp_(false)
	{
		start();
	}
	virtual ~ConnectToService()
	{}

	virtual void run()
	{
		HostInformation serviceInfo = manager_->WaitForService(inetInterface_->GetConnectedToName());
		if(!serviceInfo.IsValid() || serviceInfo.GetOnlineStatus() == HostInformation::OFFLINE)
		{
			ICRITICAL() << inetInterface_->GetServiceName() << " failed to get valid service-info for " << inetInterface_->GetConnectedToName();
			IDEBUG()  << " service : " << serviceInfo ;

			MutexLocker lock(&mutex_);
			givenUp_ = true;
			waitCondition_.wakeAll();
			return;
		}

		MutexLocker lock(&mutex_);
		serviceInfo_ = serviceInfo;

		INetConnectionManager *conn = INetConnectionManager::GetOrCreate();
		ASSERT(conn);

		InterfaceHandle handle(serviceInfo.GetHostName(), serviceInfo.GetPortNumber());
		ASSERT(handle.IsValid());

		bool ret = conn->RegisterInterface(handle, inetInterface_);

		// -- debug --
		ASSERT(ret);
		IDEBUG() << inetInterface_->GetServiceName() << " valid service-info for " << inetInterface_->GetConnectedToName();
		// -- debug --

		waitCondition_.wakeAll();
	}

	bool WaitForService()
	{
		mutex_.lock();
		while(!serviceInfo_.IsValid())
		{
			bool ret = waitCondition_.wait(&mutex_, 5000);
			if(givenUp_) return false;

			// -- debug --
			if(!serviceInfo_.IsValid())
				IDEBUG() << inetInterface_->GetServiceName() << " Still waiting for service " << inetInterface_->GetConnectedToName();
			// -- debug --
		}
		mutex_.unlock();
		return true;
	}

private:
	ServiceConnectionManager 	*manager_;
	INetInterface 				*inetInterface_;
	HostInformation				serviceInfo_;
	bool						givenUp_;

	mutable Mutex				mutex_;
	WaitCondition				waitCondition_;
};

/*--------------------------------------------------------------------------
TODO: Issues:
	- What if the network communication for a service (stub,publisher) goes down, how to detect it locally?
	- Create a SystemConnectionManager that controls the links between services and network links?
	- Add RegisterService in each STUB constructor
	- Add RemoveService in each STUB destructor
	- Similarly for Proxies, etc
----------------------------------------------------------------------------*/
ServiceConnectionManager::ServiceConnectionManager(InterfaceHandle lookupHandle, bool autoStart)
	: serviceManagerHandle_(lookupHandle)
	, serviceManagerProxy_(NULL)
	, runThread_(true)
{
	if(autoStart) start();
}

ServiceConnectionManager::~ServiceConnectionManager()
{
	if(serviceManagerProxy_) delete serviceManagerProxy_;

	serviceManagerProxy_ = NULL;
}

ServiceConnectionManager* ServiceConnectionManager::GetOrCreate(InterfaceHandle *handle)
{
	static Mutex staticLock;
	MutexLocker lock(&staticLock);

	if(serviceConnectionManager_ != NULL) return serviceConnectionManager_;

	if(handle == NULL)
	{
		InterfaceHandle serviceHandle = Runtime::GetLookupServerHandle();
		ASSERT(serviceHandle.GetPortNumber() > 0);
		ASSERT(!serviceHandle.GetHostName().empty());

		serviceConnectionManager_ = new ServiceConnectionManager(serviceHandle);
	}
	else
	{
		ASSERT(handle->GetPortNumber() > 0);
		ASSERT(!handle->GetHostName().empty());
		serviceConnectionManager_ = new ServiceConnectionManager(*handle);
	}
	return serviceConnectionManager_;
}

bool ServiceConnectionManager::WaitConnected()
{
	mutex_.lock();
	while(serviceManagerProxy_ == NULL)
	{
		IDEBUG() << "Waiting for serviceManagerProxy to connect to ServiceManager on " << serviceManagerHandle_ ;
		waitCondition_.wait(&mutex_, 10000);
	}
	mutex_.unlock();

	ASSERT(serviceManagerProxy_);
	return serviceManagerProxy_->WaitConnected();
}

HostInformation ServiceConnectionManager::WaitForService(const std::string &serviceName)
{
	ASSERT(!serviceName.empty());

	bool ret = WaitConnected();
	if(!ret)
	{
		ICRITICAL() << " Not connected to ServiceManager!";
		return HostInformation();
	}

	try
	{
		while(true)
		{
			MutexLocker lock(&mutex_);

			if(mapSubscribedServices_.count(serviceName) > 0)
			{
				HostInformation info = mapSubscribedServices_[serviceName];
				ASSERT(info.IsValid());

				if(info.GetOnlineStatus() == HostInformation::ONLINE)
					return info;
			}
			else
			{
				IDEBUG() << "Waiting for " << serviceName;
			}

			bool ret = waitCondition_.wait(&mutex_, 5000);
			if(!runThread_) break;
		}
	}
	catch(...)
	{
		ICRITICAL() << "Unhandled Exception! WaitForService() failed!";
	}

	return HostInformation();
}

/*---------------------------------------------------------------------------
	main thread function

TODO:
	- Support for peerAddress/peerPort, private/public addresses for Proxies and Subscribers
		-> Request addresses from RouteManager/AddressManager
	- Implement a ServiceInformation that includes
		-> Publisher: list of subscriber connections
		-> Subscriber: publisher connection
---------------------------------------------------------------------------*/
void ServiceConnectionManager::run()
{
	IDEBUG() << "Connecting ServiceManager on host " << serviceManagerHandle_.GetHostName();

	mutex_.lock();
	initConnectionManager(serviceManagerHandle_);

	while(runThread_)
	{
		try
		{
			bool ret = waitCondition_.wait(&mutex_, 1000);
			if(!runThread_) break;

			ASSERT(serviceManagerProxy_);

			// 1. Register or remove services through ServiceManager::ServiceRegistration(MapServices);
			bool registeredServices = this->serviceRegistration();

			// 2. Send Keep Alive messages to inform ServiceManager the services are ONLINE, and,
			//    Subscribe to service information in the ServiceManager
			MapServiceNameHostInformation mapCurrentSubscribedServices = serviceManagerProxy_->AliveChecker(mapRegisteredServices_, setSubscribedServices_);
			waitCondition_.wakeAll();

			// 3. Check if service-information has changed (timestamps), if yes, then use callbacks to each registered class ComponentBase*
			// The callbacks should include Set<HostInformation> SetServiceInformation to enable each component to update InterfaceHandles etc.
			checkSubscribedServiceInformationForChanges(mapCurrentSubscribedServices);
			mapSubscribedServices_ = mapCurrentSubscribedServices;

			// Finally: Some house-work
			removeConnectToService();
		}
		catch(Exception io)
		{
			IDEBUG() << "ServiceManager connect exception: " << io.what() ;
			waitCondition_.wait(&mutex_, 1000);
		}
		catch(...)
		{
			IDEBUG() << "Unhandled ServiceManager connect exception!" ;
			waitCondition_.wait(&mutex_, 1000);
		}
	}
	mutex_.unlock();

	IFATAL() << "ServiceConnectionManager:run() Stopped!";
}

/*---------------------------------------------------------------------------
	private non thread safe functions
---------------------------------------------------------------------------*/
void ServiceConnectionManager::initConnectionManager(InterfaceHandle serviceManagerHandle)
{
	if(serviceManagerProxy_ == NULL)
	{
		serviceManagerProxy_ = new ServiceManagerProxy(serviceManagerHandle);
		waitCondition_.wakeAll();
	}
}

// Not thread safe -> mutex from outside
// Register or remove services through ServiceManager::ServiceRegistration(MapServices);
bool ServiceConnectionManager::serviceRegistration()
{
	if(mapServicesToRegisterOrRemove_.empty()) return false;

	serviceManagerProxy_->ServiceRegistration(mapServicesToRegisterOrRemove_);
	for(MapServiceNameHostInformation::iterator it = mapServicesToRegisterOrRemove_.begin(), it_end = mapServicesToRegisterOrRemove_.end(); it != it_end; ++it)
	{
		string serviceName = it->first;
		HostInformation &serviceInfo = it->second;

		// -- debug --
		ASSERT(!serviceInfo.GetComponentName().empty());
		ASSERT(serviceName == serviceInfo.GetComponentName());
		// -- debug --

		if(serviceInfo.GetOnlineStatus() == HostInformation::ONLINE)
		{
			if(mapRegisteredServices_.count(serviceName) > 0)
				IDEBUG() << "WARNING! Already registered service " << serviceName ;
			mapRegisteredServices_[serviceName] = serviceInfo;
		}
		// -- start debug --
		// else service is OFFLINE and should be already removed!
		else if(serviceInfo.GetOnlineStatus() == HostInformation::OFFLINE)
		{
			if(mapRegisteredServices_.count(serviceName) > 0)
				IDEBUG() << "WARNING! Service should be removed already " << serviceName ;
			ASSERT(mapRegisteredServices_.count(serviceName) == 0);
		}
		else
		{
			IDEBUG() << "ERROR! service NO_ONLINE_STATUS! " << serviceInfo ;
			ASSERT(serviceInfo.GetOnlineStatus() != HostInformation::NO_ONLINE_STATUS);
		}
		// -- end debug --
	}

	mapServicesToRegisterOrRemove_.clear();

	return true;
}

// not thread-safe -> mutex outside
bool ServiceConnectionManager::removeConnectToService()
{
	for(ListConnectToService::iterator it = listConnectToService_.begin(), it_end = listConnectToService_.end(); it != it_end; ++it)
	{
		ConnectToService *connect = *it;
		ASSERT(connect);

		if(!connect->isRunning())
			delete connect;

		it = listConnectToService_.erase(it);
	}

	return true;
}

/*---------------------------------------------------------------------------
- Check if service-information has changed (timestamps), if yes, then use callbacks to each registered class ComponentBase*
- The callbacks should include Set<HostInformation> SetServiceInformation to enable each component to update InterfaceHandles etc.
- Refer: KDA LookupServerBase.h
- Not thread safe -> mutex outside
---------------------------------------------------------------------------*/
void ServiceConnectionManager::checkSubscribedServiceInformationForChanges(const MapServiceNameHostInformation &mapCurrentSubscribedServices)
{
	for(MapServiceNameHostInformation::const_iterator it = mapCurrentSubscribedServices.begin(), it_end = mapCurrentSubscribedServices.end(); it != it_end; ++it)
	{
		const HostInformation &newServiceInformation = it->second;
		const std::string &serviceName = it->first;

		if(mapSubscribedServices_.count(serviceName) <= 0) // The ServiceInformation is new, so skip over?
		{
			IDEBUG() << " Service " << serviceName << " is new! Skipping!";
			continue;
		}

		const HostInformation &currentServiceInformation = mapSubscribedServices_[serviceName];

		// -- Is stub/publisher OFFLINE?
		if(newServiceInformation.GetOnlineStatus() == HostInformation::OFFLINE)
		{
			IDEBUG() << " Service " << serviceName << " is OFFLINE! TODO: Update states?";
			/*if(currentServiceInformation.GetOnlineStatus() == HostInformation::ONLINE)
			{
				INetInterface *inetInterface = mapServiceToInterface_[subscriberInfo.GetComponentName()];
				ASSERT(inetInterface);
				bool ret = INetConnectionManager::GetOrCreate()->RemoveInterface(currHandle, inetInterface);
				ASSERT(ret);
			}*/
		}
		else
		{
			updateSubscribersToServices(newServiceInformation, currentServiceInformation);
		}
	}
}

void ServiceConnectionManager::updateSubscribersToServices(const HostInformation &newServiceInfo, const HostInformation &currServiceInfo)
{
	ASSERT(newServiceInfo.IsValid());
	if(mapServiceToSubscribers_.count(newServiceInfo.GetComponentName()) <= 0)
	{
		//ICRITICAL() << "No services are subscribing to " << newServiceInfo.GetComponentName();
		return;
	}

	const SetServiceName &setSubscribers = mapServiceToSubscribers_[newServiceInfo.GetComponentName()];
	for(SetServiceName::const_iterator it = setSubscribers.begin(), it_end = setSubscribers.end(); it != it_end; ++it)
	{
		string subscriberName = *it;
		ASSERT(mapRegisteredServices_.count(subscriberName) > 0);

		HostInformation &subscriberInfo = mapRegisteredServices_[subscriberName];

		// -- debug --
		ASSERT(subscriberInfo.GetHostDescription() == HostInformation::SUBSCRIBER ||
				subscriberInfo.GetHostDescription() == HostInformation::PROXY);

		if(newServiceInfo.GetTimeStamp() != currServiceInfo.GetTimeStamp())
			IDEBUG() << newServiceInfo.GetComponentName() << " is " << newServiceInfo.GetOnlineStatus()
				 << ". Timestamp changed " << newServiceInfo.GetTimeStamp() << " != " << currServiceInfo.GetTimeStamp();
		// -- end debug --

		InterfaceHandle newHandle(newServiceInfo.GetHostName(), newServiceInfo.GetPortNumber());
		InterfaceHandle currHandle(currServiceInfo.GetHostName(), currServiceInfo.GetPortNumber());

		// -- prechecks --
		if(newHandle == currHandle)	continue;
		if(mapServiceToInterface_.count(subscriberInfo.GetComponentName()) <= 0)
		{
			IDEBUG() << " no INetInterface for " << subscriberInfo.GetComponentName();
			continue;
		}
		// -- end prechecks --

		INetInterface *inetInterface = mapServiceToInterface_[subscriberInfo.GetComponentName()];
		ASSERT(inetInterface);

		INetConnectionManager *manager = INetConnectionManager::GetOrCreate();
		ASSERT(manager);

		switch(subscriberInfo.GetHostDescription())
		{
			case HostInformation::SUBSCRIBER:
			{
				// -- debug --
				ASSERT(newServiceInfo.GetHostDescription() == HostInformation::PUBLISHER);
				ASSERT(newHandle == currHandle);
				IDEBUG()  << newServiceInfo.GetComponentName() << " " << newHandle << " != " << currHandle ;
				// -- debug --

				bool ret = manager->RegisterInterface(newHandle, inetInterface);
				ASSERT(ret);
				ret = manager->RemoveInterface(currHandle, inetInterface);
				ASSERT(ret);
				break;
			}
			case HostInformation::PROXY:
			{
				// -- debug --
				ASSERT(newServiceInfo.GetHostDescription() == HostInformation::STUB);
				ASSERT(newHandle != currHandle);
				IDEBUG()  << newServiceInfo.GetComponentName() << " " << newHandle << " != " << currHandle ;
				// -- debug --

				bool ret = manager->RegisterInterface(newHandle, inetInterface);
				ASSERT(ret);
				ret = manager->RemoveInterface(currHandle, inetInterface);
				ASSERT(ret);
				break;
			}
			default:
				// TODO: IPSUBSCRIBER, IPPUBLISHER versus ALMSUBSCRIBER, ALMPUBLISHER
				IDEBUG()  << "Unknown host description for subscriber : " << subscriberInfo ;
				break;
		}
	}
}
/*---------------------------------------------------------------------------
	Public thread safe functions
		- Subscribe to service
---------------------------------------------------------------------------*/
bool ServiceConnectionManager::SubscribeService(HostInformation &subscriberInfo, const string &serviceName, HostInformation &serviceInfo, bool wait)
{
	// -- debug --
	ASSERT(subscriberInfo.GetHostDescription() == HostInformation::PROXY || subscriberInfo.GetHostDescription() == HostInformation::PUBLISHER);
	ASSERT(!subscriberInfo.GetComponentName().empty());
	// -- debug --

	if(subscriberInfo.GetComponentName() == Runtime::GetComponentName())
		subscriberInfo.SetComponentName(subscriberInfo.GetComponentName() + ".connectTo." + serviceName);

	subscriberInfo.SetConnectedToName(serviceName);
	RegisterService(subscriberInfo);		// registers subscriberInfo as a subscriber-service

	return SubscribeService(serviceName, serviceInfo, wait);
}

bool ServiceConnectionManager::SubscribeService(const string &serviceName, HostInformation &serviceInfo, bool wait)
{
	try
	{
		mutex_.lock();

		if(mapSubscribedServices_.count(serviceName) > 0)
		{
			// -- debug --
			ASSERT(setSubscribedServices_.count(serviceName) > 0);
			// -- end debug --

			serviceInfo = mapSubscribedServices_[serviceName];
			mutex_.unlock();
			return true;
		}

		if(setSubscribedServices_.count(serviceName) <= 0)
			setSubscribedServices_.insert(serviceName);

		if(wait)
		{
			waitCondition_.wakeAll();

			HostInformation serviceInfo;

			// -- debug --
			ASSERT(!serviceInfo.IsValid());
			ASSERT(serviceInfo.GetOnlineStatus() == HostInformation::OFFLINE);
			// -- debug --

			while(serviceInfo.GetOnlineStatus() != HostInformation::ONLINE)
			{
				if(mapSubscribedServices_.count(serviceName) > 0)
					serviceInfo = mapSubscribedServices_[serviceName];

				// -- debug --
				iDebug() << COMPONENT_FUNCTION << " Waiting for " << serviceName;
				// -- end debug --

				if(serviceInfo.GetOnlineStatus() != HostInformation::ONLINE)
					waitCondition_.wait(&mutex_, 5000);
			}
		}

		if(mapSubscribedServices_.count(serviceName) > 0)
		{
			serviceInfo = mapSubscribedServices_[serviceName];
			mutex_.unlock();
			return true;
		}
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Unhandled excpetion caught!" ;
	}

	mutex_.unlock();

	return false;
}

bool ServiceConnectionManager::SubscribeService(INetInterface *inetInterface, bool wait)
{
	// -- debug --
	ASSERT(inetInterface);
	ASSERT(inetInterface->GetInterfaceType() == HostInformation::PROXY || inetInterface->GetInterfaceType() == HostInformation::SUBSCRIBER);
	ASSERT(!inetInterface->GetServiceName().empty());
	ASSERT(!inetInterface->GetConnectedToName().empty());
	ASSERT(!inetInterface->GetInterfaceName().empty());
	// -- debug --

	try
	{
		ConnectToService *connect = NULL;

		bool ret = RegisterService(inetInterface);
		{
			MutexLocker lock(&mutex_);

			// -- debug --
			if(!ret) ASSERT(mapServiceToInterface_.count(inetInterface->GetServiceName()) > 0);
			// -- debug --

			if(mapSubscribedServices_.count(inetInterface->GetConnectedToName()) > 0)
			{
				if(mapSubscribedServices_[inetInterface->GetConnectedToName()].GetOnlineStatus() == HostInformation::ONLINE)
				{
					// -- debug --
					ASSERT(setSubscribedServices_.count(inetInterface->GetConnectedToName()) > 0);
					ASSERT(mapServiceToInterface_.count(inetInterface->GetServiceName()) > 0);
					//ASSERT(mapServiceToInterface_.count(inetInterface->GetConnectedToName()) > 0);
					// -- debug --

					IWARNING() << inetInterface->GetServiceName() << " is already subscribing to " << inetInterface->GetConnectedToName();
					return true;
				}
			}

			connect = new ConnectToService(this, inetInterface);
			listConnectToService_.push_back(connect);
		}

		ASSERT(connect);

		if(wait && connect)
		{
			bool ret = connect->WaitForService();
			if(!ret)
			{
				IWARNING() << "Could not connect to service!";
				return false;
			}
		}

		return true;
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Unhandled excpetion caught!" ;
	}

	return false;
}


/*---------------------------------------------------------------------------
	Unsubscribe service
	TODO: Review the functionality!!
		-> Looks like it does not work!
		-> Remove service from INetConnection
		-> Do I need an UnsubscribeService?
		-> Add RemoveService(INetInterface *inet);
---------------------------------------------------------------------------*/
bool ServiceConnectionManager::UnsubscribeService(const string &subscriberName, const string &serviceName)
{
	bool ret = RemoveService(subscriberName);				// really: removes subscriber

	if(mapSubscribedServices_.count(serviceName) > 0)
		mapSubscribedServices_.erase(serviceName);

	if(setSubscribedServices_.count(serviceName) > 0)
		setSubscribedServices_.erase(serviceName);

	if(mapServiceToInterface_.count(subscriberName) > 0)
		mapServiceToInterface_.erase(subscriberName);

	if(mapServiceToSubscribers_.count(serviceName) > 0)
		mapServiceToSubscribers_[serviceName].erase(subscriberName);

	return ret;
}

/*---------------------------------------------------------------------------
	Register/Remove services (Stub/Publisher)
---------------------------------------------------------------------------*/
bool ServiceConnectionManager::RegisterService(HostInformation &handle)
{
	MutexLocker lock(&mutex_);

	ASSERT(!handle.GetComponentName().empty());

	if(mapServicesToRegisterOrRemove_.count(handle.GetComponentName()) >= 1)
	{
		IWARNING() << "Already registering service " << handle.GetComponentName();
		ASSERT(handle.CompareTo(mapServicesToRegisterOrRemove_[handle.GetComponentName()]));
		return false;
	}

	handle.SetOnlineStatus(HostInformation::ONLINE);
	handle.SetTimeStamp(BaseLib::GetTimeStampInteger());

	mapServicesToRegisterOrRemove_[handle.GetComponentName()] = handle;
	waitCondition_.wakeAll();

	IDEBUG() << "Registered service " << handle.GetComponentName();
	return true;
}

//TODO: OnlineStatus is set to ONLINE but is that always the case?
bool ServiceConnectionManager::RegisterService(INetInterface* inetInterface)
{
	// -- debug --
	ASSERT(inetInterface);
	ASSERT(!inetInterface->GetServiceName().empty());
	ASSERT(!inetInterface->GetInterfaceName().empty());
	// -- debug --

	MutexLocker lock(&mutex_);

	if(mapServicesToRegisterOrRemove_.count(inetInterface->GetServiceName()) >= 1)
	{
		IWARNING() << "Already registering service " << inetInterface->GetServiceName();
		return false;
	}

	HostInformation info = Runtime::GetHostInformation();
	info.SetComponentName(inetInterface->GetServiceName());
	info.SetConnectedToName(inetInterface->GetConnectedToName());
	info.SetInterfaceName(inetInterface->GetInterfaceName());
	info.SetHostDescription(inetInterface->GetInterfaceType());
	info.SetOnlineStatus(HostInformation::ONLINE);
	info.SetTimeStamp(BaseLib::GetTimeStampInteger());

	mapServiceToInterface_[inetInterface->GetServiceName()] = inetInterface;
	mapServicesToRegisterOrRemove_[inetInterface->GetServiceName()] = info;

	// Get INetConnection from INetConnectionManager
	INetConnectionManager *conn = INetConnectionManager::GetOrCreate();
	ASSERT(conn);

	switch(inetInterface->GetInterfaceType())
	{
		case HostInformation::PROXY:
		case HostInformation::SUBSCRIBER:
		{
			ASSERT(!inetInterface->GetConnectedToName().empty());

			if(setSubscribedServices_.count(inetInterface->GetConnectedToName()) <= 0)
				setSubscribedServices_.insert(inetInterface->GetConnectedToName());

			mapServiceToSubscribers_[inetInterface->GetConnectedToName()].insert(inetInterface->GetServiceName());
			break;
		}
		case HostInformation::STUB:
		case HostInformation::PUBLISHER:
		{
			INetServerListener *server = conn->GetOrCreateServerListener(true);
			ASSERT(server);

			bool ret = server->RegisterInterface(inetInterface);
			ASSERT(ret);

			break;
		}
		default:
			ICRITICAL() << " Could not recognize interface type for: " << inetInterface->GetServiceName();
			break;
	}

	// -- debug --
	IDEBUG() << "Registered service " << inetInterface->GetServiceName();
	// -- debug --

	waitCondition_.wakeAll();

	return true;
}

bool ServiceConnectionManager::RemoveService(const string &serviceName)
{
	MutexLocker lock(&mutex_);

	// -- debug --
	ASSERT(!serviceName.empty());
	// -- debug --

	// -- prechecks --
	if(serviceName.empty())
	{
		ICRITICAL() << " Empty service name " << serviceName;
		return false;
	}
	if(mapRegisteredServices_.count(serviceName) <= 0)
	{
		IWARNING() << "WARNING! Already removed service " << serviceName;
		return false;
	}
	// -- end prechecks --

	HostInformation info = mapRegisteredServices_[serviceName];
	mapRegisteredServices_.erase(serviceName);

	if(mapServicesToRegisterOrRemove_.count(info.GetComponentName()) >= 1)
	{
		IWARNING() << "WARNING! Already removing service " << info.GetComponentName();
		return false;
	}

	// Remove INetInterface* from INetConnection
	if(mapServiceToInterface_.count(serviceName) > 0)
	{
		INetInterface *inetInterface = mapServiceToInterface_[serviceName];
		ASSERT(inetInterface->GetServiceName() == serviceName);

		InterfaceHandle handle(info.GetHostName(), info.GetPortNumber());
		ASSERT(handle.IsValid());

		bool ret = INetConnectionManager::GetOrCreate()->RemoveInterface(handle, inetInterface);

		// TODO: Add switch case for PROXY/STUB
		switch(inetInterface->GetInterfaceType())
		{
			case HostInformation::PROXY:
			case HostInformation::SUBSCRIBER:
			{
				// -- debug --
				IDEBUG() << "Removed: " << inetInterface->GetServiceName() << "->" << inetInterface->GetConnectedToName();
				ASSERT(ret);
				ASSERT(mapServiceToInterface_.count(serviceName) > 0);
				ASSERT(mapServiceToSubscribers_.count(inetInterface->GetConnectedToName()) > 0);
				ASSERT(mapServiceToSubscribers_[inetInterface->GetConnectedToName()].count(inetInterface->GetServiceName()));
				// -- debug --

				mapServiceToInterface_.erase(serviceName);
				mapServiceToSubscribers_[inetInterface->GetConnectedToName()].erase(inetInterface->GetServiceName());
				break;
			}
			case HostInformation::STUB:
			case HostInformation::PUBLISHER:
			{
				// -- debug --
				IDEBUG() << "Removed: " << inetInterface->GetServiceName() << "->" << inetInterface->GetConnectedToName();
				ASSERT(ret);
				ASSERT(mapServiceToInterface_.count(serviceName) > 0);
				//ASSERT(mapServiceToSubscribers_.count(inetInterface->GetServiceName()) > 0);
				//ASSERT(mapServiceToSubscribers_[inetInterface->GetConnectedToName()].count(inetInterface->GetServiceName()));
				// -- debug --

				// TODO: review functionality! Stub is not connected to anybody!
				mapServiceToInterface_.erase(serviceName);
				//mapServiceToSubscribers_[inetInterface->GetConnectedToName()].erase(inetInterface->GetServiceName());
				break;
			}
		case HostInformation::NO_HOST_DESCRIPTION:
		case HostInformation::COMPONENT_FACTORY:
		case HostInformation::LOOKUP_SERVER:
		case HostInformation::SYSTEM_MANAGER:
		case HostInformation::SETTINGS_MANAGER:
		case HostInformation::COMPONENT:
				ICRITICAL() << " Not handled!";
				break;
		}
	}
	else
	{
		IWARNING() << "Could not find INetInterface for " << serviceName;
	}

	info.SetOnlineStatus(HostInformation::OFFLINE);
	mapServicesToRegisterOrRemove_[serviceName] = info;
	waitCondition_.wakeAll();

	IDEBUG() << "Removing service " << info.GetComponentName();

	return true;
}

} // namespace IntactMiddleware

