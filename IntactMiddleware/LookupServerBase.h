#ifndef IntactMiddleware_LookupServerBase_h_IsIncluded
#define IntactMiddleware_LookupServerBase_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"

using namespace std;

#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{
class StubLookupService;
class PublisherLookupService;
class ProxyLookupService;
class SubscriberLookupService;

namespace LookupServerBaseSpace
{

/**
* Registers service to LookupServer
*
* @param lookupServer		Interface handle to the LookupServer
* @param myHostInfo			Service' host information
*/
DLL_STATE StubLookupService* RegisterStubServiceToLookupServer(InterfaceHandle lookupServer, HostInformation myHostInformation);

/**
* Registers service to LookupServer
*
* @param lookupServer		Interface handle to the LookupServer
* @param myHostInfo			Service' host information
*/
DLL_STATE PublisherLookupService* RegisterPublisherServiceToLookupServer(InterfaceHandle lookupServer, HostInformation myHostInformation);


/**
* Creates a connection to the LookupServer for a Stub
*
* @param lookup				Interface handle to the LookupServer
* @param myComponentName    Stub's componentName
* @param myPortNumber		Stub's port-number
* @param autoStart			Connect to LookupServer automatically.
*/
DLL_STATE StubLookupService* CreateStubConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Stub
*
* @param lookup				Interface handle to the LookupServer
* @param myHostInfo			Stub's host information
* @param myComponentName	Stub's componentName
* @param autoStart			Connect to LookupServer automatically.
*/
DLL_STATE StubLookupService* CreateStubConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Publisher
*
* @param lookup				Interface handle to the LookupServer
* @param myHostInfo			Publisher's host information
* @param myComponentName	Publisher's componentName
* @param autoStart			Connect to LookupServer automatically.
*/
DLL_STATE PublisherLookupService* CreatePublisherConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Publisher
*
* @param lookup				Interface handle to the LookupServer
* @param myComponentName    Publisher's name
* @param myPortNumber		Publisher's port-number
* @param myMulticastAddress Publisher's multicast address
* @param autoStart			Connect to LookupServer automatically.
*/
DLL_STATE PublisherLookupService* CreatePublisherConnection(InterfaceHandle lookup, string myComponentName, string myMulticastAddress, int myPortNumber, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Proxy
*
* @param lookup				Interface handle to the LookupServer
* @param myHostInfo			Proxy's host information
* @param myComponentName	Proxy's componentName
* @param connectingToComponentName	Proxy is connecting to Stub with this name
* @param autoStart			Connect to LookupServer automatically.
*/
DLL_STATE ProxyLookupService* CreateProxyConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Proxy
*
* @param lookup						Interface handle to the LookupServer
* @param myComponentName			Proxy's name
* @param myPortNumber				Proxy's port-number
* @param connectingToComponentName	Proxy is connecting to Stub with this name
* @param autoStart					Connect to LookupServer automatically.
*/
DLL_STATE ProxyLookupService* CreateProxyConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Subscriber
*
* @param lookup						Interface handle to the LookupServer
* @param myHostInfo					Subscriber's host information
* @param myComponentName			Subscriber's componentName
* @param connectingToComponentName	Subscriber is connecting to Publisher with this name
* @param autoStart					Connect to LookupServer automatically.
*/
DLL_STATE SubscriberLookupService* CreateSubscriberConnection(InterfaceHandle lookup, HostInformation myHostInfo, string myComponentName, string connectingToComponentName, bool autoStart = true);

/**
* Creates a connection to the LookupServer for a Subscriber
*
* @param lookup						Interface handle to the LookupServer
* @param myComponentName			Subscriber's name
* @param myPortNumber				Subscriber's port-number
* @param connectingToComponentName	Subscriber is connecting to Publisher with this name
* @param autoStart					Connect to LookupServer automatically.
*/
DLL_STATE SubscriberLookupService* CreateSubscriberConnection(InterfaceHandle lookup, string myComponentName, int myPortNumber, string connectingToComponentName, bool autoStart = true);

/**
* Registers Subscriber in LookupService, and waits for service information to be available.
*
* @param lookup						Interface handle to the LookupServer
* @param myComponentName			Subscriber's name
* @param serviceInLookupServer		Subscriber is interested in service (typicall PUBLISHER).
* @param serviceInformation			Service' HostInformation
*/
DLL_STATE SubscriberLookupService* GetAndMonitorPublisherServiceInformation(InterfaceHandle lookupServer,
										  HostInformation myHostInformation, 
										  string serviceInLookupServer,
										  HostInformation &serviceInformation);

/**
* Get interface handle for a running service. This method blocks until the service 
* becomes available.
*
* @param lookupServer				Interface handle to the LookupServer
* @param componentName				Service name
* @return host information for service
*/
DLL_STATE InterfaceHandle GetHandleFromLookupServer(InterfaceHandle& lookupServer, string componentName);


} // namespace LookupServerBaseSpace


// -------------------------------------------------------
//				LookupServer Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the LookupServer.
* The interface is used actively by each service implemented in a component.
*/
class DLL_STATE LookupServerInterface : public RmiInterface
{
public:
	LookupServerInterface() { }
	~LookupServerInterface() { }

