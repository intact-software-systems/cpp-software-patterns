#include "IntactMiddleware/ServiceManagerConnection.h"

#include<time.h>

#ifdef USE_GCC
#include<sys/time.h>
#else
#include<sys/timeb.h>
#endif

namespace IntactMiddleware 
{

// -------------------------------------------------------
//	ServiceManager connection used by Stub or Publisher
// -------------------------------------------------------
StubPublisherService::StubPublisherService(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart) //: Component(info.GetHostName())
	: serviceManagerProxy(NULL)
	, myHostInformation(info)
	, serviceManagerHandle(lookupHandle)
	, runThread(true)
{ 
	ServiceManagerConnectionManager::AddConnection(this);

	serviceManagerHandle.SetHostName(NetworkFunctions::getIPAddress(serviceManagerHandle.GetHostName()));
	myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));
	myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
	if(autoStart) start();
}

StubPublisherService::~StubPublisherService(void)
{ 
	ServiceManagerConnectionManager::RemoveConnection(this);

	if(runThread == true)
	{
		runThread = false;
		msleep(ALIVE_CHECK_INTERVAL_MS);
	}

	registerServiceManager(false);	

	if(serviceManagerProxy != NULL)
	{
		delete serviceManagerProxy;
		serviceManagerProxy = NULL;
	}
}

bool StubPublisherService::registerServiceManager(bool login)
{
	if(login == false)
	{
		try
		{
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);
			if(serviceManagerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
			myHostInformation = serviceManagerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " disconnected from ServiceManager " << endl;
			return true;
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.what() << endl;
		}
		catch(...)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception." << endl;
		}
		return false;
	}

	// -- login == true --
	for(int i = REGISTER_ATTEMPTS; i > 0; i--)
	{
		try
		{
			//serviceManagerProxy = ServiceManagerProxy::GetRmiClient(serviceManagerHandle, false);
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);
			if(serviceManagerProxy == NULL)
			{
				throw Exception("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation.SetTimeStamp(BaseLib::GetTimeStampInteger());
			myHostInformation = serviceManagerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " connected to ServiceManager " << endl;

			receivedHostInfoMutex.lock();
			receivedHostInfoCondition.wakeAll();
			receivedHostInfoMutex.unlock();
			return true;
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.msg() << ". Attempting again (" << i << ")" << endl;
			msleep(500);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Could not connect to ServiceManager. Attempting again (" << i << ")" << endl;
			cout << "." ;
			msleep(500);
		}
	}
	return false;
}

// Send out I am alive messages to ServiceManager
void StubPublisherService::run()
{	
	cout << "Connecting ServiceManager on host " << serviceManagerHandle.GetHostName() << " ..." ;
	while(!registerServiceManager(true) && runThread)
    {
        // empty wait loop
    }

	while(runThread)
	{
		try
		{
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);	
			if(serviceManagerProxy == NULL) 
			{
				throw IOException("Connection Error!");
			}

			while(runThread)
			{
				msleep(ALIVE_CHECK_INTERVAL_MS);
				if(!runThread) break;

				serviceManagerProxy->AliveChecker(myHostInformation, string(""));
			}
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.what() << ". Attempting Reconnect." << endl;
			msleep(1000);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Lost connection to ServiceManager. Attempting Reconnect." << endl;
			msleep(1000);
		}
	}
}

ServiceManagerProxy* StubPublisherService::getOrCreateProxy(InterfaceHandle serviceManagerHandle)
{
	bool createNewProxy = false;
	if(serviceManagerProxy != NULL) if(serviceManagerProxy->IsConnected() == false) createNewProxy = true;

	if(serviceManagerProxy == NULL || createNewProxy)
	{
		try
		{
			if(serviceManagerProxy != NULL) delete serviceManagerProxy;
			
			serviceManagerProxy = ServiceManagerProxy::GetRmiClient(serviceManagerHandle);
		}
		catch(IOException io)
		{
			serviceManagerProxy = NULL;
			throw;
		}
		catch(...)
		{
			serviceManagerProxy = NULL;
			throw;
		}
	}
	return serviceManagerProxy;
}

