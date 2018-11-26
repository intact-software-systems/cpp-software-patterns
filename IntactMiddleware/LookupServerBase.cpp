#include "IntactMiddleware/LookupServerBase.h"

#include<time.h>

#ifdef USE_GCC
#include<sys/time.h>
#else
#include<sys/timeb.h>
#endif

using namespace std;

namespace IntactMiddleware 
{
// -------------------------------------------------------
//			namespace LookupServerBaseSpace
// -------------------------------------------------------
namespace LookupServerBaseSpace
{
// -------------------------------------------------------
//			register service in LookupServer
// -------------------------------------------------------
StubLookupService* RegisterStubServiceToLookupServer(InterfaceHandle lookupServer, HostInformation myHostInformation)
{
	return new StubLookupService(myHostInformation, lookupServer, true);
}

// -------------------------------------------------------
//			register service in LookupServer
// -------------------------------------------------------
PublisherLookupService* RegisterPublisherServiceToLookupServer(InterfaceHandle lookupServer, HostInformation myHostInformation)
{
	return new PublisherLookupService(myHostInformation, lookupServer, true);
}

// -------------------------------------------------------
//			create stub connection
// -------------------------------------------------------
StubLookupService* CreateStubConnection(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string myComponentName, 
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::STUB);

	return new StubLookupService(myHostInformation, lookupServer, autoStart);
}

StubLookupService* CreateStubConnection(InterfaceHandle lookupServer, 
										  string myComponentName, 
										  int myPortNumber, 
										  bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myPortNumber, HostInformation::STUB);
	
	return new StubLookupService(myHostInformation, lookupServer, autoStart);
}

// -------------------------------------------------------
//			create publisher connection
// -------------------------------------------------------
PublisherLookupService* CreatePublisherConnection(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::PUBLISHER);

	return new PublisherLookupService(myHostInformation, lookupServer, autoStart);
}

PublisherLookupService* CreatePublisherConnection(InterfaceHandle lookupServer, 
											   string myComponentName, 
											   string myMulticastAddress, 
											   int myPortNumber, 
											   bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myMulticastAddress, myPortNumber, HostInformation::PUBLISHER);
	
	return new PublisherLookupService(myHostInformation, lookupServer, autoStart);
}

// -------------------------------------------------------
//			create proxy connection
// -------------------------------------------------------
ProxyLookupService* CreateProxyConnection(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  string connectingToComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::PROXY);

	return new ProxyLookupService(myHostInformation, lookupServer, connectingToComponentName, autoStart);
}

ProxyLookupService* CreateProxyConnection(InterfaceHandle lookupServer, 
											 string myComponentName, 
											 int myPortNumber, 
											 string connectingToComponentName, 
											 bool autoStart) 
{
	string hostname = Runtime::GetInterfaceHandle().GetHostName();
	if(hostname.empty()) hostname = TcpServer::GetLocalHostName();

	HostInformation myHostInformation(myComponentName, hostname, myPortNumber, HostInformation::PROXY);

	return new ProxyLookupService(myHostInformation, lookupServer, connectingToComponentName, autoStart);
}

ProxyLookupService* GetAndMonitorStubServiceInformation(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string serviceInLookupServer,
										  HostInformation &serviceInformation)
{
	myHostInformation.SetHostDescription(HostInformation::PROXY);

	ProxyLookupService *lookup = new ProxyLookupService(myHostInformation, lookupServer, serviceInLookupServer, true);
	serviceInformation = lookup->GetConnectedHostInformation(true);
	return lookup;	
}

// -------------------------------------------------------
//			create subscriber connection
// -------------------------------------------------------
SubscriberLookupService* CreateSubscriberConnection(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string myComponentName,
										  string connectingToComponentName,
										  bool autoStart)
{
	myHostInformation.SetComponentName(myComponentName);
	myHostInformation.SetHostDescription(HostInformation::SUBSCRIBER);

	return new SubscriberLookupService(myHostInformation, lookupServer, connectingToComponentName, autoStart);
}

