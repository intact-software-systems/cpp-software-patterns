#include "SystemManager/SystemManager.h"
#include "SystemManager/SystemManagerUI.h"

#ifdef USE_GCC
#include<sys/time.h>
#endif

//#define ALIVE_CHECKER_MS						1000
#define LATE_CHECKIN_DEADLINE_SECONDS			5
#define COMPONENT_INFORMATION_FILE				"ComponentInformation.txt"
#define MONITOR_FACTORY_RESOURCES_INTERVAL_MS	2000

namespace SystemManagerSpace
{
// -------------------------------------------------------
//			initalize SystemManagerConfig
// -------------------------------------------------------
int SystemManagerConfig::systemManagerPort = MicroMiddleware::MiddlewareSettings::GetSystemManagerPort();
string SystemManagerConfig::systemConfigurationFile = string("TestConfiguration.xml");
string SystemManagerConfig::initialConfigurationFile;
string SystemManagerConfig::componentInformationFile = COMPONENT_INFORMATION_FILE;
int SystemManagerConfig::loadConfigFileAtStartup = 0; // 0 = no, 1 = yes (other values undefined)

// -------------------------------------------------------
//	constructor/destructor SystemManager
//		- Constructor: Initializes the SystemManager
// -------------------------------------------------------
SystemManager::SystemManager(string name) 
		: ComponentBase(name)
		, systemManagerCommand(NULL)
		, monitorFactoryResources(mapHostProcessFactoryProxy, updateLock, MONITOR_FACTORY_RESOURCES_INTERVAL_MS)
		, exceptionMonitor(name)
		, stopSystemManager(false)
		, timeCounter(LATE_CHECKIN_DEADLINE_SECONDS)
{
	srand(Thread::currentThreadId()); //gettimeofday(NULL, NULL));

	initSystemManager(SystemManagerConfig::systemConfigurationFile, SystemManagerConfig::componentInformationFile);
}

SystemManager::~SystemManager()
{ 
	// if SystemManager's destructor is called we assume it was shut down correctly
	// therefore, no host information is required upon restart
	this->clearComponentInformationFile();

	monitorFactoryResources.Quit();
}

void SystemManager::clearComponentInformationFile()
{
	if(outComponentFile.is_open())
		outComponentFile.close();

	outComponentFile.open(SystemManagerConfig::componentInformationFile.c_str(), fstream::out);
	if(outComponentFile.is_open())
	{
		outComponentFile << "" ;
		outComponentFile.flush();
		outComponentFile.close();
	}
}

// -------------------------------------------------------
//	initalize the system manager -> called from constructor
// -------------------------------------------------------
void SystemManager::initSystemManager(string configFile, string componentFile)
{
	if(!componentFile.empty() || std::strcmp(componentFile.c_str(), "") != 0)
	{
		parseHostInformationFromDisc(componentFile); // -> inserts component information to mapHostComponentInformation

		outComponentFile.open(componentFile.c_str(), fstream::out);
		if(outComponentFile.is_open())
		{
			for(MapHostComponentInformation::iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
			{
				const MapComponentInformation &mapComp = (mapHostComponentInformation[it->first]);
				for(MapComponentInformation::const_iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
					HostInformationSpace::WriteHostInformationToDisc(outComponentFile, it_in->second);
			}

			if(!lookupServerInfo.GetHostName().empty())
				HostInformationSpace::WriteHostInformationToDisc(outComponentFile, lookupServerInfo);
		}
	}

	// only load at startup if SystemManager did not crash
	if(mapHostComponentInformation.empty() && SystemManagerConfig::loadConfigFileAtStartup == 1)
		cmdParseStartupScript(std::cout, mapHostComponentInformation, configFile);
	else if(!mapHostComponentInformation.empty() && SystemManagerConfig::loadConfigFileAtStartup == 1)
		cout << "WARNING! " << configFile << " not loaded at startup because SystemManager recovered component information from " << componentFile << endl;

	systemManagerCommand = new SystemManagerUI(*this);
}

// -------------------------------------------------------
//				class SystemManager
// Main function: 
// - Start system manager with XML file containing
//		- List of Component Factories (port, host)
//		- List of processes/components to execute on the component factories
// - Keep track of component factories
// - Maintain list of running processes on each component factory
// TODO: run function should use a monitor with a timed wait function,
//		 instead of a sleep(ms) and mutex
// -------------------------------------------------------
void SystemManager::run()
{
	cout << "SystemManager::run(): SystemManager is running." << endl;
	
	// if database contains components which were not loaded at startup, then verify their onlineStatus
	if(!mapHostComponentInformation.empty() && SystemManagerConfig::loadConfigFileAtStartup == 0)
		verifyComponentsAreRunning = verifySystemManagerData(mapHostComponentInformation);

	while(true)
	{
		msleep(ALIVE_CHECK_INTERVAL_MS);
		MutexLocker Lock(&updateLock);

		if(stopSystemManager) break;

		timeCounter++;
		
		if(!verifyComponentsAreRunning.empty())
			verifyComponentsAreRunning = verifySystemManagerData(verifyComponentsAreRunning);

		stringstream errorMsg;
		startAllComponents(mapHostComponentInformation, errorMsg);
		
		checkIfFactoriesAreConnected();
	}
}

void SystemManager::checkIfFactoriesAreConnected()
{
#if 1
	for(MapStringInt::iterator it = mapAliveCheck.begin(), it_end = mapAliveCheck.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) <= 0)
		{
			cout << "SystemManager::run(): WARNING! ProcessFactory " << it->first << " not in map! " << endl;
			continue;
		}

		HostInformation &factoryInfo = mapHostProcessFactory[it->first];
		
		// -- if OFFLINE then wait for factory to register again through HostRegistration()
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE)
			continue;
	
		if(it->second > (timeCounter - LATE_CHECKIN_DEADLINE_SECONDS))
		{
			factoryInfo.SetOnlineStatus(HostInformation::ONLINE);
		}
		else if(it->second <= (timeCounter - LATE_CHECKIN_DEADLINE_SECONDS))
		{
			factoryInfo.SetOnlineStatus(HostInformation::OFFLINE);
			monitorFactoryResources.RemoveFactory(factoryInfo);
		}
	}

#elif 1
	for(MapStringInt::iterator it = mapAliveCheck.begin(), it_end = mapAliveCheck.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) <= 0)
		{
			cout << "SystemManager::run(): WARNING! ProcessFactory " << it->first << " not in map! " << endl;
			continue;
		}

		HostInformation &factoryInfo = mapHostProcessFactory[it->first];
		
		// -- if OFFLINE then wait for factory to register again through HostRegistration()
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE)
			continue;
					
		// -- factory status is ONLINE, check if that is correct
		ProcessFactoryProxy *proxy = this->getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
		if(proxy == NULL)
		{
			if(factoryInfo.GetOnlineStatus() == HostInformation::ONLINE)
				cout << "SystemManager::run(): Factory " << factoryInfo.GetHostName() << " with InterfaceHandle(" << factoryInfo.GetHostIP() << "," << factoryInfo.GetPortNumber() << ") is OFFLINE " << endl;
			
			factoryInfo.SetOnlineStatus(HostInformation::OFFLINE);
			monitorFactoryResources.RemoveFactory(factoryInfo);
		}
		else // proxy != NULL -> connection to factory 
		{
			if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE &&
				it->second > (timeCounter - LATE_CHECKIN_DEADLINE_SECONDS))
			{
				factoryInfo.SetOnlineStatus(HostInformation::ONLINE);
			}
			else if(it->second <= (timeCounter - LATE_CHECKIN_DEADLINE_SECONDS))
			{
			//	cout << "SystemManager::run(): ERROR! SystemManager can reach factory on host " 
			//				<< factoryInfo.GetHostName() << " with InterfaceHandle( " 
			//				<< factoryInfo.GetHostIP() << ","  << factoryInfo.GetPortNumber() << ")" 
			//				<< " but factory is not communicating with SystemManager! " << endl;
				factoryInfo.SetOnlineStatus(HostInformation::OFFLINE);
			}
		}
	}