HostInformation StubPublisherService::GetHostInformation(bool wait)
{
	receivedHostInfoMutex.lock();
	if(wait == true && myHostInformation.GetOnlineStatus() == HostInformation::OFFLINE)
		receivedHostInfoCondition.wait(&receivedHostInfoMutex);
	
	HostInformation info = myHostInformation;
	receivedHostInfoMutex.unlock();

	return info; 
}

// -------------------------------------------------------
//				Connect to Stub Thread
// -------------------------------------------------------
/**
* Private class, which is used by class ProxySubscriberService to reconnect to a Proxy
*/
class ConnectToStubThread : public Thread
{
public:
	ConnectToStubThread(RmiClient *p, InterfaceHandle handle) : proxyBase(p), stubHandle(handle), stopThread(false)
	{
		start();
	}
	~ConnectToStubThread()
	{
		Stop();
		Notify();
	}

	inline virtual void run()
	{
		while(!stopThread)
		{
			Connect();
			
			guard.lock();
				if(stopThread == true) 
				{
					guard.unlock();
					break;
				}
			guard.unlock();
			
			connectMutex.lock();
			connectCondition.wait(&connectMutex);
			connectMutex.unlock();
		}
	}

	inline void Connect()
	{
		//proxyBase->lock();

		while(true) //proxyBase->IsConnected() == false)
		{
			guard.lock();

			if(stopThread == true) 
			{
				guard.unlock();
				break;
			}

			try
			{
				proxyBase->StartOrRestartClient(stubHandle); 
				proxyBase->Initialize();

				//if(proxyBase->IsConnected()) 
				cout << "Connected to : " << stubHandle << endl;

				guard.unlock();
			}
			catch(...)
			{
				guard.unlock();
				msleep(1000);
			}
		}
		
		//proxyBase->unlock();
	}

	inline void UpdateInterfaceHandle(InterfaceHandle handle) 
	{ 
		if(handle.GetPortNumber() == stubHandle.GetPortNumber() && handle.GetHostName() == stubHandle.GetHostName()) return;

		guard.lock();
		stubHandle = handle; 
		guard.unlock();

		this->Notify();
	}

	inline void Notify()
	{
		connectMutex.lock();
		connectCondition.wakeAll();
		connectMutex.unlock();
	}

	inline void Stop()		{ MutexLocker lock(&guard); stopThread = true; }

private:
	RmiClient		*proxyBase;
	InterfaceHandle stubHandle;
	bool			stopThread;
	Mutex			guard;
	Mutex			connectMutex;
	WaitCondition	connectCondition;
};

// -------------------------------------------------------
//	ServiceManager connection used by Proxy or Subscriber
// -------------------------------------------------------
ProxySubscriberService::ProxySubscriberService(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart) 
	//: Component(info.GetHostName())
	: subscriberBase(NULL)
	, proxyBase(NULL)
	, serviceManagerProxy(NULL)
	, reConnectToStubThread(NULL)
	, myHostInformation(info)
	, serviceManagerHandle(lookupHandle)
	, connectedComponentName(connectedName)
	, connectedHostInformation()
	, currentStubTimeStamp(0)
	, runThread(true)
{ 
	ServiceManagerConnectionManager::AddConnection(this);

	serviceManagerHandle.SetHostName(NetworkFunctions::getIPAddress(serviceManagerHandle.GetHostName()));
	myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));
	if(autoStart) start();
}

ProxySubscriberService::~ProxySubscriberService(void)
{ 
	ServiceManagerConnectionManager::RemoveConnection(this);

	if(runThread == true)
	{
		runThread = false;
		msleep(ALIVE_CHECK_INTERVAL_MS);
	}

	registerServiceManager(false);
	
	if(reConnectToStubThread)
	{
		reConnectToStubThread->Stop();
		reConnectToStubThread->Notify();
		delete reConnectToStubThread;
		reConnectToStubThread = NULL;
	}

	if(serviceManagerProxy != NULL)
	{
		delete serviceManagerProxy;
		serviceManagerProxy = NULL;
	}
}