SubscriberLookupService* CreateSubscriberConnection(InterfaceHandle lookupServer, 
												  string myComponentName, 
												  int myPortNumber, 
												  string connectingToComponentName, 
												  bool autoStart) 
{
	HostInformation myHostInformation(myComponentName, MicroMiddleware::MiddlewareSettings::GetHostName(), myPortNumber, HostInformation::SUBSCRIBER);

	return new SubscriberLookupService(myHostInformation, lookupServer, connectingToComponentName, autoStart);
}

SubscriberLookupService* GetAndMonitorPublisherServiceInformation(InterfaceHandle lookupServer, 
										  HostInformation myHostInformation, 
										  string serviceInLookupServer, 
										  HostInformation &serviceInformation)
{
	myHostInformation.SetHostDescription(HostInformation::SUBSCRIBER);

	SubscriberLookupService *lookup = new SubscriberLookupService(myHostInformation, lookupServer, serviceInLookupServer, true);
	serviceInformation = lookup->GetConnectedHostInformation(true);
	return lookup;
}
// -------------------------------------------------------
//			get a valid host information service.
// -------------------------------------------------------

InterfaceHandle GetHandleFromLookupServer(InterfaceHandle& lookupServer, string componentName)
{
	cout << "GetHandleFromLookupServer(lookupServer, componentName): Function is being phased out. Please use alternative functions!" << endl;
	ASSERT(0);

	return InterfaceHandle(); //providerInfo.GetPortNumber(), providerInfo.GetHostName());
}

} // namespace LookupServerBaseSpace

// -------------------------------------------------------
//	LookupServer connection used by Stub or Publisher
// -------------------------------------------------------
StubPublisherLookup::StubPublisherLookup(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart) //: Component(info.GetHostName())
	: lookupServerProxy(NULL)
	, myHostInformation(info)
	, lookupServerHandle(lookupHandle)
	, runThread(true)
{ 
	LookupServerConnectionManager::AddConnection(this);

	lookupServerHandle.SetHostName(NetworkFunctions::getIPAddress(lookupServerHandle.GetHostName()));
	myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));
	myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
	if(autoStart) start();
}

StubPublisherLookup::~StubPublisherLookup(void)
{ 
	LookupServerConnectionManager::RemoveConnection(this);

	if(runThread == true)
	{
		runThread = false;
		sleep(ALIVE_CHECK_INTERVAL_MS);
	}

	registerLookupServer(false);	

	if(lookupServerProxy != NULL)
	{
		delete lookupServerProxy;
		lookupServerProxy = NULL;
	}
}

bool StubPublisherLookup::registerLookupServer(bool login)
{
	if(login == false)
	{
		try
		{
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);
			if(lookupServerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
			myHostInformation = lookupServerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " disconnected from LookupServer " << endl;
			return true;
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.what() << endl;
		}
		catch(...)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception." << endl;
		}
		return false;
	}

	// -- login == true --
	for(int i = REGISTER_ATTEMPTS; i > 0; i--)
	{
		try
		{
			//lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle, false);
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);
			if(lookupServerProxy == NULL)
			{
				throw Exception("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation.SetTimeStamp(BaseLib::GetTimeStampInteger());
			myHostInformation = lookupServerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " connected to LookupServer " << endl;

			receivedHostInfoMutex.lock();
			receivedHostInfoCondition.wakeAll();
			receivedHostInfoMutex.unlock();
			return true;
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.msg() << ". Attempting again (" << i << ")" << endl;
			sleep(500);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Could not connect to LookupServer. Attempting again (" << i << ")" << endl;
			cout << "." ;
			sleep(500);
		}
	}
	return false;
}

