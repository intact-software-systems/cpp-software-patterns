#include"ServiceManager/ServiceManager.h"
#include"ServiceManager/ServiceManagerUI.h"

//#define ALIVE_CHECKER_MS				1
#define LATE_CHECKIN_DEADLINE_SECONDS	5
#define MULTICAST_ADDRESS_START			"234.5.6."
#define MAXIMUM_NUMBER_OF_PUBLISHERS	200
#define HOST_INFORMATION_FILE			"HostInformation.txt"

namespace ServiceManagerSpace
{

int ServiceManager::portNumberAssigner = 6000;

// --------------------------------------------------------
//			init ServiceManagerConfig
// --------------------------------------------------------
int ServiceManagerConfig::serviceManagerPort = MicroMiddleware::MiddlewareSettings::GetServiceManagerPort();
string ServiceManagerConfig::hostInformationFile = HOST_INFORMATION_FILE;

// --------------------------------------------------------
//			ServiceManager constructor/destructor
// --------------------------------------------------------
ServiceManager::ServiceManager(const string &name) 
	: updateLock()
	, timeCounter(LATE_CHECKIN_DEADLINE_SECONDS)
	, stopServiceManager(false)
	, serviceManagerUI(NULL)
	, fileName(ServiceManagerConfig::hostInformationFile)
{
	initServiceManager();
}

ServiceManager::~ServiceManager(void)
{
	// if ServiceManager's destructor is called we assume it was shut down correctly
	// therefore, no host information is required upon restart
	if(outFile.is_open())
		outFile.close();

	outFile.open(ServiceManagerConfig::hostInformationFile.c_str(), fstream::out);
	if(outFile.is_open())
	{
		outFile << "" ;
		outFile.flush();
		outFile.close();
	}
}

void ServiceManager::initServiceManager()
{
	for(int i = 7; i <= MAXIMUM_NUMBER_OF_PUBLISHERS; i++)
	{
		stringstream address;
		address << MULTICAST_ADDRESS_START << i ;

		multicastAddresses.push_back(address.str());
	}

	parseHostInformationFromDisc(fileName);
	outFile.open(fileName.c_str(), fstream::out);

	if(outFile.is_open())
	{
		for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
		{
			HostInformationSpace::WriteHostInformationToDisc(outFile, it->second);
			mapAliveCheck[(it->second).GetComponentName()] = 0;
		}
	}

	serviceManagerUI = new ServiceManagerUI(*this);
}

// --------------------------------------------------------
//			Thread::run()
// --------------------------------------------------------
void ServiceManager::run()
{
	cerr << "ServiceManager::run(): ServiceManager is running. Alive check interval " << ALIVE_CHECK_INTERVAL_MS << endl;
	while(true)
	{
		msleep(ALIVE_CHECK_INTERVAL_MS);
		
		MutexLocker lock(&updateLock);
		if(stopServiceManager == true) break;

		timeCounter++;

		for(MapAliveCheck::iterator it = mapAliveCheck.begin(), it_end = mapAliveCheck.end(); it != it_end; ++it)
		{
			ASSERT(mapHostInformation.count(it->first));

			if(it->second > timeCounter - LATE_CHECKIN_DEADLINE_SECONDS) 
			{
				ASSERT(mapHostInformation.count(it->first));
				mapHostInformation[it->first].SetOnlineStatus(HostInformation::ONLINE);
			}
			else if(mapHostInformation[it->first].GetOnlineStatus() == HostInformation::ONLINE)
			{
				cerr << it->first << " is assumed OFFLINE (missed several alive messages): " << timeCounter - LATE_CHECKIN_DEADLINE_SECONDS  <<endl;
				mapHostInformation[it->first].SetOnlineStatus(HostInformation::OFFLINE);
			}
		}
	}
	cerr << "ServiceManager::run(): ServiceManager is stopping." << endl;
}

// --------------------------------------------------------
//			public aliveChecker
// --------------------------------------------------------
HostInformation	ServiceManager::AliveChecker(HostInformation info, string serviceName)
{
	MutexLocker lock(&updateLock);
	
	// make sure it is set to ONLINE
	info.SetOnlineStatus(HostInformation::ONLINE);

	//cout << "ServiceManager::aliveChecker():" << " name : " << componentName << " info: "  << info << endl;

	// -- start check --
	if(info.GetHostDescription() == HostInformation::COMPONENT)
		cout << "ServiceManager::AliveChecker(): WARNING! Wrong HostDescription! HostInformation::COMPONENT given for: " << info.GetComponentName() << endl;
	if(info.GetComponentName().empty()) throw MiddlewareException("ServiceManager::AliveChecker(info): ERROR! Checked in HostInformation with no componentName!");
	// -- end check --

	mapAliveCheck[info.GetComponentName()] = timeCounter;

	// Update host information
	mapHostInformation[info.GetComponentName()] = info;

	// host sending the alive check is a publisher/stub
	if(info.GetHostDescription() == HostInformation::PUBLISHER || 
		info.GetHostDescription() == HostInformation::STUB)
	{
		//ASSERT(serviceName == "");
		return info;
	}
	
	// else -> host sending the alive check is a subscriber/proxy, 
	if(mapHostInformation.count(serviceName) <= 0) // serviceName is OFFLINE
		return HostInformation(serviceName);
	
	return mapHostInformation[serviceName];
}


map<string, HostInformation> 
ServiceManager::AliveChecker(MapHostInformation &mapRegisteredServices, SetServiceName &subscribedServices)
{
	MutexLocker lock(&updateLock);

	for(MapHostInformation::iterator it = mapRegisteredServices.begin(), it_end = mapRegisteredServices.end(); it != it_end; ++it)
	{
		HostInformation &info = it->second;
		ASSERT(info.IsValid());

		info.SetOnlineStatus(HostInformation::ONLINE); // make sure it is set to ONLINE
	
		//IDEBUG() << " name : " << info.GetComponentName() ;

		// -- start check --
		if(info.GetHostDescription() == HostInformation::COMPONENT)
			cout << "ServiceManager::AliveChecker(): WARNING! Wrong HostDescription! HostInformation::COMPONENT given for: " << info.GetComponentName() << endl;
		if(info.GetComponentName().empty()) throw MiddlewareException("ServiceManager::AliveChecker(info): ERROR! Checked in HostInformation with no componentName!");
		// -- end check --

		mapAliveCheck[info.GetComponentName()] = timeCounter;	// Update time counter
		mapHostInformation[info.GetComponentName()] = info;  	// Update host information
	}

	MapHostInformation mapSubscribedServices;
	for(SetServiceName::iterator it = subscribedServices.begin(), it_end = subscribedServices.end(); it != it_end; ++it)
	{
		string serviceName = *it;
		if(mapHostInformation.count(serviceName) <= 0) 
		{
			//IDEBUG()  << "WARNING! Service not present " << serviceName ;
			continue;
		}

		HostInformation &info = mapHostInformation[serviceName];
		ASSERT(info.IsValid());
		mapSubscribedServices.insert(pair<string, HostInformation>(info.GetComponentName(), info));
	}

	return mapSubscribedServices;
}

// --------------------------------------------------------
//			public hostInformation
// --------------------------------------------------------
HostInformation ServiceManager::GetServiceInformation(string serviceName)
{
	MutexLocker lock(&updateLock);

	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
	{
		if(it->first != serviceName) continue;
		
		return it->second;
	}

	return HostInformation(serviceName);
}

// --------------------------------------------------------
//			public hostRegistration
//	TODO: Write information OFFLINE to disc, and detect it
// when reading from disc.
// --------------------------------------------------------
HostInformation ServiceManager::ServiceRegistration(HostInformation info) 
{
	MutexLocker lock(&updateLock);
	
	serviceRegistration(info);
	
	return info;
}

void ServiceManager::ServiceRegistration(MapHostInformation &mapServices) 
{
	for(MapHostInformation::iterator it = mapServices.begin(), it_end= mapServices.end(); it != it_end; ++it)
	{
		HostInformation &hostInfo = it->second;

		MutexLocker lock(&updateLock);
		serviceRegistration(hostInfo);
	}
}

void ServiceManager::ShutDown()
{
	Quit();
}

void ServiceManager::ClearServices()
{
	MutexLocker lock(&updateLock);

	MapHostInformation removeHostInformation;

	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
	{
		HostInformation info = it->second;
		if(info.GetOnlineStatus() == HostInformation::ONLINE) continue;

		removeHostInformation.insert(pair<string, HostInformation>(info.GetComponentName(), info));
	}

	for(MapHostInformation::iterator it = removeHostInformation.begin(), it_end = removeHostInformation.end(); it != it_end; ++it)
	{
		HostInformation &info = it->second;
		if(info.GetHostDescription() == HostInformation::PROXY || 
			info.GetHostDescription() == HostInformation::STUB ||
			info.GetHostDescription() == HostInformation::SUBSCRIBER)
		{
			hostOffline(info);
		}
		else if(info.GetHostDescription() == HostInformation::PUBLISHER)
			publisherOffline(info);
	}
}

map<string, HostInformation> ServiceManager::GetAllServiceInformation()
{
	MutexLocker lock(&updateLock);

	MapHostInformation mapHostInfo;

	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
		mapHostInfo.insert(pair<string, HostInformation>(it->first, it->second));

	//cerr << "Returning : " << endl;
	//for(MapHostInformation::const_iterator it = mapHostInfo.begin(), it_end = mapHostInfo.end(); it != it_end; ++it)
	//	cerr << it->second << endl;

	return mapHostInfo;
}

// --------------------------------------------------------
//		-> not thread safe	
// 			private hostOnline/hostOffline
//				-> called from ServiceRegistration
// --------------------------------------------------------
void ServiceManager::serviceRegistration(HostInformation &info) 
{
	// -- start error handling --
	if(info.GetHostName().empty()) throw Exception("ServiceManager::hostRegistration(info): hostName is empty! ");
	if(info.GetComponentName().empty()) throw Exception("ServiceManager::hostRegistration(info): serviceName is empty! ");
	// -- end error handling --

	if(info.GetHostDescription() == HostInformation::PROXY || 
		info.GetHostDescription() == HostInformation::STUB ||
		info.GetHostDescription() == HostInformation::SUBSCRIBER)
	{
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			hostOnline(info);
			HostInformationSpace::WriteHostInformationToDisc(outFile, info);
		}
		else
		{
			IDEBUG() << "Service marked OFFLINE " << info.GetComponentName() ;
			hostOffline(info);
		}
	}
	else if(info.GetHostDescription() == HostInformation::PUBLISHER)
	{
		if(info.GetOnlineStatus() == HostInformation::OFFLINE)  		// OFFLINE --> Publisher going offline
		{
			publisherOffline(info);
			//return HostInformation(info.GetComponentName());
		}
		else
		{
			publisherOnline(info);										// ONLINE --> Publisher going Online
			HostInformationSpace::WriteHostInformationToDisc(outFile, info);
		}
	}
}