#endif
}

// -------------------------------------------------------
//	verifies/retrieves the OnlineStatus from the factories
// -------------------------------------------------------
map<string, map<string, HostInformation> >
SystemManager::verifySystemManagerData(const MapHostComponentInformation &verifyComponentsAreRunning)
{
	MapHostComponentInformation notVerifiedComponents;
	// verify system manager's current component data
	for(MapHostComponentInformation::const_iterator it = verifyComponentsAreRunning.begin(), it_end = verifyComponentsAreRunning.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) <= 0) 
		{
			notVerifiedComponents.insert(*it);
			continue;
		}
		
		HostInformation factoryInfo = mapHostProcessFactory[it->first];
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE) 
		{
			notVerifiedComponents.insert(*it);
			continue;
		}

		ProcessFactoryProxy *proxy = getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
		if(proxy == NULL) 
		{
			notVerifiedComponents.insert(*it);
			continue;
		}

		const MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::const_iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			const HostInformation &info = it_in->second;
			HostInformation::OnlineStatus currentStatus;

			try
			{
				if(info.GetProcessId() == -1) 
					currentStatus = HostInformation::OFFLINE;
				else if(!proxy->IsComponentRunning(info))
					currentStatus = HostInformation::OFFLINE;
				else 
					currentStatus = HostInformation::ONLINE;
			}
			catch(...)
			{
				cout << "verifySystemManagerData(): WARNING! Factory (" << factoryInfo.GetHostName() << ", " << factoryInfo.GetHostIP() << "," << factoryInfo.GetPortNumber() << ") unreachable!" << endl;
				notVerifiedComponents.insert(*it);
				break;
			}

			HostInformation &currentInfo = (mapHostComponentInformation[it->first])[info.GetComponentName()];
			if(currentInfo.GetProcessId() != info.GetProcessId()) continue;
				
			currentInfo.SetOnlineStatus(currentStatus);
		}
	}

	return notVerifiedComponents;
}

// -------------------------------------------------------
//	Each component-factory reports that it is alive
// -------------------------------------------------------
HostInformation SystemManager::AliveChecker(HostInformation factoryInfo)
{
	if(stopSystemManager)
	{
		factoryInfo.SetOnlineStatus(HostInformation::OFFLINE);
		return factoryInfo;
	}

	MutexLocker lock(&updateLock);

	// Fix silently if factoryInfo is wrongly marked as OFFLINE
	factoryInfo.SetOnlineStatus(HostInformation::ONLINE);

	{	// -- start error handling --
		if(factoryInfo.GetHostDescription() != HostInformation::COMPONENT_FACTORY)
		{
			stringstream stream;
			stream << "SystemManager::AliveChecker(info): WARNING! " << factoryInfo.GetComponentName() << "'s hostDescription is not COMPONENT_FACTORY but " << factoryInfo.GetHostDescription() << endl;
			cout << stream.str();
			ExceptionMessage exceptionMessage(factoryInfo, stream.str(), GetTimeStamp(), MiddlewareException::CONFIGURATION_ERROR);
			exceptionMonitor.PostException(exceptionMessage);
		}
		if(mapHostProcessFactory.count(factoryInfo.GetHostName()) > 0)
		{
			HostInformation currentInfo = mapHostProcessFactory[factoryInfo.GetHostName()];
			if(factoryInfo.GetComponentName() != currentInfo.GetComponentName())
			{
				stringstream stream;
				stream << "SystemManager::AliveChecker(info): WARNING! Factory's HostInformation changed component name! ";
				stream << " New : " << factoryInfo.GetComponentName() << "  Old: " << currentInfo.GetComponentName() << endl;
				cout << stream.str();
				ExceptionMessage exceptionMessage(factoryInfo, stream.str(), GetTimeStamp(), MiddlewareException::CONFIGURATION_ERROR);
				exceptionMonitor.PostException(exceptionMessage);
			}
		}	
		if(factoryInfo.GetHostName().empty()) throw MiddlewareException("SystemManager::AliveChecker(info): ERROR! Factory's HostName is empty! ");
		if(factoryInfo.GetComponentName().empty()) throw MiddlewareException("SystemManager::AliveChecker(info): ERROR! Factory has no componentName!");
	}	// -- end error handling --


	mapAliveCheck[factoryInfo.GetHostName()] = timeCounter;
	mapHostProcessFactory[factoryInfo.GetHostName()] = factoryInfo; 	// Update host information

	return factoryInfo;
}

