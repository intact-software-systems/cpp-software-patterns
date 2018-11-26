#ifndef IntactMiddleware_ServiceManagerConnection_h_IsIncluded
#define IntactMiddleware_ServiceManagerConnection_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/ServiceManagerBase.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{
// -------------------------------------------------------
//	ServiceManager connections for stub and publisher
// -------------------------------------------------------
/**
* Container class for Stub and Publisher connection to the ServiceManager
* The class cannot be created directly, but rather must be subclassed.
*/
class DLL_STATE StubPublisherService : public Thread
{
private:
	ServiceManagerProxy	*serviceManagerProxy;
	HostInformation		myHostInformation;
	InterfaceHandle		serviceManagerHandle;
	Mutex				receivedHostInfoMutex;
	WaitCondition		receivedHostInfoCondition;
	bool				runThread;

protected:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Stub or Publisher HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	StubPublisherService(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true);
	
	friend class ServiceManagerConnectionManager;

	/**
	* Stops the connection to the ServiceManager.
	*/
	virtual ~StubPublisherService(void);

protected:
	/**
	* Registers Stub or Publisher with the ServiceManager.
	*
	* @return bool		true if successfully registered with ServiceManager.
	*/
	virtual bool registerServiceManager(bool login);

	/**
	* Main loop that sends "I am alive" messages to ServiceManager
	*/
	virtual void run();

	/**
	* Gets or creates a ServiceManagerProxy.
	*/
	ServiceManagerProxy* getOrCreateProxy(InterfaceHandle serviceManagerHandle);

public:
	//inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(0, ""); }

	/**
	* Returns the HostInformation for the Stub or Publisher service.
	*
	* @return HostInformation		The HostInformation for the Stub or Publisher.
	*/
	HostInformation GetHostInformation(bool wait = false);

	/**
	* Deregisters from ServiceManager, and shuts down the thread.
	*/
	inline void Shutdown()		{ runThread = false; }
};

/**
* Specialized class for Stub connections to the ServiceManager
*/
class DLL_STATE StubServiceManager : public StubPublisherService
{
public:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Stub HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	StubServiceManager(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true)
		: StubPublisherService(info, lookupHandle, autoStart)
	{}
};

/**
* Specialized class for Publisher connections to the ServiceManager
*/
class DLL_STATE PublisherServiceManager : public StubPublisherService
{
public:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Publisher HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	PublisherServiceManager(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true)
		: StubPublisherService(info, lookupHandle, autoStart)
	{}
};

// -------------------------------------------------------
//	ServiceManager connections for proxy and subscriber
// -------------------------------------------------------
class ConnectToStubThread;
/**
* Container class for Proxy and Subscriber connection to the ServiceManager
* The class cannot be created directly, but rather must be subclassed.
*/
class DLL_STATE ProxySubscriberService : public Thread
{
private:
	RmiBase				*subscriberBase;
	RmiClient			*proxyBase;
	ServiceManagerProxy	*serviceManagerProxy;
	ConnectToStubThread *reConnectToStubThread;

	Mutex				connectedHostInfoMutex;
	WaitCondition		connectedHostInfoCondition;
	HostInformation		myHostInformation;
	InterfaceHandle		serviceManagerHandle;
	string				connectedComponentName;
	HostInformation		connectedHostInformation;
	int					currentStubTimeStamp;
	bool				runThread;

protected:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Proxy or Subscriber HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param connectedName	The Stub the Proxy is interested in, or the Publisher the Subscriber is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	ProxySubscriberService(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true);
	
	friend class ServiceManagerConnectionManager;

	/**
	* Stops the connection to the ServiceManager.
	*/
	virtual ~ProxySubscriberService(void);

protected:
	/**
	* Registers Stub or Publisher with the ServiceManager.
	*
	* @return bool		true if successfully registered with ServiceManager.
	*/
	virtual bool registerServiceManager(bool login);

	/**
	* Main loop that sends "I am alive" messages to ServiceManager
	*/
	virtual void run();