void ServiceManager::hostOnline(HostInformation &info)
{
	HostInformation *registered_info = findHostInformation(info);
	if(registered_info) // -- already registered
	{
		if(registered_info->GetOnlineStatus() == HostInformation::ONLINE)
			cout << "hostOnline(): WARNING! " << info.GetComponentName() << " already registered ONLINE." << endl;

		if(info.GetPortNumber() == -1 && registered_info->GetPortNumber() > 0)
			info.SetPortNumber(registered_info->GetPortNumber());
	}

	if(info.GetPortNumber() == -1) // -> assign port number
	{
		info.SetPortNumber(ServiceManager::portNumberAssigner++);
	}

	// -- start check --
	if(info.GetComponentName().empty()) throw Exception("ServiceManager::hostOnline(info): ComponentName is empty!");
	// -- end check --
	
	cout << "hostOnline(): " << info << endl;
	mapAliveCheck[info.GetComponentName()] = timeCounter;
	mapHostInformation[info.GetComponentName()] = info;
}

void ServiceManager::hostOffline(HostInformation &info)
{
	removeHostInformation(info);
}

// --------------------------------------------------------
//			private publisherOnline
//				-> called from hostRegistration
// --------------------------------------------------------
void ServiceManager::publisherOnline(HostInformation &info)
{
	HostInformation *registered_info = findHostInformation(info);

	if(registered_info) // -- already registered
	{
		if(registered_info->GetOnlineStatus() == HostInformation::ONLINE)
			cout << "hostOnline(): WARNING! " << info.GetComponentName() << " already registered ONLINE." << endl;

		if(info.GetMulticastAddress().empty() && !registered_info->GetMulticastAddress().empty())
			info.SetMulticastAddress(registered_info->GetMulticastAddress());
		if(info.GetPortNumber() == -1 && registered_info->GetPortNumber() > 0)
			info.SetPortNumber(registered_info->GetPortNumber());
	}

	if(info.GetMulticastAddress().empty()) // -> assign multicast address to publisher
	{
		info.SetMulticastAddress(multicastAddresses.front());	
		usedMulticastAddresses.push_back(multicastAddresses.front());
		multicastAddresses.pop_front();
	}

	if(info.GetPortNumber() == -1) // -> assign port number to publisher
	{
		info.SetPortNumber(ServiceManager::portNumberAssigner++);
	}

	cout << "publisherOnline(): " << info << endl;
	mapAliveCheck[info.GetComponentName()] = timeCounter;
	mapHostInformation[info.GetComponentName()] = info;
}