// -------------------------------------------------------
//	factories and components are registered
// -------------------------------------------------------
HostInformation SystemManager::HostRegistration(HostInformation info)
{
	// -- start error handling --
	if(info.GetHostName().empty()) throw MiddlewareException("SystemManager::hostRegistration(info): hostName is empty! ");
	if(info.GetComponentName().empty()) throw MiddlewareException("SystemManager::hostRegistration(info): componentName is empty! ");
	// -- end error handling --

	if(stopSystemManager)
	{
		info.SetOnlineStatus(HostInformation::OFFLINE);
		return info;
	}

	MutexLocker Lock(&updateLock);

	if(info.GetHostDescription() == HostInformation::COMPONENT_FACTORY)
	{
		// -> Factory is only re-registering if either - Factory went down/up, or - SystemManager is restarted
		if(mapHostProcessFactoryProxy.count(info.GetHostIP()) >= 1)
		{
			ProcessFactoryProxy *proxy = mapHostProcessFactoryProxy[info.GetHostIP()];
			if(proxy != NULL) 
			{
				proxy->StartOrRestartClient(InterfaceHandle(info.GetHostIP(), info.GetPortNumber()));
			}
			//MapProcessFactoryProxy::iterator it = mapHostProcessFactoryProxy.find(info.GetHostIP());
			//mapHostProcessFactoryProxy.erase(it);
		}

		// -- start verification --
		// -> Verify that Component Factory and System Manager has the same Component information
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			// insert components to verifyComponentsAreRunning if any are ONLINE
			MapComponentInformation &mapVerifyComp = verifyComponentsAreRunning[info.GetHostName()];
			MapComponentInformation &mapComp = mapHostComponentInformation[info.GetHostName()];
			for(MapComponentInformation::iterator it = mapComp.begin(), it_end = mapComp.end(); it != it_end; ++it)
			{
				(it->second).SetHostIP(info.GetHostIP());

				if((it->second).GetOnlineStatus() == HostInformation::OFFLINE) continue;
				
				if(mapVerifyComp.count((it->second).GetComponentName()) <= 0)
					mapVerifyComp[(it->second).GetComponentName()] = (it->second);
			}

			monitorFactoryResources.AddFactory(info);
		}
		// -- end verification --

		mapAliveCheck[info.GetHostName()] = timeCounter;
		mapHostProcessFactory[info.GetHostName()] = info;

		cout << "SystemManager::hostRegistration(): Factory " << info.GetHostName() << " with InterfaceHandle(" << info.GetHostIP() << "," << info.GetPortNumber() << ") is " << info.GetOnlineStatus() << endl;

		// -> create new proxy upon (re) connect
		getOrCreateFactoryProxy(info.GetHostIP(), info.GetPortNumber());
	}
	else // == HostInformation::COMPONENT
	{
		MapComponentInformation &mapComp = mapHostComponentInformation[info.GetHostName()];
		
		if(mapComp.count(info.GetComponentName()) <= 0) // -> happens when CreateComponent is called directly from a component
		{
			cout << "SystemManager::hostRegistration() WARNING! Received status update on " << info.GetComponentName() << " not started from this SystemManager!" << endl;
			if(info.GetStartDescription() == HostInformation::START)
				mapComponentNameStarts[info.GetComponentName()]++;
		
			if(mapHostProcessFactory.count(info.GetHostName()) > 0)
				info.SetHostIP(mapHostProcessFactory[info.GetHostName()].GetHostIP());
			else
				cout << "SystemManager::hostRegistration() WARNING! Factory on " << info.GetHostName() << " is not registered!" << endl;
		}

		mapComp[info.GetComponentName()] = info;
		HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
	
		if(info.GetStartDescription() == HostInformation::NO_START_DESCRIPTION)
		{
			cout << "SystemManager::hostRegistration(): WARNING! " << info.GetComponentName() << " has NO_START_DESCRIPTION!" << endl;
			cout << info << endl;
		}

		cout << "SystemManager::hostRegistration(): Component " << info.GetComponentName() << " on host " << info.GetHostName() << " is " << info.GetOnlineStatus() << endl;
	}
	
	return info;
}

// -------------------------------------------------------
//	functions to kill/start/"get-info about" components
// -------------------------------------------------------
//InterfaceHandle SystemManager::GetSettingsManagerHandle()
//{
//	MutexLocker lock(&updateLock);
//	return InterfaceHandle(settingsManagerHandle.GetPortNumber(), settingsManagerHandle.GetHostIP());
//}

void SystemManager::SetResourceInformation(HostInformation &factoryInfo, map<int, HostResources> &mapHostResources)
{
	MutexLocker lock(&updateLock);
	monitorFactoryResources.SetHostResources(factoryInfo, mapHostResources);
}

InterfaceHandle SystemManager::GetLookupServerHandle()
{
	MutexLocker lock(&updateLock);
	return InterfaceHandle(lookupServerInfo.GetPortNumber(), lookupServerInfo.GetHostIP());
}

void SystemManager::KillAll(string &returnMsg)
{
	stringstream errorMsg;
	cmdKillAllComponents(errorMsg);
	returnMsg = errorMsg.str();
}

int SystemManager::ClearAndKillAll(string &returnMsg)
{
	stringstream errorMsg;
	
	cmdKillAllComponents(errorMsg);
	int ret = cmdClearDataBase(errorMsg);
	
	returnMsg = errorMsg.str();
	return ret;
}

HostInformation SystemManager::GetHostInformation(string componentName)
{
	MutexLocker Lock(&updateLock);

	for(MapHostComponentInformation::const_iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		const MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::const_iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			const HostInformation &info = it_in->second;
			if(info.GetComponentName() == componentName)
				return info;
		}
	}

	for(MapComponentInformation::const_iterator it = mapHostProcessFactory.begin(), it_end = mapHostProcessFactory.end(); it != it_end; ++it)
	{
		const HostInformation &info = it->second;
		if(info.GetComponentName() == componentName)
			return info;
	}

	return HostInformation();
}

int SystemManager::KillComponent(string componentName, string &returnMsg)
{
	if(cmdKillComponent(componentName))
		return 1;

	return 0;
}