bool ProxySubscriberService::registerServiceManager(bool login)
{
	if(login == false)
	{
		try
		{
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);
			if(serviceManagerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
			myHostInformation = serviceManagerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " disconnected from ServiceManager " << endl;
			return true;
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.what() << endl;
		}
		catch(...)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception." << endl;
		}
		return false;
	}

	for(int i = REGISTER_ATTEMPTS; i > 0; i--)
	{
		try
		{
			//serviceManagerProxy = ServiceManagerProxy::GetRmiClient(serviceManagerHandle, false);
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);
			if(serviceManagerProxy == NULL)
			{
				throw IOException("Connect exception!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation.SetTimeStamp(BaseLib::GetTimeStampInteger());
			myHostInformation = serviceManagerProxy->ServiceRegistration(myHostInformation);

			connectedHostInformation = serviceManagerProxy->GetServiceInformation(connectedComponentName);
			cout << endl << myHostInformation.GetComponentName() << ": Received host info: " << connectedHostInformation << endl;
			
			if(connectedHostInformation.GetOnlineStatus() == HostInformation::ONLINE)
			{
				currentStubTimeStamp = connectedHostInformation.GetTimeStamp();

				connectedHostInfoMutex.lock();
				connectedHostInfoCondition.wakeAll();
				connectedHostInfoMutex.unlock();
			}
			return true;
			break;
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.msg() << ". Attempting again (" << i << ")" << endl;
			msleep(500);
		}
		catch(...)
		{
			cout << "." ;
			msleep(500);
		}
	}

	return false;
}