// --------------------------------------------------------
//			private publisherOffline
//				-> called from hostRegistration
// --------------------------------------------------------
void ServiceManager::publisherOffline(HostInformation &info)
{
	cout << "publisherOffline():" << info << endl;

	recycleMulticastAddress(info.GetMulticastAddress()); // recycle multicast address
	removeHostInformation(info);
}

// --------------------------------------------------------
//	private findHostInformation/removeHostInformation/etc.
// --------------------------------------------------------
HostInformation* ServiceManager::findHostInformation(const HostInformation &info)
{
	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
	{
		if(it->first == info.GetComponentName()) 
		{
			//IDEBUG() << " Publisher " << info.GetComponentName() << " already registered! " ;
			return &(it->second);
		}
	}
	return NULL;
}

void ServiceManager::removeHostInformation(const HostInformation &info)
{
	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
	{
		if(it->first == info.GetComponentName()) 
		{
			mapHostInformation.erase(it);
			break;
		}
	}
	
	for(MapAliveCheck::iterator it = mapAliveCheck.begin(), it_end = mapAliveCheck.end(); it != it_end; ++it)
		if(it->first == info.GetComponentName())
		{
			mapAliveCheck.erase(it);
			break;
		}
}

void ServiceManager::parseHostInformationFromDisc(string &filename)
{
	vector<HostInformation> vectorHostInformation;
	if(HostInformationSpace::ParseHostInformationFromDisc(filename, vectorHostInformation) == false)
		return;
	
	while(!vectorHostInformation.empty())
	{
		HostInformation info = vectorHostInformation.back();
		vectorHostInformation.pop_back();

		// -- start check --
		if(info.GetComponentName().empty()) throw Exception("ServiceManager::parseHostInformationFromDisc(fileName): ComponentName is empty!");
		// -- end check --
		
		// Newer information already inserted? Drop current...
		if(mapHostInformation.count(info.GetComponentName()) >= 1) continue;

		// Insert current information to map
		usedMulticastAddress(info.GetMulticastAddress()); // mark multicast address as in-use
		// -- update port assignment usage --
		if(info.GetPortNumber() > -1) ServiceManager::portNumberAssigner = info.GetPortNumber() + 1;

		mapHostInformation[info.GetComponentName()] = info;
	}
}