map<string, HostInformation> SystemManager::ParseXmlFile(string xmlCommands, string &returnMsg)
{
	stringstream xmlFileStream;
	xmlFileStream << "xmlCommands" << rand() << ".xml";
	string xmlFile(xmlFileStream.str());
	
	ofstream ofile(xmlFile.c_str());	// -> creates temporary file
	ofile << xmlCommands ;
	ofile.close();
	
	MapHostComponentInformation mapStartComponents;
	
	stringstream errorMessages;
	errorMessages << "Error messages: " << endl;

	int foundErrorsInXml = cmdParseStartupScript(errorMessages, mapStartComponents, xmlFile);
	remove(xmlFile.c_str()); // -> removes temporary file from disc
	
	MapComponentInformation mapHostInfo;
	if(foundErrorsInXml > 0)
	{
		errorMessages << "File could not be loaded due to " << foundErrorsInXml << " errors." << endl;
		HostInformation info;
		info.SetArgumentList(errorMessages.str());
		mapHostInfo.insert(pair<string, HostInformation>(string("ERROR"), info));
		returnMsg = errorMessages.str();
	}
	else
	{
		for(MapHostComponentInformation::const_iterator it = mapStartComponents.begin(), it_end = mapStartComponents.end(); it != it_end; ++it)
		{
			const MapComponentInformation &mapComp = it->second;
			for(MapComponentInformation::const_iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
				mapHostInfo.insert(pair<string, HostInformation>(it_in->first, it_in->second));
		}
	}

	return mapHostInfo;
}

int SystemManager::RestartDefault(string &returnMsg)
{
	MutexLocker Lock(&updateLock);

	cout << "SystemManager::RestartDefault(): Killing all components!" << endl;
	stringstream errorMsg;
	cmdKillAllComponents(errorMsg);

	cout << "SystemManager::RestartDefault(): Clearing SystemManager's database!" << endl;
	cmdClearDataBase(errorMsg);

	if(!SystemManagerConfig::initialConfigurationFile.empty())
	{
		cout << "SystemManager::RestartDefault(): Waiting 7 seconds for services in LookupServer to go OFFLINE........." << endl;
		msleep(7000);
	}

	cout << "SystemManager::RestartDefault(): Starting all components in " << SystemManagerConfig::initialConfigurationFile << endl;
	int errNum = cmdStartComponents(errorMsg, SystemManagerConfig::initialConfigurationFile);
	returnMsg = errorMsg.str();
	
	return errNum;
}

// -------------------------------------------------------
//  loads components to database
// -------------------------------------------------------
int SystemManager::StartComponents(string xmlCommands, map<string, string> &mapErrorMessages)
{
	stringstream xmlFileStream, errorMsg, systemManager;
	systemManager << HostInformation::SYSTEM_MANAGER ;

	xmlFileStream << "xmlCommands" << rand() << ".xml";
	string xmlFile(xmlFileStream.str());
	
	ofstream ofile(xmlFile.c_str());	// -> creates temporary file
	ofile << xmlCommands ;
	ofile.close();
	
	MapHostComponentInformation mapStartComponents;
	
	int foundErrorsInXml = cmdParseStartupScript(errorMsg, mapStartComponents, xmlFile);
	remove(xmlFile.c_str()); // -> removes temporary file from disc
	
	if(foundErrorsInXml > 0)
	{
		errorMsg << "File: " << xmlFile << " not loaded due to " << foundErrorsInXml << " errors." << endl;
		cout << errorMsg.str();
		mapErrorMessages.insert(pair<string, string>(systemManager.str(), errorMsg.str()));
		return foundErrorsInXml;
	}
	
	foundErrorsInXml = loadComponents(mapStartComponents, false, errorMsg);
	if(foundErrorsInXml > 0)
	{
		errorMsg << "File: " << xmlFile << " not loaded due to " << foundErrorsInXml << " errors." << endl;
		cout << errorMsg.str();
		mapErrorMessages.insert(pair<string, string>(systemManager.str(), errorMsg.str()));
		return foundErrorsInXml;
	}

	// -- xml commands did not contain errors, proceed to load and start --
	MutexLocker Lock(&updateLock);

	loadComponents(mapStartComponents, true, errorMsg);
	startAllComponents(mapStartComponents, errorMsg);
	updateSystemManagerOnlineHosts(mapStartComponents);

	// -- store error messages --
	if(!errorMsg.str().empty())
		mapErrorMessages.insert(pair<string, string>(systemManager.str(), errorMsg.str()));

	return foundErrorsInXml;
}

void SystemManager::updateSystemManagerOnlineHosts(const MapHostComponentInformation &mapStartedComponents)
{
	for(MapHostComponentInformation::const_iterator it = mapStartedComponents.begin(), it_end = mapStartedComponents.end(); it != it_end; ++it)
	{
		string host = it->first;
		string factoryIPAddress;

		if(this->mapHostProcessFactory.count(host) > 0)
			factoryIPAddress = mapHostProcessFactory[host].GetHostIP();
				
		const MapComponentInformation &mapCompNew = it->second;
		
		MapComponentInformation &mapComp = this->mapHostComponentInformation[host];

		for(MapComponentInformation::const_iterator it_in = mapCompNew.begin(), it_in_end = mapCompNew.end(); it_in != it_in_end; ++it_in)
		{
			HostInformation newHostInfo = it_in->second;
			if(!factoryIPAddress.empty())
				newHostInfo.SetHostIP(factoryIPAddress);

			HostInformation oldHostInfo = mapComp[newHostInfo.GetComponentName()];
			if(oldHostInfo.GetOnlineStatus() != HostInformation::ONLINE)
			{
				mapComp[newHostInfo.GetComponentName()] = newHostInfo;	
			}
		}
	}
}

int SystemManager::loadComponents(const MapHostComponentInformation &mapStartComponents, bool load, ostream &errorMsg)
{
	int foundErrorsInXml = 0;

	for(MapHostComponentInformation::const_iterator it = mapStartComponents.begin(), it_end = mapStartComponents.end(); it != it_end; ++it)
	{
		string host = it->first;
		string factoryIPAddress;

		if(this->mapHostProcessFactory.count(host) > 0)
			factoryIPAddress = mapHostProcessFactory[host].GetHostIP();
				
		const MapComponentInformation &mapCompNew = it->second;
		
		for(MapComponentInformation::const_iterator it_in = mapCompNew.begin(), it_in_end = mapCompNew.end(); it_in != it_in_end; ++it_in)
		{
			HostInformation newHostInfo = it_in->second;
			if(!factoryIPAddress.empty())
				newHostInfo.SetHostIP(factoryIPAddress);

			MapComponentInformation &mapComp = this->mapHostComponentInformation[host];
			if(load && mapComp.count(newHostInfo.GetComponentName()) >= 1)
			{
				HostInformation oldHostInfo = mapComp[newHostInfo.GetComponentName()];
				if(oldHostInfo.GetOnlineStatus() == HostInformation::ONLINE)
				{
					errorMsg << "WARNING! " << oldHostInfo.GetComponentName() << " not loaded. It is ONLINE." << endl;
					continue;
				}
				
				newHostInfo.SetOnlineStatus(oldHostInfo.GetOnlineStatus());
			}
			
			if(load) 
			{
				mapComp[newHostInfo.GetComponentName()] = newHostInfo;
				this->mapComponentNameStarts[newHostInfo.GetComponentName()] = 0;
			}
		}
	}

	return foundErrorsInXml;	
}

// -------------------------------------------------------
//  safe shut-down of the system manager
// -------------------------------------------------------
void SystemManager::ShutDown()
{
	Quit();
}

map<string, HostInformation> SystemManager::GetAllComponentInformation()
{
	MutexLocker lock(&updateLock);

	MapComponentInformation mapHostInfo;
	
	for(MapComponentInformation::const_iterator it = mapHostProcessFactory.begin(), it_end = mapHostProcessFactory.end(); it != it_end; ++it)
		mapHostInfo.insert(pair<string, HostInformation>(it->first, it->second));

	for(MapHostComponentInformation::const_iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		const MapComponentInformation mapComp = it->second;
		for(MapComponentInformation::const_iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
			mapHostInfo.insert(pair<string, HostInformation>(it_in->first, it_in->second));
	}

	return mapHostInfo;
}

map<long long, ExceptionMessage> SystemManager::GetMiddlewareExceptions(long long &lastTimeStamp)
{
	return this->exceptionMonitor.GetMiddlewareExceptions(lastTimeStamp);
}

map<string, HostResources> SystemManager::GetMapResourceInformation()
{
	return this->monitorFactoryResources.GetMapAllResources();
}

// -------------------------------------------------------
//	start the components by contacting the component factories
// -------------------------------------------------------
bool SystemManager::startComponentsOnFactory(const HostInformation &factoryInfo, MapComponentInformation &mapComp, ostream &errorMsg)
{
	// -- start check --
	if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE)
	{
		//cerr << "startComponentsOnFactory(): WARNING! Factory on " << factoryInfo.GetHostName() << " is OFFLINE " << factoryInfo << endl;
		return false;
	}
	// -- end check --

	ProcessFactoryProxy *proxy = this->getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
	if(proxy == NULL) return false;

	for(MapComponentInformation::iterator it = mapComp.begin(), it_end = mapComp.end(); it != it_end; ++it)
	{
		// -- really start this component?
		HostInformation &info = (it->second);
		if(info.GetOnlineStatus() == HostInformation::ONLINE) // already online
		{
			if(this->mapComponentNameStarts[info.GetComponentName()] == 0)
				this->mapComponentNameStarts[info.GetComponentName()]++;
			continue;
		}
		// online status is still being verified?
		else if(info.GetOnlineStatus() == HostInformation::NO_ONLINE_STATUS)
			continue;
		// START component, but do not restart if already started once
		else if(info.GetStartDescription() == HostInformation::START && mapComponentNameStarts[info.GetComponentName()] > 0)
			continue;
		else if(info.GetStartDescription() == HostInformation::STOP)
			continue;
		else if(info.GetStartDescription() == HostInformation::NO_START_DESCRIPTION)
			continue;

		this->mapComponentNameStarts[info.GetComponentName()]++;
		
		// Send entire HostInformation to factory, and receive updated host information back
		errorMsg << this->startComponentOnFactory(proxy, info);

		if(info.GetOnlineStatus() == HostInformation::OFFLINE)
			info.SetStartDescription(HostInformation::STOP);
	}

	return true;
}

string SystemManager::startComponentOnFactory(ProcessFactoryProxy *proxy, HostInformation &info)
{
	string errorMsg;
	try
	{
		errorMsg = proxy->CreateComponent(info, lookupServerInfo);
	}
	catch(...)
	{
		stringstream msg;
		msg << "startComponentOnFactory(): ERROR! factory is unreachable " << proxy->GetInterfaceHandle() << endl;
		cout << msg.str();
		return msg.str();
	}

	if(info.GetProcessId() > 0)
		cout << "SUCCESS! " << info << endl;
	else
		cout << "ERROR! " << info << endl;

	HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
	return errorMsg;
}

map<string, map<string, HostInformation> >
SystemManager::startAllComponents(MapHostComponentInformation &mapStartComponents, ostream &errorMsg)
{
	MapHostComponentInformation mapStartedComponents; // components that were started
	for(MapHostComponentInformation::iterator it = mapStartComponents.begin(), it_end = mapStartComponents.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) <= 0)
		{
			errorMsg << "ERROR! SystemManager has NO information about factory on " << it->first << endl;
			continue;
		}

		HostInformation factoryInfo = mapHostProcessFactory[it->first];
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE)
		{
			errorMsg << "WARNING! Factory OFFLINE on " << factoryInfo.GetHostName() << endl;
			continue;
		}
		
		// -- start the components on factoryInfo.GetHostName() --
		stringstream factoryMsg;
		MapComponentInformation &mapComp = it->second;
		if(startComponentsOnFactory(factoryInfo, mapComp, factoryMsg))
			mapStartedComponents.insert(*it);

		// in case of error messages from factory store them
		if(!factoryMsg.str().empty())
		{
			ExceptionMessage msg(factoryInfo, factoryMsg.str(), GetTimeStamp());
			exceptionMonitor.PostException(msg);
		}
	}

	return mapStartedComponents;
}