// Send out I am alive messages to LookupServer
void StubPublisherLookup::run()
{	
	cout << "Connecting LookupServer on host " << lookupServerHandle.GetHostName() << " ..." ;
	while(!registerLookupServer(true) && runThread)
	{
        // empty wait loop
    }

	while(runThread)
	{
		try
		{
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);	
			if(lookupServerProxy == NULL) 
			{
				throw IOException("Connection Error!");
			}

			while(runThread)
			{
				sleep(ALIVE_CHECK_INTERVAL_MS);
				if(!runThread) break;

				lookupServerProxy->AliveChecker(myHostInformation, string(""));
			}
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.what() << ". Attempting Reconnect." << endl;
			sleep(1000);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Lost connection to LookupServer. Attempting Reconnect." << endl;
			sleep(1000);
		}
	}
}

LookupServerProxy* StubPublisherLookup::getOrCreateProxy(InterfaceHandle lookupServerHandle)
{
	bool createNewProxy = false;
	if(lookupServerProxy != NULL) if(lookupServerProxy->IsConnected() == false) createNewProxy = true;

	if(lookupServerProxy == NULL || createNewProxy)
	{
		try
		{
			if(lookupServerProxy != NULL) delete lookupServerProxy;
			
			lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle);
		}
		catch(IOException io)
		{
			lookupServerProxy = NULL;
			throw;
		}
		catch(...)
		{
			lookupServerProxy = NULL;
			throw;
		}
	}
	return lookupServerProxy;
}

HostInformation StubPublisherLookup::GetHostInformation(bool wait)
{
	receivedHostInfoMutex.lock();
	if(wait == true && myHostInformation.GetOnlineStatus() == HostInformation::OFFLINE)
		receivedHostInfoCondition.wait(&receivedHostInfoMutex);
	
	HostInformation info = myHostInformation;
	receivedHostInfoMutex.unlock();

	return info; 
}

// -------------------------------------------------------
//	LookupServer connection used by Stub or Publisher
// -------------------------------------------------------
/*StubPublisherLookup::StubPublisherLookup(HostInformation &info, InterfaceHandle lookupHandle, bool autoStart) :
	lookupServerProxy(NULL)
	, myHostInformation(info)
	, lookupServerHandle(lookupHandle)
{ 
	lookupServerHandle.SetHostName(HostInformationSpace::GetIPAddress(lookupServerHandle.GetHostName()));
	myHostInformation.SetHostIP(HostInformationSpace::GetIPAddress(myHostInformation.GetHostName()));
	myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
	if(autoStart) start();
}
StubPublisherLookup::~StubPublisherLookup(void)
{ 
	registerLookupServer(false);
}

bool StubPublisherLookup::registerLookupServer(bool login)
{
	if(login == false)
	{
		myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
		lookupServerProxy->ServiceRegistration(myHostInformation);
		return true;
	}

	for(int i = REGISTER_ATTEMPTS; i > 0; i--)
	{
		try
		{
			lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle, false);
			
			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation.SetTimeStamp(HostInformationSpace::GetTimeStamp());
			myHostInformation = lookupServerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " connected to LookupServer " << endl;

			receivedHostInfoMutex.lock();
			receivedHostInfoCondition.wakeAll();
			receivedHostInfoMutex.unlock();
			return true;
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.msg() << ". Attempting again (" << i << ")" << endl;
			Thread::sleep(500);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Could not connect to LookupServer. Attempting again (" << i << ")" << endl;
			cout << "." ;
			Thread::sleep(500);
		}
	}

	return false;
}

// Send out I am alive messages to LookupServer
void StubPublisherLookup::run()
{	
	cout << "Connecting LookupServer on host " << lookupServerHandle.GetHostName() << " ..." ;
	while(!registerLookupServer(true));

	while(true)
	{
		try
		{
			lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle, false);
			if(lookupServerProxy == NULL) 
			{
				Thread::sleep(1000);
				continue;
			}

			while(true)
			{
				Thread::sleep(ALIVE_CHECK_INTERVAL_MS);
				lookupServerProxy->AliveChecker(myHostInformation, string(""));
			}
		}
		catch(Exception io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
			Thread::sleep(1000);
		}
		catch(...)
		{
			//cout << myHostInformation.GetComponentName() << ": Lost connection to LookupServer. Attempting Reconnect." << endl;
			Thread::sleep(1000);
		}
	}
}

HostInformation StubPublisherLookup::GetHostInformation(bool wait)
{
	receivedHostInfoMutex.lock();
	if(wait == true && myHostInformation.GetOnlineStatus() == HostInformation::OFFLINE)
		receivedHostInfoCondition.wait(&receivedHostInfoMutex);
	
	HostInformation info = myHostInformation;
	receivedHostInfoMutex.unlock();

	return info; 
}
*/