	enum MethodID {
		Method_ServiceRegistration	= 11,
		Method_ServiceInformation = 12,
		Method_AliveChecker = 13,
		Method_ShutDown = 14,
		Method_AllServiceInformation = 19,
		Method_ClearServices = 31
	};

	enum InterfaceId
	{
		LookupServerInterfaceId = 10
	};

	virtual int GetInterfaceId() const { return LookupServerInterface::LookupServerInterfaceId; }

	/**
	* Registers the service given by HostInformation in the LookupServer.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostInformation.
	*/
	virtual HostInformation ServiceRegistration(HostInformation info) = 0;
	
	/**
	* Retrieve HostInformation about a service identified by componentName.
	*
	* @param componentName	The name of the component (service) to retrieve HostInformation.
	* @return				HostInformation about the componentName.
	*/
	virtual HostInformation GetServiceInformation(string componentName) = 0;

	/**
	* Called by all component services to inform the LookupServer that it is connected/disconnected.
	* The componentName is used if the caller requires information whether a service is currently online.
	*
	* @param info			The (callers) service' HostInformation.
	* @param componentName	Name of a service that the caller is inquiring whether it is online or not.
	* @return				componentName's HostInformation.
	*/
	virtual HostInformation	AliveChecker(HostInformation info, string componentName) = 0;

	/**
	* Shuts down the LookupServer.
	*/
	virtual void ShutDown() = 0;
	
	/**
	* Returns all service information in the LookupServer in a map (string, HostInformatino).
	*
	* @return				Map containing all service information.
	*/
	virtual map<string, HostInformation> GetAllServiceInformation() = 0;
	
	/**
	* Clears Offline Services.
	*/
	virtual void ClearServices() = 0;
};

// -------------------------------------------------------
//				Interface RmiClient
//			- Sender/publisher class
// -------------------------------------------------------
class DLL_STATE LookupServerProxy : virtual public RmiClient, virtual public LookupServerInterface
{
public:
	RMICLIENT_COMMON(LookupServerProxy);

	virtual HostInformation ServiceRegistration(HostInformation info)
	{
		BeginMarshal(LookupServerInterface::Method_ServiceRegistration);
		info.Write(GetSocketWriter());
		EndMarshal();

        BeginUnmarshal();
		info.Read(GetSocketReader());
        EndUnmarshal();

		return info;
	}

	virtual HostInformation GetServiceInformation(string componentName)
	{
		BeginMarshal(LookupServerInterface::Method_ServiceInformation);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		HostInformation wrap;
		wrap.Read(GetSocketReader());
		EndUnmarshal();

		return wrap;
	}

	virtual HostInformation AliveChecker(HostInformation info, string componentName)
	{
		BeginMarshal(LookupServerInterface::Method_AliveChecker);
		info.Write(GetSocketWriter());
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		EndUnmarshal();
		
		return info;
	}

	virtual void ShutDown()
	{
		BeginMarshal(LookupServerInterface::Method_ShutDown);
		EndMarshal();
	}
	
	virtual map<string, HostInformation> GetAllServiceInformation()
	{
		BeginMarshal(LookupServerInterface::Method_AllServiceInformation);
		EndMarshal();

		BeginUnmarshal();

		map<string, HostInformation> mapComponentHostInformation;
		int sizeofmap = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sizeofmap; i++)
		{
			HostInformation info;
			info.Read(GetSocketReader());

			mapComponentHostInformation.insert(pair<string, HostInformation>(info.GetComponentName(), info));
		}		

		EndUnmarshal();

		return mapComponentHostInformation;
	}

	virtual void ClearServices()
	{
		BeginMarshal(LookupServerInterface::Method_ClearServices);
		EndMarshal();
	}
};