// -------------------------------------------------------
//	gets or creates a proxy to a single factory
// -------------------------------------------------------

#if 0
ProcessFactoryProxy* SystemManager::getOrCreateFactoryProxy(string factoryHost, int factoryPort)
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


#else

// TODO: Use function proxy->SetInterfaceHandle(factoryPort, factoryHost);
ProcessFactoryProxy* SystemManager::getOrCreateFactoryProxy(string factoryHost, int factoryPort)
{
	// Create factory proxy
	if(factoryHost.empty() || factoryPort <= -1) return NULL;

	ProcessFactoryProxy *proxy = NULL;
	if(mapHostProcessFactoryProxy.count(factoryHost) >= 1)
	{
		proxy = mapHostProcessFactoryProxy[factoryHost];
	}
	
	bool createNewProxy = false;
	if(proxy != NULL) if(!proxy->IsConnected()) createNewProxy = true;

	if(proxy == NULL || createNewProxy)
	{
		try
		{
			//if(proxy != NULL) 
			//{
			//	proxy->StartOrRestartClient(InterfaceHandle(factoryHost, factoryPort));
			//}
			//else 
			if(proxy == NULL)
			{			
				proxy = new ProcessFactoryProxy(InterfaceHandle(factoryPort, factoryHost), true);
				mapHostProcessFactoryProxy[factoryHost] = proxy;
			}
			
			//cout << "createFactoryProxy(): Established contact to factory (" << factoryHost << "," << factoryPort << ")" << endl;
		}
		catch(...)
		{
			//cout << "getOrCreateFactoryProxy(): WARNING! Unable to contact factory (" << factoryHost << "," << factoryPort << ")" << endl;
			mapHostProcessFactoryProxy[factoryHost] = NULL;
			return NULL;
		}
	}

	//if(proxy == NULL)
	//	cout << "getOrCreateFactoryProxy(): WARNING! Unable to contact factory (" << factoryHost << "," << factoryPort << ")" << endl;

	return proxy;
}
#endif

// -------------------------------------------------------
//		start ServiceManager
// -------------------------------------------------------
int SystemManager::cmdStartServiceManager(ostream &ostr)
{
	HostInformation hostInfo;

	hostInfo.SetComponentName("service.manager"); //MiddlewareSettings::GetServiceManagerService());
	hostInfo.SetHostName(MicroMiddleware::MiddlewareSettings::GetHostName());
	hostInfo.SetHostIP(NetworkFunctions::getIPAddress(hostInfo.GetHostName()));
	hostInfo.SetOnlineStatus(HostInformation::OFFLINE);
	hostInfo.SetHostDescription(HostInformation::LOOKUP_SERVER); //COMPONENT);
	hostInfo.SetStartDescription(HostInformation::RESTART);
	hostInfo.SetPortNumber(5000);
	hostInfo.SetExecutableName("ServiceManager");

	MapHostComponentInformation mapStartComponents;

	MapComponentInformation &mapInfo = mapStartComponents[hostInfo.GetHostName()];
	mapInfo.insert(pair<string, HostInformation>(IntactSettings::GetServiceManagerService(), hostInfo));

	int foundErrorsInXml = loadComponents(mapStartComponents, false, ostr);
	if(foundErrorsInXml > 0)
	{
		ostr << "ServiceManager not started due to " << foundErrorsInXml << " errors." << endl;
		return foundErrorsInXml;
	}

	MutexLocker Lock(&updateLock);

	loadComponents(mapStartComponents, true, ostr);

	lookupServerInfo = hostInfo;
	return 0;
}