void ServiceManager::recycleMulticastAddress(const string &multicastAddress)
{
	if(multicastAddress.empty()) return;
	
	multicastAddresses.push_back(multicastAddress);
	for(ListAddresses::iterator it = usedMulticastAddresses.begin(), it_end = usedMulticastAddresses.end(); it != it_end; ++it)
		if(*it == multicastAddress)
		{
			usedMulticastAddresses.erase(it);
			break;
		}
}

void ServiceManager::usedMulticastAddress(const string &multicastAddress)
{
	if(multicastAddress.empty()) return;

	usedMulticastAddresses.push_back(multicastAddress);
	for(ListAddresses::iterator it = multicastAddresses.begin(), it_end = multicastAddresses.end(); it != it_end; ++it)
		if(*it == multicastAddress)
		{
			multicastAddresses.erase(it);
			break;
		}
}

void ServiceManager::cmdPrintHostInformation(ostream &ostr)
{
	if(mapHostInformation.empty())
	{
		ostr << "---- No components in ServiceManager's database..." << endl;
		return;
	}

	ostr << "---- OFFLINE Components in ServiceManager's database: " << endl;
	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
		if(it->second.GetOnlineStatus() == HostInformation::OFFLINE)
			ostr << it->second << endl;

	ostr << endl;
	ostr << "---- ONLINE Components in ServiceManager's database: " << endl;
	for(MapHostInformation::iterator it = mapHostInformation.begin(), it_end = mapHostInformation.end(); it != it_end; ++it)
		if(it->second.GetOnlineStatus() == HostInformation::ONLINE)
			ostr << it->second << endl;
}

} // namespace ServiceManagerSpace

