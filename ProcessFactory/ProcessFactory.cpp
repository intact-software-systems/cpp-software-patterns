#include "ProcessFactory/ProcessFactory.h"
#include "ProcessFactory/FactoryLibrary.h"

#ifdef USE_GCC
#else
#include<process.h>
#include<fcntl.h>
#include<time.h>
#include<windows.h>
#include<tlhelp32.h>
#endif

using namespace std;

#define RESOURCE_MONITOR_INTERVAL_MS 3000

#ifdef USE_GCC
#endif

// -------------------------------------------------------
//	initialize static struct ProcessFactoryConfig
// -------------------------------------------------------
int ProcessFactoryConfig::factoryPort = MicroMiddleware::MiddlewareSettings::GetProcessFactoryPort();
string ProcessFactoryConfig::display = "";
int ProcessFactoryConfig::debugLevel = 0;
string ProcessFactoryConfig::configurationFile = string("configurationFile.xml");
string ProcessFactoryConfig::systemManagerHost = string("");
int ProcessFactoryConfig::systemManagerPort = MicroMiddleware::MiddlewareSettings::GetSystemManagerPort();
string ProcessFactoryConfig::networkInterfaceIP = string("");
string ProcessFactoryConfig::debugTool = string("");

HostInformation ProcessFactoryConfig::factoryInfo = HostInformation();

#ifdef USE_GCC
string ProcessFactoryConfig::binDir  = string("./");
string ProcessFactoryConfig::logDir  = string("./");
#else
string ProcessFactoryConfig::binDir;
string ProcessFactoryConfig::logDir;
#endif