// -------------------------------------------------------
// Parse startup script.xml
//		- List of Component Factories (port, host)
//		- List of processes/components to execute on the component factories
// -------------------------------------------------------
int SystemManager::cmdStartComponents(ostream &ostr, string xmlFile)
{
	MapHostComponentInformation mapStartComponents;
	
	int foundErrorsInXml = cmdParseStartupScript(ostr, mapStartComponents, xmlFile);
	if(foundErrorsInXml > 0)
	{
		ostr << "File: " << xmlFile << " not loaded due to " << foundErrorsInXml << " errors." << endl;
		return foundErrorsInXml;
	}
	
	foundErrorsInXml = loadComponents(mapStartComponents, false, ostr);
	if(foundErrorsInXml > 0)
	{
		ostr << "File: " << xmlFile << " not loaded due to " << foundErrorsInXml << " errors." << endl;
		return foundErrorsInXml;
	}

	MutexLocker Lock(&updateLock);

	loadComponents(mapStartComponents, true, ostr);

	return 0;	
}

int SystemManager::cmdParseStartupScript(ostream &ostr, MapHostComponentInformation &mapHostComponents, string xmlFile)
{
	MutexLocker Lock(&updateLock);

	// -- check that fileName is a file --
	ifstream ifile;
	ifile.open(xmlFile.c_str(), fstream::in);
	if(!ifile.is_open()) 
	{
		ostr << "cmdParseStartupScript(xmlFile): " << xmlFile << " not found" << endl;
		return 1;
	}
	ifile.close();
	// -- end check --

	int foundErrorsInXml = 0;
	SystemManagerConfig::systemConfigurationFile = xmlFile;

	try
	{
		ConfigurationFile configFile(xmlFile);

		//foundErrorsInXml += parseConfigFilePath(ostr, mapHostComponents, configFile, "SystemManager/SettingsManager", HostInformation::SETTINGS_MANAGER);
		foundErrorsInXml += parseConfigFilePath(ostr, mapHostComponents, configFile, "SystemManager/LookupServer", HostInformation::LOOKUP_SERVER);
		foundErrorsInXml += parseConfigFilePath(ostr, mapHostComponents, configFile, "SystemManager/Factory", HostInformation::COMPONENT_FACTORY);
		foundErrorsInXml += parseConfigFilePath(ostr, mapHostComponents, configFile, "SystemManager/Component", HostInformation::COMPONENT);
	}
	catch(...)
	{
		ostr << "cmdParseStartupScript(): ERROR! Error occured while parsing " << xmlFile << endl;
	}

	return foundErrorsInXml;
}

int SystemManager::parseConfigFilePath(ostream &ostr, MapHostComponentInformation &mapHostComponents, const ConfigurationFile &configFile, string path, HostInformation::HostDescription hostDescription)
{
	int foundErrorsInXml = 0;

	IntactInfra::ConfigurationNodeList lst = configFile.getConfigurationNodes(path);
    
	//IDEBUG() << " node list " << lst.size() << endl;

	for(IntactInfra::ConfigurationNodeList::iterator it = lst.begin(); it != lst.end(); it++)
	{
		IntactInfra::ConfigurationNode &node = *it;

		// -- init host information --
		HostInformation info;
		info.SetHostDescription(hostDescription);
		if(node.count("name"))		info.SetComponentName(node["name"]);
		if(node.count("exec"))		info.SetExecutableName(node["exec"]);
		if(node.count("host"))		info.SetHostName(node["host"]);
		if(node.count("port"))		info.SetPortNumber(atoi((node["port"]).c_str()));
		if(node.count("multicast"))	info.SetMulticastAddress(node["multicast"]);
		if(node.count("args"))		info.SetArgumentList(node["args"]);
		if(node.count("start"))		
		{
			if(node["start"] == string("start"))
				info.SetStartDescription(HostInformation::START);
			else if(node["start"] == string("restart"))
				info.SetStartDescription(HostInformation::RESTART);
		}
		else
			info.SetStartDescription(HostInformation::START);
		
		// -- start check info --
		if(hostDescription == HostInformation::COMPONENT)
		{
			if(info.GetComponentName().empty()) 
			{
				foundErrorsInXml++;
				ostr << "ERROR! No component name given! Info:" << info << endl;
			}
			if(info.GetExecutableName().empty()) 
			{
				foundErrorsInXml++;
				ostr << "ERROR! No executable information given in " << info.GetComponentName() << endl;
			}
			if(info.GetHostName().empty()) 
			{
				foundErrorsInXml++;
				ostr << "ERROR! No host-name information given in " << info.GetComponentName() << endl;
			}
		}
		// -- end check info --

		// -- insert host information --
		if(hostDescription == HostInformation::LOOKUP_SERVER)
		{
			if(info.GetHostName().empty())
			{
				foundErrorsInXml++;
				ostr << "ERROR! No HostName given for LookupServer."<< endl;
			}
			else
			{
				if(info.GetComponentName().empty()) info.SetComponentName(info.GetHostName());
				info.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);
				lookupServerInfo = info;
				lookupServerInfo.SetHostIP(NetworkFunctions::getIPAddress(lookupServerInfo.GetHostName()));
				
				HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
			}

			continue;
		}
		/*else if(hostDescription == HostInformation::SETTINGS_MANAGER)
		{
			if(info.GetHostName().empty())
			{
				foundErrorsInXml++;
				ostr << "ERROR! No HostName given for SettingsManager!"<< endl;
				continue;
			}
			if(info.GetComponentName().empty()) info.SetComponentName(info.GetHostName());
			//info.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);
			
			settingsManagerInfo = info;
			settingsManagerInfo.SetHostIP(NetworkFunctions::GetIPAddress(settingsManagerInfo.GetHostName()));
			
			HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
		}*/

		MapComponentInformation &mapInfo = mapHostComponents[info.GetHostName()];
		if(mapInfo.count(info.GetComponentName()))
		{
			foundErrorsInXml++;
			ostr << "ERROR! Component already inserted: " << info.GetComponentName() << endl;
		}
				
		if(hostDescription == HostInformation::COMPONENT_FACTORY)
		{
			HostInformation::OnlineStatus onlineStatus = HostInformation::OFFLINE;
			if(mapHostProcessFactory.count(info.GetHostName()) > 0)
			{
				onlineStatus = mapHostProcessFactory[info.GetHostName()].GetOnlineStatus();
			}
			
			if(onlineStatus == HostInformation::OFFLINE)
			{
				if(info.GetComponentName().empty()) info.SetComponentName(info.GetHostName());
				mapHostProcessFactory[info.GetHostName()] = info;
			}
		}
		else // == HostInformation::COMPONENT
		{
			if(mapHostProcessFactory.count(info.GetHostName()) > 0)
				info.SetHostIP(mapHostProcessFactory[info.GetHostName()].GetHostIP());

			mapInfo[info.GetComponentName()] = info;	
		}
	}

	return foundErrorsInXml;
}