	/**
	* Updates the connectedHostInformation (Stub/Publisher) and notifies waiting threads.
	*
	* @param componentInfo		The new connectedHostInformation.
	* @param notify				If true then the monitor notifies waiting threads.
	*/
	inline void updateConnectedHostInformation(const HostInformation &componentInfo, bool notify = false)
	{
		connectedHostInfoMutex.lock();
		currentStubTimeStamp = componentInfo.GetTimeStamp();
		connectedHostInformation = componentInfo;
		if(notify)
		{
			connectedHostInfoCondition.wakeAll();
		}
		connectedHostInfoMutex.unlock();
	}
	
	/**
	* Gets or creates a ServiceManagerProxy.
	*/
	ServiceManagerProxy* getOrCreateProxy(InterfaceHandle serviceManagerHandle);

public:
	//inline virtual InterfaceHandle GetInterfaceHandle()		{ return InterfaceHandle(0, ""); }

	/**
	* Returns the HostInformation for the Proxy or Subscriber.
	*
	* @return HostInformation		The HostInformation for the Proxy or Subscriber.
	*/
	inline HostInformation GetHostInformation()				{ return myHostInformation; }

	/**
	* Sets that SubscriberBase class, which is needed in case a restart is required.
	* A restart is required if the Publisher changes the port and/or multicastaddress.
	*
	* @param sub		SubscriberBase class.
	*/
	inline void SetSubscriberBase(RmiBase *sub)		{ subscriberBase = sub; }

	/**
	* Sets that Proxy class, which is needed in case a restart is required.
	* A restart is required if the Stub is restarted.
	*
	* @param proxy		Proxy class.
	*/
	inline void SetProxy(RmiClient *proxy)						{ proxyBase = proxy; }

	/**
	* Returns the HostInformation for the Stub or Publisher.
	*
	* @param wait					If wait is true, then the function waits until the ServiceManager 
	*								has responded with the HostInformation for the Stub or Publisher.
	* @return HostInformation		The HostInformation for the Proxy or Subscriber.
	*/
	inline HostInformation GetConnectedHostInformation(bool wait = false)
	{ 
		connectedHostInfoMutex.lock();
		
		if(connectedHostInformation.GetComponentName().empty() && wait == true)
			connectedHostInfoCondition.wait(&connectedHostInfoMutex);
		
		HostInformation info = connectedHostInformation;
		connectedHostInfoMutex.unlock();
	
		return info; 	
	}

	/**
	* Deregisters from ServiceManager, and shuts down the thread.
	*/
	inline void Shutdown()		{ runThread = false; }
};

/**
* Specialized class for Proxy connections to the ServiceManager
*/
class DLL_STATE ProxyServiceManager : public ProxySubscriberService
{
public:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Proxy HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param connectedName	The Stub the Proxy is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	ProxyServiceManager(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true)
		: ProxySubscriberService(info, lookupHandle, connectedName, autoStart)
	{}
};