// -------------------------------------------------------
//		ProcessFactory implementation starts
// -------------------------------------------------------
namespace ProcessFactorySpace
{

// -------------------------------------------------------
//			class ProcessFactory
// -------------------------------------------------------
ProcessFactory::ProcessFactory(const string& name, int factoryPort, InterfaceHandle systemManager) 
	//: Component(name, false)
	: privateInstanceId(0)
	, systemManagerInterfaceHandle(systemManager)
	, factoryInterfaceHandle(factoryPort, MicroMiddleware::MiddlewareSettings::GetHostName()) 
	, resourceMonitor(MicroMiddleware::MiddlewareSettings::GetHostName(), RESOURCE_MONITOR_INTERVAL_MS)
	, notifySystemManagerThread(NULL)
	, componentFactoryCommand(NULL)
	, runProcessFactory(true)
{ 
#ifdef USE_GCC
	srand(Thread::currentThreadId()); //gettimeofday(NULL, NULL));
	signal(SIGCHLD, SIG_IGN);
#else
	srand(::GetCurrentTime());
#endif

	IDEBUG() << "System manager handle " << systemManagerInterfaceHandle;
	if(systemManagerInterfaceHandle.GetPortNumber() > 0 && !systemManagerInterfaceHandle.GetHostName().empty())
		notifySystemManagerThread = new NotifySystemManagerThread(systemManagerInterfaceHandle);

	resourceMonitor.SetNotifySystemManagerThread(notifySystemManagerThread);
	resourceMonitor.AddProcess(ProcessFactoryConfig::factoryInfo);
	
	componentFactoryCommand = new ProcessFactoryCommand(*this);
}

ProcessFactory::~ProcessFactory()
{ 
	if(notifySystemManagerThread != NULL)
		notifySystemManagerThread->stopThread(true);
}

// -------------------------------------------------------
//			update running objects table
//	- If a process has died, inform SystemManager
// -------------------------------------------------------
void ProcessFactory::run()
{
	bool removedItem = false;

    while(runProcessFactory)
	{
		if(!removedItem) 
			Thread::sleep(1);

		if(!runProcessFactory) break;

		removedItem = false;

		MutexLocker lock(&guard);

		#ifdef USE_GCC
			reapZombieProcesses();
		#endif

		for(MapComponentInformation::iterator it = mapComponentInformation.begin(), it_end = mapComponentInformation.end(); it != it_end; ++it)
		{
			HostInformation &info = it->second;

			if(info.GetProcessId() != -1)
			{
				int ret = 0; // Process state: Dead = 0, Alive = 1, Undefined State = all other values
			#ifdef USE_GCC
				int childStatus;
				int childPid = waitpid(info.GetProcessId(), &childStatus, WNOHANG);
				if(childPid == -1)
				{
					if(errno != ECHILD)
					{
						cout << "ERROR " << strerror(errno) << " waiting for child process: " << info << endl;
						ret = -1;
					}
					// With popen this is always true because it uses sh command, which forks a new process (id)
					else //if(errno == ECHILD) 
						ret = ProcessFactoryLibrary::isProcessRunning(info.GetProcessId());
				}
				else if(childPid == 0) // child not dead
					ret = 1;
			#else
				ret = ProcessFactoryLibrary::isProcessRunning(info.GetProcessId());
			#endif
				// -- check return value 'ret' --
				if(ret < 0 || ret > 1)
				{
					cout << "ProcessFactory::run(): ERROR while checking if process is running! Return value: " << ret << endl;
					continue;
				}
				else if(ret == 1)// process still running
					continue;
			}

			// -- Process is dead --
			info.SetOnlineStatus(HostInformation::OFFLINE);
			cout << "ProcessFactory::run(): DEAD! " << info << endl;

			#ifdef USE_GCC
				if(info.GetProcessId() > -1)
				{
					stringstream stream;
					stream << "/var/run/sonarproc/" << info.GetProcessId() << "-" << info.GetExecutableName() << "-" << info.GetComponentName();
					remove(stream.str().c_str());
				}
			#endif

			resourceMonitor.RemoveProcess(info);
			notifySystemManager(info, HostInformation::OFFLINE);

			mapComponentInformation.erase(it);			
			removedItem = true;
			break;
		}
    }
}

void ProcessFactory::notifySystemManager(HostInformation &info, HostInformation::OnlineStatus status)
{
	if(notifySystemManagerThread == NULL) return;

	info.SetOnlineStatus(status);
	if(info.GetOnlineStatus() == HostInformation::OFFLINE)
		info.SetProcessId(-1);
	if(info.GetStartDescription() == HostInformation::START)
		info.SetStartDescription(HostInformation::STOP);
	
	notifySystemManagerThread->addAndWakeup(info);
}

#ifdef USE_GCC
void ProcessFactory::reapZombieProcesses()
{
	int myProcessId = getpid();

	vector<int> childZombies = ProcessFactoryLibrary::Linux::findChildZombies(myProcessId);

	int childStatus;
	for(vector<int>::iterator it = childZombies.begin(), it_end = childZombies.end(); it != it_end; ++it)
	{
		int childPid = *it;
		
		int ret = waitpid(childPid, &childStatus, WNOHANG);
		if(ret == -1 && errno != ECHILD)
			IDEBUG() << "ERROR " << strerror(errno) << " waiting for zombie child process id " << childPid << " " << childStatus;
	}
}
#endif

// -------------------------------------------------------
//	Process/Node resources
// -------------------------------------------------------
HostResources ProcessFactory::GetProcessResources(string componentName)
{
	MutexLocker lock(&guard);

	if(mapComponentInformation.count(componentName) <= 0 || componentName.empty())
		return HostResources();

	const HostInformation &info = mapComponentInformation[componentName];
	
	if(info.GetProcessId() < 0) return HostResources();

	return resourceMonitor.GetResources(info.GetProcessId());
}

HostResources ProcessFactory::GetNodeResources()
{
	HostResources res = resourceMonitor.GetResources();
	res.SetHostName(factoryInterfaceHandle.GetHostName());
	return res;
}

string ProcessFactory::GetAllComponentResources()
{
	return resourceMonitor.GetStringAllResources();
}

map<int, HostResources> ProcessFactory::GetMapComponentResources()
{
	return resourceMonitor.GetMapAllResources();
}

// -------------------------------------------------------
//	Checks if the componentInfo.GetProcessId() is running
// -------------------------------------------------------
int	ProcessFactory::IsComponentRunning(HostInformation componentInfo)
{
	MutexLocker lock(&guard);

	int running = ProcessFactoryLibrary::isProcessRunning(componentInfo.GetProcessId(), componentInfo.GetExecutableName());
	if(running > 0)
	{
		cout << "Component " << componentInfo.GetComponentName() << " with process id " << componentInfo.GetProcessId() << " is running" << endl;
		if(mapComponentInformation.count(componentInfo.GetComponentName()) <= 0)
		{
			componentInfo.SetOnlineStatus(HostInformation::ONLINE);
			mapComponentInformation[componentInfo.GetComponentName()] = componentInfo;
			resourceMonitor.AddProcess(componentInfo);
		}
	}

	return running;
}

// -------------------------------------------------------
//			create component
// -------------------------------------------------------
InterfaceHandle ProcessFactory::CreateComponent(const string& componentName, const string& instanceName) 
{
	if(componentName.empty() || instanceName.empty())
		ExceptionMulticast::PostException("CreateComponent(componentName, instanceName): WARNING! Function called with empty strings! " + componentName + ", " + instanceName, MiddlewareException::CONFIGURATION_ERROR);

	guard.lock();
	
	// -- start is instanceName already running? --
	if(mapComponentInformation.count(instanceName) >= 1) 
	{
		const HostInformation &currentInfo = mapComponentInformation[instanceName];
		if(ProcessFactoryLibrary::isProcessRunning(currentInfo.GetProcessId()) == 1)
		{
			cerr << "CreateComponent(): WARNING! Component already started: " << currentInfo << endl;
			InterfaceHandle handle(currentInfo.GetPortNumber(), currentInfo.GetHostIP());
			
			guard.unlock();

			return handle; 
		}
	}
	// -- end check if already running --

	int componentsInstanceId = ++privateInstanceId;
	guard.unlock();

	// -- start arrange arguments --
	stringstream executableName;

	if(componentName.rfind(".exe") != string::npos)
		executableName << componentName ;
	else
		executableName << componentName << ".exe" ;

	stringstream args;
	if(!instanceName.empty())
		args << instanceName << " ";
	else 
		args << componentName << " ";
	
	args << componentsInstanceId << " " << factoryInterfaceHandle.GetPortNumber();
	// -- end arrange arguments --
	
	// -- new impl --
	HostInformation info(instanceName, factoryInterfaceHandle.GetHostName(), -1, HostInformation::COMPONENT, HostInformation::OFFLINE);
	info.SetExecutableName(executableName.str());
	if(!ProcessFactoryConfig::networkInterfaceIP.empty())
		info.SetHostIP(ProcessFactoryConfig::networkInterfaceIP);
	else
		info.SetHostIP(factoryInterfaceHandle.GetHostName());
	info.SetArgumentList(args.str());
	// -- new impl ends --

	stringstream executableWithPath;
	if(ProcessFactoryConfig::binDir.length() > 0)
		executableWithPath << ProcessFactoryConfig::binDir;
	executableWithPath << executableName.str();

	try
	{
		stringstream errorMsg;
		int processId = createProcess(executableWithPath.str(), instanceName, args.str(), errorMsg);
		info.SetProcessId(processId);

		// -- new impl starts --
		if(processId > -1)
		{
			#ifdef USE_GCC
				stringstream stream;
				stream << "/var/run/sonarproc/" << processId << "-" << executableName.str() << "-" << instanceName;
				ofstream ofstr(stream.str().c_str());
				ofstr << processId << endl;
				ofstr.close();
			#endif

			guard.lock();
				info.SetOnlineStatus(HostInformation::ONLINE);
				mapComponentInformation[info.GetComponentName()] = info;
				resourceMonitor.AddProcess(info);
				notifySystemManager(info, HostInformation::ONLINE);
			guard.unlock();

			cout << "CreateComponent(): SUCCESS! " << info << endl;
		
			return getInterfaceHandle(5000, componentsInstanceId, processId, componentName, instanceName, factoryInterfaceHandle.GetHostName());
		}
		else
		{
			info.SetOnlineStatus(HostInformation::OFFLINE);
			cout << "CreateComponent(): ERROR! " << info << endl;
		}
		// -- new impl ends --
	}
	catch(...)
	{
		info.SetProcessId(-1);
		info.SetOnlineStatus(HostInformation::OFFLINE);
		cout << "CreateComponent(): ERROR! " << info << endl;
	}

	return InterfaceHandle();
}
// -------------------------------------------------------
//		create component, called from external components
// -------------------------------------------------------
string ProcessFactory::CreateExtComponent(HostInformation &info, HostInformation lookupServer)
{
	info.SetHostDescription(HostInformation::COMPONENT);
	info.SetHostName(factoryInterfaceHandle.GetHostName());
	if(!ProcessFactoryConfig::networkInterfaceIP.empty())
		info.SetHostIP(ProcessFactoryConfig::networkInterfaceIP);
	else
		info.SetHostIP(factoryInterfaceHandle.GetHostName());
	info.SetStartDescription(HostInformation::NO_START_DESCRIPTION);
	info.SetOnlineStatus(HostInformation::OFFLINE);

	string errorMsg = CreateComponent(info, lookupServer);
	
	MutexLocker lock(&guard);
	notifySystemManager(info, info.GetOnlineStatus());
	return errorMsg;
}

// -------------------------------------------------------
//		create component, called from SystemManager
// -------------------------------------------------------
string ProcessFactory::CreateComponent(HostInformation &info, HostInformation lookupServer)
{
	// -- start error checking --
	if(info.GetComponentName().empty() || info.GetExecutableName().empty())
	{
		stringstream stream;
		stream << "CreateComponent(info, lookupServer): ERROR! Function called with empty strings!" << endl;
		stream << "HostInfo: " << info << endl << " LookupServer: " << lookupServer << endl;
		ExceptionMulticast::PostException(stream.str(), MiddlewareException::CONFIGURATION_ERROR);
		return stream.str();
	}
	if(info.GetStartDescription() == HostInformation::NO_START_DESCRIPTION)
		cout << "CreateComponent(): WARNING! NO_START_DESCRIPTION is given!" << endl;
	// -- end error checking --
	
	guard.lock();

	stringstream errorMsg;
	// -- Is component already started by factory, and still running? --
	if(mapComponentInformation.count(info.GetComponentName()) >= 1) 
	{
		const HostInformation &currentInfo = mapComponentInformation[info.GetComponentName()];
		if(ProcessFactoryLibrary::isProcessRunning(currentInfo.GetProcessId()) == 1)
		{
			errorMsg << "CreateComponent(): WARNING! Component already started: " << info << endl;
			cout << errorMsg.str();
			info = currentInfo;
			guard.unlock();
			
			return errorMsg.str();
		}
	}
	// -> factory's database is empty due to recent (re)start
	// -- see if system manager's process id checks out --
	else if(info.GetProcessId() != -1 && ProcessFactoryLibrary::isProcessRunning(info.GetProcessId(), info.GetExecutableName()) == 1) // true? -> component is running
	{
		info.SetOnlineStatus(HostInformation::ONLINE);
		mapComponentInformation[info.GetComponentName()] = info;

		errorMsg << "CreateComponent(): WARNING! Component already started: " << info << endl;
		cout << errorMsg.str();

		guard.unlock();
		return errorMsg.str();
	}

	guard.unlock();

	// -- create the new component process --
	try
	{
		stringstream arguments;
		arguments << PREFIX_HOST_INFO << " " << info << " " 
				  << PREFIX_INSTANCE_NAME << " " << info.GetComponentName() << " " 
				  << PREFIX_LOOKUP_HOST << " " << lookupServer.GetHostIP() << " " 
				  << PREFIX_LOOKUP_PORT << " " << lookupServer.GetPortNumber() << " ";

		//cout << "CreateComponent(): " << info.GetExecutableName() << " " << arguments.str() << endl;
		stringstream executableWithPath;
		if(ProcessFactoryConfig::binDir.length() > 0)
			executableWithPath << ProcessFactoryConfig::binDir;
		executableWithPath << info.GetExecutableName();

		int processId = createProcess(executableWithPath.str(), info.GetComponentName(), arguments.str(), errorMsg);
		info.SetProcessId(processId);
		
		if(processId == -1) // error
		{
			info.SetOnlineStatus(HostInformation::OFFLINE);
			errorMsg << "CreateComponent(): ERROR! " << info << endl;
			cout << errorMsg.str();
		}
		else // success
		{
			info.SetOnlineStatus(HostInformation::ONLINE);
			cout << "CreateComponent(): SUCCESS! " << info << endl;
			#ifdef USE_GCC
				stringstream stream;
				stream << "/var/run/sonarproc/" << processId << "-" << info.GetExecutableName() << "-" << info.GetComponentName();
				ofstream ofstr(stream.str().c_str());
				ofstr << processId << endl;
				ofstr.close();
			#endif
			
			guard.lock();
				mapComponentInformation[info.GetComponentName()] = info;
				resourceMonitor.AddProcess(info);
			guard.unlock();
		}
	}
	catch(...)
	{
		info.SetProcessId(-1);
		info.SetOnlineStatus(HostInformation::OFFLINE);
		
		errorMsg << "CreateComponent(): ERROR! " << info << endl;
		cout << errorMsg.str();
	}

	return errorMsg.str();
}

// -------------------------------------------------------
//			create process
// -------------------------------------------------------
int ProcessFactory::createProcess(const string &executableName, const string &componentName, const string &args, stringstream &errorMsg)
{
	int id = -1;
	stringstream command;

#ifdef USE_GCC
	// -- start check if executable exists --
	ifstream ifile(executableName.c_str());
	if(!ifile.is_open())
	{
		errorMsg << "ProcessFactory::createProcess(): ERROR! Couldn't find executable " << executableName << endl;
		ifile.close();
		return id;
	}
	ifile.close();
	// -- end check if executable exists --

	if(strcmp(ProcessFactoryConfig::display.c_str(), "noX") != 0)
	{
		id = ProcessFactoryLibrary::Linux::createProcessXtermForkExec(executableName, componentName, args);
	}
	else if(ProcessFactoryConfig::debugLevel > 0)
	{
		FILE *fd = NULL;
		id = ProcessFactoryLibrary::Linux::createProcessPopenReadPid(executableName, componentName, args, fd);
		if(id > 0 && fd != NULL) // NB! -> only need to close fd when I use signal(SIGCHLD, SIG_IGN)
			pclose(fd);
		//	ClosePipeThread *cpt = new ClosePipeThread(fd);	// self destructing
	}
	else
	{
		id = ProcessFactoryLibrary::Linux::createProcessForkExec(executableName, componentName, args);
	}
#else
	try
	{
		command << executableName << " " << args ;
		id = ProcessFactoryLibrary::Windows::createProcess(command.str(), componentName);
	}
	catch(MiddlewareException ex)
	{
		errorMsg << "CreateProcess(): ERROR! Is " << executableName << " located in the path?" << endl; 
	}
	catch(...)
	{
		errorMsg << "CreateProcess(): ERROR! Is " << executableName << " located in the path?" << endl; 
	}
#endif
	return id;
}

int ProcessFactory::KillAll() 
{
    MutexLocker singleLock(&guard);
	for(MapComponentInformation::iterator it = mapComponentInformation.begin(), it_end = mapComponentInformation.end(); it != it_end; ++it)
	{
		(it->second).SetStartDescription(HostInformation::STOP);
		resourceMonitor.RemoveProcess((it->second));

		try
		{
			ProcessFactoryLibrary::killProcess(it->second);
		}
		catch(...)
		{
			cout << "ProcessFactory::KillAll(): ERROR while Removing dead process : " << (it->second) << endl;
			continue;
		}
		
		cout << "ProcessFactory::KillAll(): Removing dead process : " << (it->second) << endl;
	}

	mapComponentInformation.clear();
	return 1;
}

int ProcessFactory::KillComponent(string componentName)
{
    MutexLocker singleLock(&guard);
    
	for(MapComponentInformation::iterator it = mapComponentInformation.begin(), it_end = mapComponentInformation.end(); it != it_end; ++it)
		if((it->second).GetComponentName() == componentName)
		{
			(it->second).SetStartDescription(HostInformation::STOP);
			ProcessFactoryLibrary::killProcess(it->second);
			
			cout << "ProcessFactory::KillComponent(): Removing dead process : " << (it->second) << endl;
			resourceMonitor.RemoveProcess((it->second));
			mapComponentInformation.erase(it);
			return 1;
		}

	return 0;
}

// -------------------------------------------------------
//			get component's interfacehandle
// -------------------------------------------------------
InterfaceHandle ProcessFactory::GetComponent(const string& componentName, const string& instanceName) 
{
    MutexLocker singleLock(&guard);
    
	for(MapComponentInformation::iterator it = mapComponentInformation.begin(), it_end = mapComponentInformation.end(); it != it_end; ++it)
		if((it->second).GetComponentName() == componentName || (it->second).GetComponentName() == instanceName)
			return InterfaceHandle((it->second).GetPortNumber(), (it->second).GetHostName());

    throw MiddlewareException("Could not find " + instanceName);

	return InterfaceHandle();
}

InterfaceHandle ProcessFactory::getInterfaceHandle(
    int timeOut,
	int instanceId,
    int processId, 
    const string& componentName, 
    const string& instanceName,
    const string& hostName) 
{
    int sleepInterval = 100;  // milliseconds
    int maxNumberOfTries = timeOut/sleepInterval;
    int numberOfTries = 0;

    while(numberOfTries < maxNumberOfTries) 
	{
        guard.lock();
		for(MapInstanceIdComponentName::iterator it = mapInstanceIdComponentName.begin(), it_end = mapInstanceIdComponentName.end(); it != it_end; ++it)
		{
			if((it->first) == instanceId)
			{
				if(mapComponentInformation.count(it->second) <= 0)
				{
					throw MiddlewareException("ProcessFactory::getInterfaceHandle(): Couldn't find the component " + it->second);
					//cout << "ProcessFactory::getInterfaceHandle(): Couldnt find the component " << it->second << endl;
				}

				const HostInformation &info = mapComponentInformation[it->second];
				InterfaceHandle handle = InterfaceHandle(info.GetPortNumber(), info.GetHostName());

				cout << "ProcessFactory::getInterfaceHandle(): Returning InterfaceHandle(" << handle << ")" << endl;
				mapInstanceIdComponentName.erase(it);

				guard.unlock();
				return handle;
			}
		}
		guard.unlock();

		int ret = ProcessFactoryLibrary::isProcessRunning(processId);
		if(ret < 1)
		{
			cout << "ProcessFactory::getInterfaceHandle(): DEAD! Process " << componentName << " " << instanceName << " is not running! " << endl;
			break;
		}
		
		numberOfTries++;            
        Thread::sleep(1); //sleepInterval);
    }

    stringstream ss;
	ss  << "ProcessFactory::getInterfaceHandle(): Could not create instance " 
        << instanceName << " of type " << componentName << " on " << hostName 
        << " within the given time limit (" << timeOut/1000.0 << " s).";

	cout << ss.str() << endl;

    throw MiddlewareException(ss.str());
}

// -------------------------------------------------------
//	outdated function to report back that a component has been created
// -------------------------------------------------------
void ProcessFactory::ComponentCreated(
    const string& hostName, 
    int mainStubPort, 
    int processId, 
    const string& componentName, 
    const string& processFileName,
	int instanceId)
{
	HostInformation info;
	info.SetComponentName(componentName);
	info.SetProcessId(processId);
	info.SetExecutableName(processFileName);
	info.SetHostName(hostName);
	info.SetPortNumber(mainStubPort);
	info.SetHostDescription(HostInformation::COMPONENT);
	info.SetOnlineStatus(HostInformation::ONLINE);
	if(!ProcessFactoryConfig::networkInterfaceIP.empty())
		info.SetHostIP(ProcessFactoryConfig::networkInterfaceIP);
	else
		info.SetHostIP(hostName);

    MutexLocker lock(&guard);
	
	mapInstanceIdComponentName[instanceId] = componentName;
	mapComponentInformation[componentName] = info;
	cout << "ProcessFactory::ComponentCreated(): " << componentName << " (port=" << mainStubPort << ", pid=" << processId << ")" << endl;
}

// -------------------------------------------------------
//	Functions used by ProcessFactory command UI
// -------------------------------------------------------
void ProcessFactory::cmdKillAll()
{
	MapComponentInformation tempMap = mapComponentInformation;

	this->KillAll();
	
	for(MapComponentInformation::iterator it = tempMap.begin(), it_end = tempMap.end(); it != it_end; ++it)
		this->notifySystemManager(it->second, HostInformation::OFFLINE);
}

void ProcessFactory::cmdQuit()
{
	MutexLocker lock(&guard);
	runProcessFactory = false;
}

// -------------------------------------------------------
//		print component factory's data-base
// -------------------------------------------------------
void ProcessFactory::cmdPrintHostInformation(ostream& ostr)
{
	MutexLocker Lock(&guard);

	ostr << "------------- Host Resources for Components on Factory " << factoryInterfaceHandle.GetHostName() << " --------------" << endl;
	ostr << resourceMonitor.GetStringAllResources();

	ostr << "----------------------- Components On Factory " << factoryInterfaceHandle.GetHostName() << " -----------------------" << endl;
	for(MapComponentInformation::iterator it = mapComponentInformation.begin(), it_end = mapComponentInformation.end(); it != it_end; ++it)
		ostr << it->second << endl;
}

}