// -------------------------------------------------------
// Parse componentInformationFile:
//	- If SystemManager crashes, its database is
//		stored in componentInformationFile
// -------------------------------------------------------
void SystemManager::parseHostInformationFromDisc(string &filename)
{
	vector<HostInformation> vectorHostInformation;
	if(!HostInformationSpace::ParseHostInformationFromDisc(filename, vectorHostInformation))
		return;
	
	while(!vectorHostInformation.empty())
	{
		HostInformation info = vectorHostInformation.back();
		vectorHostInformation.pop_back();

		// -- start check information --
		try
		{
			if(info.GetComponentName().empty()) throw MiddlewareException("ComponentName is empty!");
			if(info.GetHostName().empty()) throw MiddlewareException("HostName is empty!");
		}
		catch(IOException io)
		{
			cout << "parseHostInformationFromDisc(filename): ERROR in HostInformation! " << io.what() << endl;
			continue;
		}
		// -- end check information --

		if(info.GetHostDescription() == HostInformation::LOOKUP_SERVER)
		{
			lookupServerInfo = info;
			lookupServerInfo.SetHostIP(NetworkFunctions::getIPAddress(lookupServerInfo.GetHostName()));
			lookupServerInfo.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);
			continue;
		}
		/*else if(info.GetHostDescription() == HostInformation::SETTINGS_MANAGER)
		{
			settingsManagerInfo = info;
			settingsManagerInfo.SetHostIP(NetworkFunctions::getIPAddress(settingsManagerInfo.GetHostName()));
			settingsManagerInfo.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);
			//continue;
		}*/

		if(mapHostComponentInformation.count(info.GetHostName()) <= 0)
			mapHostComponentInformation[info.GetHostName()] = MapComponentInformation();
		
		MapComponentInformation &mapComp = mapHostComponentInformation[info.GetHostName()];
		if(mapComp.count(info.GetComponentName()) <= 0)
		{
			// uncertain whether component is ONLINE or OFFLINE, verified later
			info.SetOnlineStatus(HostInformation::NO_ONLINE_STATUS);

			//cerr << "parseHostInformationFromDisc(): Info from disc: " << info << endl;
			mapComp[info.GetComponentName()] = info;
		}
	}
}

// -------------------------------------------------------
//	kill functions: 
//	 - Controlled from command interface, or a proxy
// -------------------------------------------------------
void SystemManager::cmdKillAllComponents(std::ostream &errorMsg)
{
	MutexLocker Lock(&updateLock);

	for(MapComponentInformation::iterator it = mapHostProcessFactory.begin(), it_end = mapHostProcessFactory.end(); it != it_end; ++it)
	{
		HostInformation factoryInfo = it->second;
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE) continue;

		ProcessFactoryProxy *proxy = getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
		// -- start check --
		if(proxy == NULL)
		{
			errorMsg << "killAllComponents(): ERROR! Couldn't create proxy-connection to the factory on host: " << factoryInfo.GetHostName() << endl;
			if(errorMsg.rdbuf() != std::cout.rdbuf())
				std::cout << "killAllComponents(): ERROR! Couldn't create proxy-connection to the factory on host: " << factoryInfo.GetHostName() << endl;
			continue;
		}
		// -- end check --

		try
		{
			int ret = proxy->KillAll(); // ignore return value
			cout << "SystemManager::cmdKillAllComponents() : Killed all components on factory : " << factoryInfo.GetHostName() << endl;
		}
		catch(...)
		{
			errorMsg << "cmdKillAllComponents(): ERROR! Factory " << factoryInfo.GetHostName() << " unreachable!" << endl;
			if(errorMsg.rdbuf() != std::cout.rdbuf())
				std::cout << "cmdKillAllComponents(): ERROR! Factory " << factoryInfo.GetHostName() << " unreachable!" << endl;
			continue;
		}

		// update online status
		MapComponentInformation &mapComp = mapHostComponentInformation[it->first];
		for(MapComponentInformation::iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			it_in->second.SetOnlineStatus(HostInformation::OFFLINE);
			it_in->second.SetProcessId(-1);

			if(it_in->second.GetStartDescription() != HostInformation::NO_START_DESCRIPTION)
			{
				it_in->second.SetStartDescription(HostInformation::STOP);
				HostInformationSpace::WriteHostInformationToDisc(outComponentFile, it_in->second);
			}
		}
	}
}

bool SystemManager::cmdKillComponent(string componentName)
{
	MutexLocker Lock(&updateLock);

	for(MapHostComponentInformation::iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) <= 0) continue;

		HostInformation factoryInfo = mapHostProcessFactory[it->first];
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE) continue;
		
		MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			HostInformation &info = it_in->second;
			if(info.GetComponentName() == componentName)
			{
				if(info.GetOnlineStatus() == HostInformation::OFFLINE) 
				{
					std::cout << "Component " << info.GetComponentName() << " is currently " << info.GetOnlineStatus() << endl;
					return false;
				}
				
				ProcessFactoryProxy *proxy = getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
				// -- start check --
				if(proxy == NULL)
				{
					std::cout << "cmdKillComponent(pid): ERROR! Couldn't create proxy-connection to the factory on host: " << factoryInfo.GetHostName() << endl;
					return false;
				}
				// -- end check --

				try
				{
					std::cout << "Killing component: " << info << endl;
					int ret = proxy->KillComponent(info.GetComponentName()); // ignore return value
					info.SetOnlineStatus(HostInformation::OFFLINE);
					info.SetProcessId(-1);
					if(info.GetStartDescription() != HostInformation::NO_START_DESCRIPTION)
					{
						info.SetStartDescription(HostInformation::STOP);			
						HostInformationSpace::WriteHostInformationToDisc(outComponentFile, info);
					}
				}
				catch(...)
				{
					std::cout << "cmdKillComponent(): ERROR! Factory " << info.GetHostName() << " unreachable!" << std::endl;
					return false;
				}
				return true;
			}
		}
	}
	return false;
}