// Send out I am alive messages to ServiceManager
void ProxySubscriberService::run()
{	
	cout << "Connecting ServiceManager on host " << serviceManagerHandle.GetHostName() << " ..." ;
	while(!registerServiceManager(true))
    {
        // empty wait loop
    }

	while(true)
	{
		try
		{
			serviceManagerProxy = getOrCreateProxy(serviceManagerHandle);
			if(serviceManagerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			while(true)
			{
				msleep(ALIVE_CHECK_INTERVAL_MS);
				
				HostInformation componentInfo = serviceManagerProxy->AliveChecker(myHostInformation, connectedComponentName);
				
				if(connectedComponentName == "") continue;

				// -- Is stub/publisher OFFLINE?
				if(componentInfo.GetOnlineStatus() == HostInformation::OFFLINE)
				{
					if(connectedHostInformation.GetOnlineStatus() == HostInformation::ONLINE)
						cout << myHostInformation.GetComponentName() << ": " << connectedComponentName << " is " << componentInfo.GetOnlineStatus() << " current timestamp " << componentInfo.GetTimeStamp() << endl;
					
					//if(currentStubTimeStamp != componentInfo.GetTimeStamp())
					//	cout << componentInfo.GetComponentName() << " Timestamp changed although " << componentInfo.GetOnlineStatus() << " ERROR! in ServiceManager! " << endl;

					// -> save latest component information anyway
					updateConnectedHostInformation(componentInfo);
					continue;
				}

				// -- Am I a SUBSCRIBER?
				if(myHostInformation.GetHostDescription() == HostInformation::SUBSCRIBER)
				{
					// -- Has PUBLISHER been restarted with different multicast address?
					if(connectedHostInformation.GetMulticastAddress() != componentInfo.GetMulticastAddress() ||
						connectedHostInformation.GetPortNumber() != componentInfo.GetPortNumber())
					{
						if(subscriberBase)
						{
							// -- start debug --
							stringstream stream;
							stream << myHostInformation.GetComponentName() << ": Received new host info: " << componentInfo << endl;
							cout << stream.str();
							ExceptionMulticast::PostException(stream.str(), MiddlewareException::GENERAL_WARNING);
							// -- end debug --
							ASSERT(0);
							//subscriberBase->ReStart(true, InterfaceHandle(componentInfo.GetPortNumber(), componentInfo.GetMulticastAddress()));
						}
						else
						{
							// -- start debug --
							stringstream stream;
							stream << myHostInformation.GetComponentName() << ": " << connectedComponentName << " has been restarted! Unable to reconnect!" << endl;
							cout << stream.str();
							ExceptionMulticast::PostException(stream.str(), MiddlewareException::GENERAL_WARNING);
							// -- end debug --
						}

					}
				}
				// -- Am I a PROXY?
				else if(myHostInformation.GetHostDescription() == HostInformation::PROXY)
				{
					// -- Has STUB been restarted?
					if( (proxyBase != NULL) && 
						(componentInfo.GetTimeStamp() > 0) && 
						(currentStubTimeStamp != componentInfo.GetTimeStamp()))
					{
						// -- debug start --
						stringstream stream;
						stream << myHostInformation.GetComponentName() << ": " << connectedComponentName << " was disconnected! Reconnecting using current host info: " << componentInfo << endl;
						cout << stream.str();
						ExceptionMulticast::PostException(stream.str(), MiddlewareException::GENERAL_WARNING);
						// -- debug end --

						if(!reConnectToStubThread)
						{
							reConnectToStubThread = new ConnectToStubThread(proxyBase, InterfaceHandle(componentInfo.GetPortNumber(), componentInfo.GetHostName()));
						}
						else 
						{
							reConnectToStubThread->UpdateInterfaceHandle(InterfaceHandle(componentInfo.GetPortNumber(), componentInfo.GetHostName()));
							reConnectToStubThread->Notify();
						}
					}
				}
			
				// -- save latest component information --
				updateConnectedHostInformation(componentInfo, true);
			}
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": ServiceManager connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
			msleep(1000);
		}
		catch(...)
		{
			msleep(1000);
		}
	}
}

ServiceManagerProxy* ProxySubscriberService::getOrCreateProxy(InterfaceHandle serviceManagerHandle)
{
	bool createNewProxy = false;
	if(serviceManagerProxy != NULL) if(serviceManagerProxy->IsConnected() == false) createNewProxy = true;

	if(serviceManagerProxy == NULL || createNewProxy)
	{
		try
		{
			if(serviceManagerProxy != NULL) delete serviceManagerProxy;
			
			serviceManagerProxy = ServiceManagerProxy::GetRmiClient(serviceManagerHandle);
		}
		catch(IOException io)
		{
			serviceManagerProxy = NULL;
			throw;
		}
		catch(...)
		{
			serviceManagerProxy = NULL;
			throw;
		}
	}
	return serviceManagerProxy;
}

set<Thread*>	ServiceManagerConnectionManager::setComponent;
Mutex			ServiceManagerConnectionManager::guard;

bool ServiceManagerConnectionManager::AddConnection(Thread *component)
{
	MutexLocker lock(&guard);
	if(ServiceManagerConnectionManager::setComponent.count(component) > 0)
	{
		IDEBUG() << "Already added component " ; // component->Name ;
		return false;
	}

	ServiceManagerConnectionManager::setComponent.insert(component);
	return true;
}

bool ServiceManagerConnectionManager::RemoveConnection(Thread *component)
{
	MutexLocker lock(&guard);
	if(ServiceManagerConnectionManager::setComponent.count(component) <= 0)
	{
		IDEBUG() << "Component not found: " ; // component->Name ;
		return false;
	}

	ServiceManagerConnectionManager::setComponent.erase(component);
	return true;
}

void ServiceManagerConnectionManager::ReleaseAllConnections()
{
	guard.lock();

	SetComponent localSet = ServiceManagerConnectionManager::setComponent;

	guard.unlock();

	for(SetComponent::iterator it = localSet.begin(), it_end = localSet.end(); it != it_end; ++it)
	{
		Thread *component = *it;
		if(component == NULL) continue;
		
		ProxySubscriberService *proxyService = dynamic_cast<ProxySubscriberService*>(component);
		if(proxyService)
		{
			proxyService->Shutdown();
			proxyService->registerServiceManager(false);
			continue;
		}

		StubPublisherService *stubService = dynamic_cast<StubPublisherService*>(component);
		if(stubService)
		{
			stubService->Shutdown();
			stubService->registerServiceManager(false);
			continue;
		}
		IDEBUG() << "WARNING! Component not recognized " ; // component->Name ;
	}
}



// -------------------------------------------------------
//			namespace ServiceManagerBaseSpace
// -------------------------------------------------------
namespace ServiceManagerBaseSpace
{
// -------------------------------------------------------
//			register service in ServiceManager
// -------------------------------------------------------
StubServiceManager* RegisterStubServiceToServiceManager(InterfaceHandle serviceManager, HostInformation myHostInformation)
{
	return new StubServiceManager(myHostInformation, serviceManager, true);
}

// -------------------------------------------------------
//			register service in ServiceManager
// -------------------------------------------------------
PublisherServiceManager* RegisterPublisherServiceToServiceManager(InterfaceHandle serviceManager, HostInformation myHostInformation)
{
	return new PublisherServiceManager(myHostInformation, serviceManager, true);
}

// -------------------------------------------------------
//			create stub connection
// -------------------------------------------------------
StubServiceManager* CreateStubConnection(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string myComponentName, 
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::STUB);

	return new StubServiceManager(myHostInformation, serviceManager, autoStart);
}

StubServiceManager* CreateStubConnection(InterfaceHandle serviceManager, 
										  string myComponentName, 
										  int myPortNumber, 
										  bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myPortNumber, HostInformation::STUB);
	
	return new StubServiceManager(myHostInformation, serviceManager, autoStart);
}