// -------------------------------------------------------
//				LookupServer Stub
//			- Receiver/consumer class
// -------------------------------------------------------
class DLL_STATE LookupServerStub : public RmiServerListener
{
public:
	LookupServerStub(const InterfaceHandle &handle, LookupServerInterface* serverInterface, bool autoStart)
		: RmiServerListener(handle, autoStart)
		, lookupServerInterface(serverInterface)
	{ }
	virtual ~LookupServerStub() {}

	//static LookupServerStub* GetStub(LookupServerInterface* lookupServerInterface, bool autoStart) 
    //{
     //   Stub* existingStub = FindStub(lookupServerInterface);
     //   return existingStub ? dynamic_cast<LookupServerStub*>(existingStub) : new LookupServerStub(AllocatePort(), lookupServerInterface, autoStart);
    //}

	//virtual string& GetInterfaceID() const
	//{
	//	return LookupServerInterface::InterfaceID;
	//}

	virtual void RMInvocation(int methodId, RmiBase* stubClient)
	{
		switch(methodId)
		{
			case LookupServerInterface::Method_ServiceRegistration:
			{
				HostInformation info;
				info.Read(stubClient->GetSocketReader());
				stubClient->EndUnmarshal();
				
				info = lookupServerInterface->ServiceRegistration(info);

				stubClient->BeginMarshal(LookupServerInterface::Method_ServiceRegistration);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case LookupServerInterface::Method_ServiceInformation:
			{	
				string name = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				HostInformation info = lookupServerInterface->GetServiceInformation(name);

				stubClient->BeginMarshal(LookupServerInterface::Method_ServiceInformation);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case LookupServerInterface::Method_AliveChecker:
			{
				HostInformation info;
				string componentName;

				info.Read(stubClient->GetSocketReader());
				componentName = stubClient->GetSocketReader()->ReadString();
				stubClient->EndUnmarshal();

				info = lookupServerInterface->AliveChecker(info, componentName);
				
				stubClient->BeginMarshal(LookupServerInterface::Method_AliveChecker);
				info.Write(stubClient->GetSocketWriter());
				stubClient->EndMarshal();
				break;
			}
			case LookupServerInterface::Method_ShutDown:
			{
				stubClient->EndUnmarshal();
				lookupServerInterface->ShutDown();
				break;
			}
			case LookupServerInterface::Method_AllServiceInformation:
			{
				stubClient->EndUnmarshal();
				
				map<string, HostInformation> mapNameHostInformation = lookupServerInterface->GetAllServiceInformation();

				stubClient->BeginMarshal(LookupServerInterface::Method_AllServiceInformation);

				SocketWriter *writer = stubClient->GetSocketWriter();
				writer->WriteInt32(mapNameHostInformation.size());
				
				for(map<string, HostInformation>::iterator it = mapNameHostInformation.begin(), it_end = mapNameHostInformation.end(); it != it_end; ++it)
				{
					HostInformation &info = it->second;
					info.Write(writer);
				}				
				stubClient->EndMarshal();				
				break;
			}

			case LookupServerInterface::Method_ClearServices:
			{
				stubClient->EndUnmarshal();
				lookupServerInterface->ClearServices();
				break;
			}
			default:
				RmiServerListener::RMInvocation(methodId, stubClient);
				break;
		}
	}
private:
	LookupServerInterface *lookupServerInterface;
};

// -------------------------------------------------------
//	LookupServer connections for stub and publisher
// -------------------------------------------------------
/**
* Container class for Stub and Publisher connection to the LookupService
* The class cannot be created directly, but rather must be subclassed.
*/
class DLL_STATE StubPublisherLookup : public Thread
{
private:
	LookupServerProxy	*lookupServerProxy;
	HostInformation		myHostInformation;
	InterfaceHandle		lookupServerHandle;
	Mutex				receivedHostInfoMutex;
	WaitCondition		receivedHostInfoCondition;
	bool				runThread;

protected:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Stub or Publisher HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	StubPublisherLookup(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true);
	
	friend class LookupServerConnectionManager;

	/**
	* Stops the connection to the LookupServer.
	*/
	virtual ~StubPublisherLookup(void);

protected:
	/**
	* Registers Stub or Publisher with the LookupServer.
	*
	* @return bool		true if successfully registered with LookupServer.
	*/
	virtual bool registerLookupServer(bool login);

	/**
	* Main loop that sends "I am alive" messages to LookupServer
	*/
	virtual void run();

	/**
	* Gets or creates a LookupServerProxy.
	*/
	LookupServerProxy* getOrCreateProxy(InterfaceHandle lookupServerHandle);

public:
	//inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(0, ""); }

	/**
	* Returns the HostInformation for the Stub or Publisher service.
	*
	* @return HostInformation		The HostInformation for the Stub or Publisher.
	*/
	HostInformation GetHostInformation(bool wait = false);

	/**
	* Deregisters from LookupServer, and shuts down the thread.
	*/
	inline void Shutdown()		{ runThread = false; }
};

/**
* Specialized class for Stub connections to the LookupService
*/
class DLL_STATE StubLookupService : public StubPublisherLookup
{
public:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Stub HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	StubLookupService(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true)
		: StubPublisherLookup(info, lookupHandle, autoStart)
	{}
};

/**
* Specialized class for Publisher connections to the LookupService
*/
class DLL_STATE PublisherLookupService : public StubPublisherLookup
{
public:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Publisher HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	PublisherLookupService(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart = true)
		: StubPublisherLookup(info, lookupHandle, autoStart)
	{}
};

// -------------------------------------------------------
//	LookupServer connections for proxy and subscriber
// -------------------------------------------------------
class ConnectToStubThread;
/**
* Container class for Proxy and Subscriber connection to the LookupService
* The class cannot be created directly, but rather must be subclassed.
*/
class DLL_STATE ProxySubscriberLookup : public Thread
{
private:
	Mutex				connectedHostInfoMutex;
	WaitCondition		connectedHostInfoCondition;
	HostInformation		myHostInformation;
	InterfaceHandle		lookupServerHandle;
	string				connectedComponentName;
	HostInformation		connectedHostInformation;
	int					currentStubTimeStamp;
	bool				runThread;

	RmiBase				*subscriberBase;
	RmiClient			*proxyBase;
	LookupServerProxy	*lookupServerProxy;
	ConnectToStubThread *reConnectToStubThread;

protected:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Proxy or Subscriber HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param connectedName	The Stub the Proxy is interested in, or the Publisher the Subscriber is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	ProxySubscriberLookup(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true);
	
	friend class LookupServerConnectionManager;

	/**
	* Stops the connection to the LookupServer.
	*/
	virtual ~ProxySubscriberLookup(void);

protected:
	/**
	* Registers Stub or Publisher with the LookupServer.
	*
	* @return bool		true if successfully registered with LookupServer.
	*/
	virtual bool registerLookupServer(bool login);

	/**
	* Main loop that sends "I am alive" messages to LookupServer
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
	* Gets or creates a LookupServerProxy.
	*/
	LookupServerProxy* getOrCreateProxy(InterfaceHandle lookupServerHandle);

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
	* @param wait					If wait is true, then the function waits until the LookupServer 
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
	* Deregisters from LookupServer, and shuts down the thread.
	*/
	inline void Shutdown()		{ runThread = false; }
};

/**
* Specialized class for Proxy connections to the LookupService
*/
class DLL_STATE ProxyLookupService : public ProxySubscriberLookup
{
public:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Proxy HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param connectedName	The Stub the Proxy is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	ProxyLookupService(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true)
		: ProxySubscriberLookup(info, lookupHandle, connectedName, autoStart)
	{}
};

/**
* Specialized class for Subscriber connections to the LookupService
*/
class DLL_STATE SubscriberLookupService : public ProxySubscriberLookup
{
public:
	/**
	* Creates and maintains a connection to the LookupServer.
	*
	* @param info			The Subscriber HostInformation.
	* @param lookupHandle	InterfacHandle (port, host) to the LookupServer.
	* @param connectedName	The Publisher the Subscriber is interested in.
	* @param autoStart		Decides whether the thread is automatically started.
	*/
	SubscriberLookupService(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart = true)
		: ProxySubscriberLookup(info, lookupHandle, connectedName, autoStart)
	{}
};


class DLL_STATE LookupServerConnectionManager
{
private:
	typedef set<Thread*>	SetComponent;
	static SetComponent		setComponent;
	static Mutex			guard;

public:
	LookupServerConnectionManager() {}
	~LookupServerConnectionManager() {}

	static bool AddConnection(Thread*);
	static bool RemoveConnection(Thread*);
	static void ReleaseAllConnections();
};

} // namespace end


#endif //LOOKUP_SERVER_BASE_H