// -------------------------------------------------------
//	- After a kill this function clears the system-
//	manager's database
// -------------------------------------------------------
int SystemManager::cmdClearDataBase(std::ostream &errorMsg)
{
	MutexLocker Lock(&updateLock);

	bool okToClearCache = true;

	// -> First Verify that it is safe to clear cache!
	MapComponentInformation mapRunningComponents;
	for(MapHostComponentInformation::iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		ProcessFactoryProxy *factoryProxy = NULL;
		if(mapHostProcessFactory.count(it->first) > 0)
		{
			const HostInformation &factoryInfo = mapHostProcessFactory[it->first];
			ProcessFactoryProxy *factoryProxy = getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
		}

		MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			const HostInformation &componentInfo = it_in->second;

			if(componentInfo.GetOnlineStatus() == HostInformation::ONLINE && factoryProxy == NULL)
			{
				okToClearCache = false;
				mapRunningComponents.insert(pair<string, HostInformation>(componentInfo.GetComponentName(), componentInfo));
				
				if(mapHostProcessFactory.count(it->first) > 0)
				{
					const HostInformation &factoryInfo = mapHostProcessFactory[it->first];
					mapRunningComponents.insert(pair<string, HostInformation>(factoryInfo.GetHostName(), factoryInfo));
				}
			}
		}
	}

	if(!okToClearCache)
	{
		errorMsg << "WARNING! Could not clear cache! Some components may still be running on OFFLINE Component Factory: " << endl;
		for(MapComponentInformation::iterator it_in = mapRunningComponents.begin(), it_in_end = mapRunningComponents.end(); it_in != it_in_end; ++it_in)		
		{
			if(it_in->second.GetHostDescription() == HostInformation::COMPONENT_FACTORY)
				errorMsg << "Factory: " ;
			else 
				errorMsg << "Comp: " ;
			
			errorMsg << it_in->second << endl;
		}
		if(std::cout.rdbuf() == errorMsg.rdbuf())
			std::cout << errorMsg.rdbuf();
		return 0;
	}
	else
	{
		this->clearComponentInformationFile();
		mapHostComponentInformation.clear();
		verifyComponentsAreRunning.clear();
		this->exceptionMonitor.ClearAll();
	}
	
	return 1;
}

// -------------------------------------------------------
//		print system manager's data-base
// -------------------------------------------------------
void SystemManager::cmdPrintHostInformation(ostream& ostr)
{
	MutexLocker Lock(&updateLock);

	ostr << "====================================================================================" << endl;
	if(!lookupServerInfo.GetHostName().empty())
	{
		ostr << "-------------------------- Lookup Server Information -------------------------------" << endl;
		ostr << this->lookupServerInfo << endl;
	}
	else ostr << "No Lookup Server information Available." << endl;

	if(!settingsManagerInfo.GetHostName().empty())
	{
		ostr << "-------------------------- SettingsManager Information -----------------------------" << endl;
		ostr << this->settingsManagerInfo << endl;
	}
	else ostr << "No SettingsManager information Available." << endl;
	
	ostr << endl;

	if(mapHostComponentInformation.empty())
	{
		ostr << "Component database is empty. Printing factory information." << endl;
		ostr << "-------------------------- Factories OFFLINE ---------------------------------------" << endl;
		printFactoryInformation(ostr, HostInformation::OFFLINE);

		ostr << endl;
		ostr << "-------------------------- Factories ONLINE ---------------------------------------" << endl;
		printFactoryInformation(ostr, HostInformation::ONLINE);
	}
	else
	{
		ostr << "-------------------------- Components OFFLINE --------------------------------------" << endl;
		printHostInformation(ostr, HostInformation::OFFLINE);
		
		ostr << endl;
		ostr << "-------------------------- Components ONLINE ---------------------------------------" << endl;
		printHostInformation(ostr, HostInformation::ONLINE);
	}
}

void SystemManager::printFactoryInformation(ostream &ostr, HostInformation::OnlineStatus status)
{
	for(MapComponentInformation::iterator it_in = mapHostProcessFactory.begin(), it_in_end = mapHostProcessFactory.end(); it_in != it_in_end; ++it_in)
	{
		if(it_in->second.GetOnlineStatus() != status) continue;
		
		ostr << "Factory: " << it_in->second << endl;
	}
}

void SystemManager::printHostInformation(ostream &ostr, HostInformation::OnlineStatus status)
{
	for(MapHostComponentInformation::iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) > 0)
		{
			HostInformation factoryInfo = mapHostProcessFactory[it->first];
			ostr << "---------> " << factoryInfo.GetHostName() << endl;
			if(factoryInfo.GetOnlineStatus() == status)
			{
				if(factoryInfo.GetHostName().empty())
					ostr << "No factory information for host " << it->first << endl;
				else
					ostr << "Factory: " << factoryInfo << endl;
			}
		}
		else
			ostr << "No factory information for host " << it->first << endl;

		MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
		{
			if(it_in->second.GetOnlineStatus() != status) continue;

			ostr << "Comp: " << it_in->second << endl;
		}
	}
}

void SystemManager::cmdPrintAllHostInformation(ostream& ostr)
{
	MutexLocker Lock(&updateLock);

	ostr << "---------- HostInformation in SystemManager Database ----------" << endl;

	for(MapHostComponentInformation::iterator it = mapHostComponentInformation.begin(), it_end = mapHostComponentInformation.end(); it != it_end; ++it)
	{
		if(mapHostProcessFactory.count(it->first) > 0)
		{
			HostInformation factoryInfo = mapHostProcessFactory[it->first];
			ostr << "---------" << endl;
			if(factoryInfo.GetHostName().empty())
				ostr << "No factory information for host " << it->first << endl;
			else
				ostr << "Factory: " << factoryInfo << endl;
		}
		else
			ostr << "No factory information for host " << it->first << endl;

		MapComponentInformation &mapComp = it->second;
		for(MapComponentInformation::iterator it_in = mapComp.begin(), it_in_end = mapComp.end(); it_in != it_in_end; ++it_in)
			ostr << "Comp: " << it_in->second << endl;
	}
}
// -------------------------------------------------------
//	retrieves and prints host resources from all factories
// -------------------------------------------------------
void SystemManager::cmdMonitorResources()
{
	MutexLocker Lock(&updateLock);
	cout << "---------- HostResources in SystemManager Database ----------" << endl;

	for(MapComponentInformation::iterator it = mapHostProcessFactory.begin(), it_end = mapHostProcessFactory.end(); it != it_end; ++it)
	{
		HostInformation factoryInfo = it->second;
		if(factoryInfo.GetOnlineStatus() == HostInformation::OFFLINE) continue;

		ProcessFactoryProxy *proxy = getOrCreateFactoryProxy(factoryInfo.GetHostIP(), factoryInfo.GetPortNumber());
		// -- start check --
		if(proxy == NULL)
		{
			cout << "cmdMonitorResources(): ERROR! Couldn't create proxy-connection to the factory on host: " << factoryInfo.GetHostName() << endl;
			continue;
		}
		// -- end check --
		
		try
		{
			typedef map<int, HostResources> MapPidHostResources;
			MapPidHostResources mapPidHostResources = proxy->GetMapComponentResources();
			
			cout << "Host resources for " << factoryInfo.GetHostName() << endl;
			for(MapPidHostResources::const_iterator it = mapPidHostResources.begin(), it_end = mapPidHostResources.end(); it != it_end; ++it)
				cout << it->second << endl;	
		}
		catch(...)
		{
			cout << "cmdMonitorResources(): WARNING! Factory " << factoryInfo.GetHostIP() << " unreachable! " << endl;
			continue;
		}
	}
}

} // namespace SystemManagerSpace

