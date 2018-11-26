#ifndef IntactMiddleware_ServiceConnectionManager_h_IsIncluded
#define IntactMiddleware_ServiceConnectionManager_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{

class ServiceManagerProxy;
class ConnectToService;

/* -------------------------------------------------------
	ServiceManager connections for stub and publisher
	Include:
		- Register Stub as service
		- Register Proxy as listener to Stub?
		- Register Publisher as service
		- Register Subscriber as listener to Publisher?
		- Register Group as a service
		- Register GroupParticipant as a participant of a Group? Done by GroupManager

	 TODO:
		 - RegisterService should also include a ComponentBase object
		 - This provides ServiceConnectionManager a callback to ComponentBase::Initialize if the ServiceInformation has changed!
		 - For example: ServiceInformation has changed if time-stamp is more recent
		 - Only needed if the Component includes has a Proxy or Subscriber, that is, if SubscribeServiceInformation is called
		 - This way each component can be reinitialized if needed!

	TODO:
	ServiceConnectionManager and INetConnectionManager:
		- INetConnectionManager has RegisterInterface/RemoveInterface
		- ServiceConnectionManager should use INetConnectionManager to create a connection, and register INetInterface* to an INetConnection.
		- Similarly when RemoveService is invoked, remove INetInterface from INetConnection.

 -------------------------------------------------------*/
class DLL_STATE ServiceConnectionManager : public Thread
{
public:
	typedef std::map<std::string, MicroMiddleware::HostInformation>		MapServiceNameHostInformation;
	typedef std::set<std::string>										SetServiceName;

	// Map a service to potentially multiple subscribers
	typedef std::map<std::string, SetServiceName>						MapServiceToSubscribers;
	typedef std::map<std::string, MicroMiddleware::INetInterface*>		MapServiceToInterface;
	typedef std::list<ConnectToService*>								ListConnectToService;

private:
	ServiceConnectionManager(MicroMiddleware::InterfaceHandle lookupHandle, bool autoStart = true);
	virtual ~ServiceConnectionManager(void);

private:
	virtual void run();

public:
	static ServiceConnectionManager* GetOrCreate(MicroMiddleware::InterfaceHandle *handle = NULL);

public:
	bool RegisterService(MicroMiddleware::HostInformation &handle);
	bool RegisterService(MicroMiddleware::INetInterface *inet);
	bool RemoveService(const string &serviceName);
	//bool RemoveService(MicroMiddleware::INetInterface *inet);

	bool SubscribeService(MicroMiddleware::HostInformation &subscriberInfo, const std::string &serviceName, MicroMiddleware::HostInformation &serviceInfo, bool wait = true);
	bool SubscribeService(const std::string &serviceName, MicroMiddleware::HostInformation &serviceInfo, bool wait = true);
	bool SubscribeService(MicroMiddleware::INetInterface *inet, bool wait = true);
	
	bool UnsubscribeService(const std::string &subscriberName, const std::string &serviceName);

public:
	bool 				WaitConnected();
	HostInformation 	WaitForService(const std::string &serviceName);

	inline void 					Shutdown()						{ MutexLocker lock(&mutex_); runThread_ = false; }
	inline void 					WakeUpAll()						{ MutexLocker lock(&mutex_); waitCondition_.wakeAll(); }
	MapServiceNameHostInformation 	GetSubscribedServices() const 	{ MutexLocker lock(&mutex_); return mapSubscribedServices_; } 

private:
	void initConnectionManager(MicroMiddleware::InterfaceHandle serviceManagerHandle);
	bool serviceRegistration();
	bool removeConnectToService();
	void checkSubscribedServiceInformationForChanges(const MapServiceNameHostInformation &mapCurrentSubscribedServices);
	void updateSubscribersToServices(const HostInformation &newServiceInfo, const HostInformation &currServiceInfo);


private:
	MicroMiddleware::InterfaceHandle	serviceManagerHandle_;
	ServiceManagerProxy					*serviceManagerProxy_;

private:
	mutable Mutex			mutex_;
	WaitCondition			waitCondition_;
	bool					runThread_;

private:
	MapServiceNameHostInformation   mapServicesToRegisterOrRemove_;

	MapServiceNameHostInformation	mapRegisteredServices_;
	MapServiceNameHostInformation   mapSubscribedServices_;
	SetServiceName					setSubscribedServices_;
	MapServiceToSubscribers			mapServiceToSubscribers_;
	MapServiceToInterface			mapServiceToInterface_;
	ListConnectToService			listConnectToService_;

private:
	static ServiceConnectionManager *serviceConnectionManager_;
};

}

#endif // namespace IntactMiddleware

