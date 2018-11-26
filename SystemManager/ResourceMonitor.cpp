#include "SystemManager/ResourceMonitor.h"
#include "SystemManager/SystemManager.h"

namespace SystemManagerSpace
{
// -------------------------------------------------------
//	constructor/destructor MonitorFactoryResources
// -------------------------------------------------------
MonitorFactoryResources::MonitorFactoryResources(MapProcessFactoryProxy &mapFactoryProxy, Mutex &smMutex, int interval, bool autoStart)
	: Thread(), 
	monitorInterval(interval),
	monitorResources(true),
	mapHostProcessFactoryProxy(mapFactoryProxy),
	smMutexProxyGuard(smMutex)

{
	if(autoStart == true) start();
}

MonitorFactoryResources::~MonitorFactoryResources()
{
}

void MonitorFactoryResources::Quit()
{
	guard.lock();
	if(monitorResources == false)
	{
		guard.unlock();
	}
	else
	{
		monitorResources = false;
		guard.unlock();
		msleep(monitorInterval);
	}
}

// -------------------------------------------------------
//			run()
// -------------------------------------------------------
void MonitorFactoryResources::run()
{
	while(monitorResources)
	{
		msleep(monitorInterval);

		MutexLocker lock(&guard);

		if(monitorResources == false) break;

		this->updateFactoryMaps();

#if 0
		mapComponentResources.clear();
		// retrieve resources used by all processes, and the node
		for(MapComponentInformation::const_iterator it = mapComponentFactories.begin(), it_end = mapComponentFactories.end(); it != it_end; ++it)
		{
			typedef map<int, HostResources> MapPidHostResources;
			MapPidHostResources mapPidHostResources;

			const string &factoryHost = it->first;
			const string &factoryIP = it->second.GetHostIP();
			try
			{
				smMutexProxyGuard.lock();
				ProcessFactoryProxy *factory = getOrCreateFactoryProxy(factoryIP, (it->second).GetPortNumber());
				if(factory == NULL)
				{
					throw string("Factory unreachable");
				}
				else
				{
					mapPidHostResources = factory->GetMapComponentResources();
					smMutexProxyGuard.unlock();
				}
			}
			catch(...)
			{
				//cerr << "MonitorFactoryResources::run(): WARNING! Factory " << factoryIP << " unreachable! " << endl;
				smMutexProxyGuard.unlock();
				//this->removeFactory(it->second);
				continue;
			}

			for(MapPidHostResources::const_iterator it = mapPidHostResources.begin(), it_end = mapPidHostResources.end(); it != it_end; ++it)
			{
				const HostResources &res = it->second;
				mapComponentResources[res.GetComponentName()] = res;
			}
		}
#endif
	}

	cerr << "MonitorFactoryResources::run(): Finishing... " << endl;
}

void MonitorFactoryResources::updateFactoryMaps()
{
	MutexLocker lockProxy(&updateFactoryProxyGuard);
	
	for(MapComponentInformation::iterator it = mapAddProcessFactory.begin(), it_end = mapAddProcessFactory.end(); it != it_end; ++it)			
	{
		if(mapRemoveProcessFactory.count(it->first) > 0) 	// This condition should not happen!
		{
			cerr << "MonitorFactoryResources::updateFactoryMaps(): WARNING! Factory " << it->first << " marked for removal! " << endl;
			continue;
		}

		// This could happen!
		//if(mapComponentFactories.count(it->first) > 0)
		//	cerr << "MonitorFactoryResources::updateFactoryMaps(): WARNING! Factory " << it->first << " already monitored! " << endl;
		
		mapComponentFactories.insert(pair<string, HostInformation>(it->first, it->second));
	}

	MapComponentResources eraseComponenResources;
	for(MapComponentInformation::iterator it = mapRemoveProcessFactory.begin(), it_end = mapRemoveProcessFactory.end(); it != it_end; ++it)
	{
		const HostInformation &factoryInfo = it->second;
		for(MapComponentResources::iterator it_in = mapComponentResources.begin(), it_in_end = mapComponentResources.end(); it_in != it_in_end; ++it_in)
		{	
			HostResources &res = it_in->second;
			if(res.GetHostName() == factoryInfo.GetHostName())
				eraseComponenResources.insert(pair<string, HostResources>(it_in->first, it_in->second));
		}
		mapComponentFactories.erase(it->first);
	}
	
	for(MapComponentResources::iterator it = eraseComponenResources.begin(), it_end = eraseComponenResources.end(); it != it_end; ++it)
		mapComponentResources.erase(it->first);
	

	mapAddProcessFactory.clear();
	mapRemoveProcessFactory.clear();
}

/*#if 0
ProcessFactoryProxy* MonitorFactoryResources::getOrCreateFactoryProxy(const string &factoryHost, const int &factoryPort)
{
	// Create factory proxy
	if(factoryHost.empty() == true || factoryPort <= -1) return NULL;

	ProcessFactoryProxy *proxy = NULL;
	if(mapHostProcessFactoryProxy.count(factoryHost) >= 1)
	{
		proxy = mapHostProcessFactoryProxy[factoryHost];
	}

	CheckFactoryProxyIsConnected isConnected(proxy, factoryHost, factoryPort);
	ProcessFactoryProxy *newProxy = isConnected.GetFactoryProxy();

	if(proxy != NULL && newProxy != proxy)
		delete proxy;
	
	mapHostProcessFactoryProxy[factoryHost] = newProxy;

	//if(newProxy == NULL)
	//	cout << "getOrCreateFactoryProxy(): WARNING! Unable to contact factory (" << factoryHost << "," << factoryPort << ")" << endl;

	return newProxy;
}

#else*/
ProcessFactoryProxy* MonitorFactoryResources::getOrCreateFactoryProxy(const string &factoryHost, const int &factoryPort)
{
	if(factoryHost.empty() == true || factoryPort <= -1) return NULL;

	// Create factory proxy
	ProcessFactoryProxy *proxy = NULL;
	if(mapHostProcessFactoryProxy.count(factoryHost) >= 1)
	{
		proxy = mapHostProcessFactoryProxy[factoryHost];
	}
	
	bool createNewProxy = false;
	if(proxy != NULL) if(proxy->IsConnected() == false) createNewProxy = true;

	if(proxy == NULL || createNewProxy)
	{
		try
		{
			if(proxy != NULL) delete proxy;

			proxy = new ProcessFactoryProxy(InterfaceHandle(factoryPort, factoryHost), true);
			mapHostProcessFactoryProxy[factoryHost] = proxy;
		}
		catch(...)
		{
			//cerr << "MonitorFactoryResources::createFactoryProxy(): ERROR! Unable to contact factory (" << factoryHost << "," << factoryPort << ")" << endl;
			mapHostProcessFactoryProxy[factoryHost] = NULL;
			return NULL;
		}
	}

	return proxy;
}

//#endif

// -------------------------------------------------------
//	public add/remove process
// -------------------------------------------------------
void MonitorFactoryResources::AddFactory(const HostInformation &info) 
{	
	MutexLocker lockProxy(&updateFactoryProxyGuard);

	if(mapRemoveProcessFactory.count(info.GetHostName()))
		mapRemoveProcessFactory.erase(info.GetHostName());

	mapAddProcessFactory.insert(pair<string, HostInformation>(info.GetHostName(), info));
}

void MonitorFactoryResources::RemoveFactory(const HostInformation &info) 
{	
	MutexLocker lockProxy(&updateFactoryProxyGuard);

	if(mapAddProcessFactory.count(info.GetHostName()))
		mapAddProcessFactory.erase(info.GetHostName());

	mapRemoveProcessFactory.insert(pair<string, HostInformation>(info.GetHostName(), info));
}

// -------------------------------------------------------
//	Get functions to retrieve resource information
// -------------------------------------------------------
HostResources MonitorFactoryResources::GetHostResources(const string &componentName)
{
	MutexLocker lock(&guard);

	if(mapComponentResources.count(componentName) <= 0)
		return HostResources(componentName);

	return mapComponentResources[componentName];
}

string MonitorFactoryResources::GetAllFactoryResources()
{
	MutexLocker lock(&guard);
	
	stringstream stream;
	for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
	{
		if((it->second).GetResourceView() == HostResources::NODE)
		{
			stream << "--factory " << it->second;
		}
		else if((it->second).GetResourceView() == HostResources::PROCESS)
		{
			stream << "--component " << it->second;
		}
	}
	
	return stream.str();
}

map<string, HostResources> MonitorFactoryResources::GetMapAllResources()
{
	MutexLocker lock(&guard);
	return mapComponentResources;
}

void MonitorFactoryResources::SetHostResources(HostInformation &factoryInfo, map<int, HostResources> &mapHostResources)
{
	//AddFactory(factoryInfo);
	MutexLocker lock(&guard);

	// -> clear old process-resources from host (also potentially dead ones
	vector<string> vectorComponentNames;
	for(MapComponentResources::iterator it = mapComponentResources.begin(), it_end = mapComponentResources.end(); it != it_end; ++it)
	{
		HostResources &res = it->second;
		if(res.GetHostName() == factoryInfo.GetHostName())
			vectorComponentNames.push_back(res.GetComponentName());
	}

	for(vector<string>::iterator it = vectorComponentNames.begin(), it_end = vectorComponentNames.end(); it != it_end; ++it)
		mapComponentResources.erase(*it);

	// -> update new process-resources
	for(map<int, HostResources>::iterator it = mapHostResources.begin(), it_end = mapHostResources.end(); it != it_end; ++it)
	{
		HostResources &res = it->second;

		if(mapComponentResources.count(res.GetComponentName()) <= 0)
			mapComponentResources.insert(pair<string, HostResources>(res.GetComponentName(), res));
		else
			mapComponentResources[res.GetComponentName()] = res;
	}
}

} // namespace SystemManagerSpace