// -------------------------------------------------------
//			create publisher connection
// -------------------------------------------------------
PublisherServiceManager* CreatePublisherConnection(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::PUBLISHER);

	return new PublisherServiceManager(myHostInformation, serviceManager, autoStart);
}

PublisherServiceManager* CreatePublisherConnection(InterfaceHandle serviceManager, 
											   string myComponentName, 
											   string myMulticastAddress, 
											   int myPortNumber, 
											   bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myMulticastAddress, myPortNumber, HostInformation::PUBLISHER);
	
	return new PublisherServiceManager(myHostInformation, serviceManager, autoStart);
}

// -------------------------------------------------------
//			create proxy connection
// -------------------------------------------------------
ProxyServiceManager* CreateProxyConnection(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  string connectingToComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::PROXY);

	return new ProxyServiceManager(myHostInformation, serviceManager, connectingToComponentName, autoStart);
}

ProxyServiceManager* CreateProxyConnection(InterfaceHandle serviceManager, 
											 string myComponentName, 
											 int myPortNumber, 
											 string connectingToComponentName, 
											 bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myPortNumber, HostInformation::PROXY);

	return new ProxyServiceManager(myHostInformation, serviceManager, connectingToComponentName, autoStart);
}

ProxyServiceManager* GetAndMonitorStubServiceInformation(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string serviceInServiceManager,
										  HostInformation &serviceInformation)
{
	myHostInformation.SetHostDescription(HostInformation::PROXY);

	ProxyServiceManager *lookup = new ProxyServiceManager(myHostInformation, serviceManager, serviceInServiceManager, true);
	serviceInformation = lookup->GetConnectedHostInformation(true);
	return lookup;	
}

// -------------------------------------------------------
//			create subscriber connection
// -------------------------------------------------------
SubscriberServiceManager* CreateSubscriberConnection(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  string connectingToComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::SUBSCRIBER);

	return new SubscriberServiceManager(myHostInformation, serviceManager, connectingToComponentName, autoStart);
}

SubscriberServiceManager* CreateSubscriberConnection(InterfaceHandle serviceManager, 
												  string myComponentName, 
												  int myPortNumber, 
												  string connectingToComponentName, 
												  bool autoStart) 
{
	HostInformation myHostInformation(myComponentName, MicroMiddleware::MiddlewareSettings::GetHostName(), myPortNumber, HostInformation::SUBSCRIBER);

	return new SubscriberServiceManager(myHostInformation, serviceManager, connectingToComponentName, autoStart);
}

SubscriberServiceManager* GetAndMonitorPublisherServiceInformation(InterfaceHandle serviceManager, 
										  HostInformation myHostInformation, 
										  string serviceInServiceManager, 
										  HostInformation &serviceInformation)
{
	myHostInformation.SetHostDescription(HostInformation::SUBSCRIBER);

	SubscriberServiceManager *lookup = new SubscriberServiceManager(myHostInformation, serviceManager, serviceInServiceManager, true);
	serviceInformation = lookup->GetConnectedHostInformation(true);
	return lookup;
}
// -------------------------------------------------------
//			get a valid host information service.
// -------------------------------------------------------

InterfaceHandle GetHandleFromServiceManager(InterfaceHandle& serviceManager, string componentName)
{
	cout << "GetHandleFromServiceManager(serviceManager, componentName): Function is being phased out. Please use alternative functions!" ;
	ASSERT(0);

	return InterfaceHandle(); //providerInfo.GetPortNumber(), providerInfo.GetHostName());
}

} // namespace ServiceManagerBaseSpace


} // namespace IntactMiddleware