/**
* Specialized class for Subscriber connections to the ServiceManager
*/
class DLL_STATE SubscriberServiceManager : public ProxySubscriberService
{
public:
	/**
	* Creates and maintains a connection to the ServiceManager.
	*
	* @param info			The Subscriber HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the ServiceManager.
	* @param connectedName	The Publisher the Subscriber is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	SubscriberServiceManager(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true)
		: ProxySubscriberService(info, lookupHandle, connectedName, autoStart)
	{}
};


class DLL_STATE ServiceManagerConnectionManager
{
private:
	typedef set<Thread*>	SetComponent;
	static SetComponent		setComponent;
	static Mutex			guard;

public:
	ServiceManagerConnectionManager() {}
	~ServiceManagerConnectionManager() {}

	static bool AddConnection(Thread*);
	static bool RemoveConnection(Thread*);
	static void ReleaseAllConnections();
};

namespace ServiceManagerBaseSpace
{

/**
* Registers service to ServiceManager
*
* @param serviceManager		Interface handle to the ServiceManager
* @param myHostInfo			Service' host information
*/
DLL_STATE StubServiceManager* RegisterStubServiceToServiceManager(InterfaceHandle serviceManager, HostInformation myHostInformation);

/**
* Registers service to ServiceManager
*
* @param serviceManager		Interface handle to the ServiceManager
* @param myHostInfo			Service' host information
*/
DLL_STATE PublisherServiceManager* RegisterPublisherServiceToServiceManager(InterfaceHandle serviceManager, HostInformation myHostInformation);


/**
* Creates a connection to the ServiceManager for a Stub
*
* @param lookup				Interface handle to the ServiceManager
* @param myComponentName    Stub's componentName
* @param myPortNumber		Stub's port-number
* @param autoStart			Connect to ServiceManager automatically.
*/
DLL_STATE StubServiceManager* CreateStubConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Stub
*
* @param lookup				Interface handle to the ServiceManager
* @param myHostInfo			Stub's host information
* @param myComponentName	Stub's componentName
* @param autoStart			Connect to ServiceManager automatically.
*/
DLL_STATE StubServiceManager* CreateStubConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Publisher
*
* @param lookup				Interface handle to the ServiceManager
* @param myHostInfo			Publisher's host information
* @param myComponentName	Publisher's componentName
* @param autoStart			Connect to ServiceManager automatically.
*/
DLL_STATE PublisherServiceManager* CreatePublisherConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Publisher
*
* @param lookup				Interface handle to the ServiceManager
* @param myComponentName    Publisher's name
* @param myPortNumber		Publisher's port-number
* @param myMulticastAddress Publisher's multicast address
* @param autoStart			Connect to ServiceManager automatically.
*/
DLL_STATE PublisherServiceManager* CreatePublisherConnection(InterfaceHandle lookup, string myComponentName, string myMulticastAddress, int myPortNumber, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Proxy
*
* @param lookup				Interface handle to the ServiceManager
* @param myHostInfo			Proxy's host information
* @param myComponentName	Proxy's componentName
* @param connectingToComponentName	Proxy is connecting to Stub with this name
* @param autoStart			Connect to ServiceManager automatically.
*/
DLL_STATE ProxyServiceManager* CreateProxyConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Proxy
*
* @param lookup						Interface handle to the ServiceManager
* @param myComponentName			Proxy's name
* @param myPortNumber				Proxy's port-number
* @param connectingToComponentName	Proxy is connecting to Stub with this name
* @param autoStart					Connect to ServiceManager automatically.
*/
DLL_STATE ProxyServiceManager* CreateProxyConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Subscriber
*
* @param lookup						Interface handle to the ServiceManager
* @param myHostInfo					Subscriber's host information
* @param myComponentName			Subscriber's componentName
* @param connectingToComponentName	Subscriber is connecting to Publisher with this name
* @param autoStart					Connect to ServiceManager automatically.
*/
DLL_STATE SubscriberServiceManager* CreateSubscriberConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the ServiceManager for a Subscriber
*
* @param lookup						Interface handle to the ServiceManager
* @param myComponentName			Subscriber's name
* @param myPortNumber				Subscriber's port-number
* @param connectingToComponentName	Subscriber is connecting to Publisher with this name
* @param autoStart					Connect to ServiceManager automatically.
*/
DLL_STATE SubscriberServiceManager* CreateSubscriberConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, string connectingToComponentName, bool autoStart = true);

/**
* Registers Subscriber in ServiceManager, and waits for service information to be available.
*
* @param lookup						Interface handle to the ServiceManager
* @param myComponentName			Subscriber's name
* @param serviceInServiceManager		Subscriber is interested in service (typicall PUBLISHER).
* @param serviceInformation			Service' HostInformation
*/
DLL_STATE SubscriberServiceManager* GetAndMonitorPublisherServiceInformation(InterfaceHandle serviceManager,
										  HostInformation myHostInformation, 
										  string serviceInServiceManager,
										  HostInformation &serviceInformation);

/**
* Get interface handle for a running service. This method blocks until the service 
* becomes available.
*
* @param serviceManager				Interface handle to the ServiceManager
* @param componentName				Service name
* @return host information for service
*/
DLL_STATE InterfaceHandle GetHandleFromServiceManager(InterfaceHandle& serviceManager, string componentName);

} // namespace ServiceManagerBaseSpace

} // namespace IntactMiddleware

#endif // 