// -------------------------------------------------------
//				Connect to Stub Thread
// -------------------------------------------------------
/**
* Private class, which is used by class ProxySubscriberLookup to reconnect to a Proxy
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
				sleep(1000);
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
//	LookupServer connection used by Proxy or Subscriber
// -------------------------------------------------------
ProxySubscriberLookup::ProxySubscriberLookup(HostInformation &info, InterfaceHandle lookupHandle, string connectedName, bool autoStart) 
	//: Component(info.GetHostName())
	: myHostInformation(info)
	, lookupServerHandle(lookupHandle)
	, connectedComponentName(connectedName)
	, connectedHostInformation()
	, currentStubTimeStamp(0)
	, runThread(true)
	, subscriberBase(NULL)
	, proxyBase(NULL)
	, lookupServerProxy(NULL)
	, reConnectToStubThread(NULL)
{ 
	LookupServerConnectionManager::AddConnection(this);

	lookupServerHandle.SetHostName(NetworkFunctions::getIPAddress(lookupServerHandle.GetHostName()));
	myHostInformation.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation.GetHostName()));
	if(autoStart) start();
}

ProxySubscriberLookup::~ProxySubscriberLookup(void)
{ 
	LookupServerConnectionManager::RemoveConnection(this);

	if(runThread == true)
	{
		runThread = false;
		sleep(ALIVE_CHECK_INTERVAL_MS);
	}

	registerLookupServer(false);
	
	if(reConnectToStubThread)
	{
		reConnectToStubThread->Stop();
		reConnectToStubThread->Notify();
		delete reConnectToStubThread;
		reConnectToStubThread = NULL;
	}

	if(lookupServerProxy != NULL)
	{
		delete lookupServerProxy;
		lookupServerProxy = NULL;
	}
}

bool ProxySubscriberLookup::registerLookupServer(bool login)
{
	if(login == false)
	{
		try
		{
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);
			if(lookupServerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
			myHostInformation = lookupServerProxy->ServiceRegistration(myHostInformation);
			cout << endl << myHostInformation.GetComponentName() << " disconnected from LookupServer " << endl;
			return true;
		}
		catch(IOException io)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.what() << endl;
		}
		catch(...)
		{
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception." << endl;
		}
		return false;
	}

	for(int i = REGISTER_ATTEMPTS; i > 0; i--)
	{
		try
		{
			//lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle, false);
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);
			if(lookupServerProxy == NULL)
			{
				throw IOException("Connect exception!");
			}

			myHostInformation.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation.SetTimeStamp(BaseLib::GetTimeStampInteger());
			myHostInformation = lookupServerProxy->ServiceRegistration(myHostInformation);

			connectedHostInformation = lookupServerProxy->GetServiceInformation(connectedComponentName);
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
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.msg() << ". Attempting again (" << i << ")" << endl;
			sleep(500);
		}
		catch(...)
		{
			cout << "." ;
			sleep(500);
		}
	}

	return false;
}

// Send out I am alive messages to LookupServer
void ProxySubscriberLookup::run()
{	
	cout << "Connecting LookupServer on host " << lookupServerHandle.GetHostName() << " ..." ;
	while(!registerLookupServer(true))
    {
        // empty wait loop
    }

	while(true)
	{
		try
		{
			lookupServerProxy = getOrCreateProxy(lookupServerHandle);
			if(lookupServerProxy == NULL)
			{
				throw IOException("Connection Error!");
			}

			while(true)
			{
				sleep(ALIVE_CHECK_INTERVAL_MS);
				
				HostInformation componentInfo = lookupServerProxy->AliveChecker(myHostInformation, connectedComponentName);
				
				if(connectedComponentName == "") continue;

				// -- Is stub/publisher OFFLINE?
				if(componentInfo.GetOnlineStatus() == HostInformation::OFFLINE)
				{
					if(connectedHostInformation.GetOnlineStatus() == HostInformation::ONLINE)
						cout << myHostInformation.GetComponentName() << ": " << connectedComponentName << " is " << componentInfo.GetOnlineStatus() << " current timestamp " << componentInfo.GetTimeStamp() << endl;
					
					//if(currentStubTimeStamp != componentInfo.GetTimeStamp())
					//	cout << componentInfo.GetComponentName() << " Timestamp changed although " << componentInfo.GetOnlineStatus() << " ERROR! in LookupServer! " << endl;

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
			cout << myHostInformation.GetComponentName() << ": LookupServer connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
			sleep(1000);
		}
		catch(...)
		{
			sleep(1000);
		}
	}
}

LookupServerProxy* ProxySubscriberLookup::getOrCreateProxy(InterfaceHandle lookupServerHandle)
{
	bool createNewProxy = false;
	if(lookupServerProxy != NULL) if(lookupServerProxy->IsConnected() == false) createNewProxy = true;

	if(lookupServerProxy == NULL || createNewProxy)
	{
		try
		{
			if(lookupServerProxy != NULL) delete lookupServerProxy;
			
			lookupServerProxy = LookupServerProxy::GetRmiClient(lookupServerHandle);
		}
		catch(IOException io)
		{
			lookupServerProxy = NULL;
			throw;
		}
		catch(...)
		{
			lookupServerProxy = NULL;
			throw;
		}
	}
	return lookupServerProxy;
}

LookupServerConnectionManager::SetComponent	LookupServerConnectionManager::setComponent;
Mutex			LookupServerConnectionManager::guard;

bool LookupServerConnectionManager::AddConnection(Thread *component)
{
	MutexLocker lock(&guard);
	if(LookupServerConnectionManager::setComponent.count(component) > 0)
	{
		IDEBUG() << "Already added component " ; // component->Name ;
		return false;
	}

	LookupServerConnectionManager::setComponent.insert(component);
	return true;
}

bool LookupServerConnectionManager::RemoveConnection(Thread *component)
{
	MutexLocker lock(&guard);
	if(LookupServerConnectionManager::setComponent.count(component) <= 0)
	{
		IDEBUG() << "Component not found: " ; // component->Name ;
		return false;
	}

	LookupServerConnectionManager::setComponent.erase(component);
	return true;
}

void LookupServerConnectionManager::ReleaseAllConnections()
{
	guard.lock();

	SetComponent localSet = LookupServerConnectionManager::setComponent;

	guard.unlock();

	for(SetComponent::iterator it = localSet.begin(), it_end = localSet.end(); it != it_end; ++it)
	{
		Thread *component = *it;
		if(component == NULL) continue;
		
		ProxySubscriberLookup *proxyLookup = dynamic_cast<ProxySubscriberLookup*>(component);
		if(proxyLookup)
		{
			proxyLookup->Shutdown();
			proxyLookup->registerLookupServer(false);
			continue;
		}

		StubPublisherLookup *stubLookup = dynamic_cast<StubPublisherLookup*>(component);
		if(stubLookup)
		{
			stubLookup->Shutdown();
			stubLookup->registerLookupServer(false);
			continue;
		}
		IDEBUG() << "WARNING! Component not recognized " ; // component->Name ;
	}
}



} // namespace end


