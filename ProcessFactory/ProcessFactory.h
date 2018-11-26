#ifndef ProcessFactory_h_IS_INCLUDED
#define ProcessFactory_h_IS_INCLUDED

#include "ProcessFactory/IncludeExtLibs.h"
#include "ProcessFactory/ResourceMonitor.h"

#include <list>
#include <map>

#ifdef USE_GCC
#include <sys/types.h>
#include <signal.h>
#endif

// -------------------------------------------------------
//			ProcessFactoryConfig
// 
//	- display        a string containing a unix-type display parameter, e.g. "blade0:0.0" used
//	                     for redirecting the display output to other hosts
//	- debugLevel     an integer indicating the level of debug printf's (normally = 0)
// -------------------------------------------------------
struct ProcessFactoryConfig
{
	static int factoryPort;
	static string display;
	static int debugLevel;

	static string configurationFile;
	static string systemManagerHost;
	static int systemManagerPort;
	static string networkInterfaceIP;
	static string debugTool;

	static HostInformation factoryInfo;

	static string binDir;
	static string logDir;
};

class NotifySystemManagerThread;

using namespace std;

namespace ProcessFactorySpace
{

class ProcessFactoryCommand;

// -------------------------------------------------------
//			class ProcessFactory
// -------------------------------------------------------
class ProcessFactory //: public Component
	: public ProcessFactoryInterface, public Thread
{
private:
	// -- start outdated variables --
	typedef map<int, string>				MapInstanceIdComponentName;
	MapInstanceIdComponentName				mapInstanceIdComponentName;
	int										privateInstanceId;
	// -- end outdated variables --

	typedef map<string, HostInformation>	MapComponentInformation;
	MapComponentInformation					mapComponentInformation;

	Mutex									guard;
	InterfaceHandle							systemManagerInterfaceHandle;
	InterfaceHandle							factoryInterfaceHandle;

	ResourceMonitor							resourceMonitor;
	NotifySystemManagerThread				*notifySystemManagerThread;
	ProcessFactoryCommand					*componentFactoryCommand;
	bool									runProcessFactory;

private:
	/**
	* Creates a process by executing executableName.
	*
	* @param executableName			Name of the executable file.
	* @param componentName			Name of the component to be started.
	* @param args					Command line arguments provided when executing executableName.
	*/
	int		createProcess(const string &executableName, const string &componentName, const string &args, stringstream &errorMsg);

	/**
	* Notifies the SystemManager when the OnlineStatus (ONLINE or OFFLINE) of a component changes. 
	*
	* @param info					A component's HostInformation.
	* @param status					OnlineStatus (ONLINE or OFFLINE) of the component. 
	*/
	void	notifySystemManager(HostInformation &info, HostInformation::OnlineStatus status);

#ifdef USE_GCC
	/**
	* Reaps (waits-out) Zombie processes that the ProcessFactory has forked but not waited for yet.
	*/
	void reapZombieProcesses();
#endif

	/** 
	* OUTDATED FUNCTION:
    * waits for the factory to receive the notification about 
    * the creation of the object.  If the notification message has
    * not been received within the specified timeOut a
    * MiddlewareException will be thrown.
    *
    * @param timeOut                the time out period in milliseconds
    * @param processId		        the process id of the object currently being created
    * @param componentName          the type of the object currently being created
    * @param instanceName           the name of the instance currently being created
    * @param hostName               the name of the computer hosting the object currently being created
    */
    InterfaceHandle getInterfaceHandle(int timeOut, int instanceId, int processId, const string& componentName, const string& instanceName, const string& hostName);

private:
	void cmdPrintHostInformation(ostream& ostr);

public: // Methods below are public since they are called from signal handler and main().
	void cmdQuit();
	void cmdKillAll();

public:
    /**
    * Creates a Component Factory.
    *
    * @param name           the name of this instance
    * @param factoryPort    the port number of this factory, i.e. the port number of the main factory stub
	* @param systemManager	InterfaceHandle to systemManager
    */
	ProcessFactory(const string& name, int factoryPort, InterfaceHandle systemManager);

	/**
	* Stops ProcessFactory.
	*/
	virtual ~ProcessFactory();

	/**
	* Allows ProcessFactoryCommand to access private functions in LookupServer.
	*/
	friend class ProcessFactoryCommand;

	/**
	* Main thread-loop of the ProcessFactory. 
	* It continouously checks the running state of components (ONLINE or OFFLINE),
	* and reports to the System Manager when it changes.
	*/
	virtual void run();

public:
    virtual inline InterfaceHandle GetInterfaceHandle() {	return factoryInterfaceHandle; }

public:
	// -- start outdated functions --
	virtual InterfaceHandle CreateComponent(const string& componentType, const string& instanceName);
	virtual void			ComponentCreated(const string& hostName, int mainStubPort, int processID, const string& instanceName, const string& processFileName, int instanceId);
	virtual InterfaceHandle GetComponent(const string& componentName, const string& instanceName);
	// -- end outdated functions --

	/**
	* Creates (executes) a component using the given HostInformation, and provides the component with information
	* on how to contact the LookupServer. This is called from SystemManager.
	*
	* @param info			The HostInformation includes all information that is needed to start the component.
	* @param lookupServer	Contains the host information for the LookupServer.
	* @return				String containing (potential) error messages.
	*/
	virtual string CreateComponent(HostInformation &info, HostInformation lookupServer);

	/**
	* Creates (executes) a component using the given HostInformation, and provides the component with information
	* on how to contact the LookupServer. This is called from components and is identical to CreateComponent, 
	* except it notifies the SystemManager if it is available.
	*
	* @param info			The HostInformation includes all information that is needed to start the component.
	* @param lookupServer	Contains the host information for the LookupServer.
	* @return				String containing (potential) error messages.
	*/
	virtual string CreateExtComponent(HostInformation &info, HostInformation lookupServer);

	/**
	* Kills (stops) a component identified by componentName.
	*
	* @param componentName	The name of the component to be killed (stopped).
	* @return				Returns 1 if component was killed, 0 if otherwise.
	*/
	virtual int				KillComponent(string componentName);

	/**
	* Kills (stops) all components that has been started by the ProcessFactory
	*
	* @return				Returns 1.
	*/
    virtual int				KillAll(); 
	
	/**
	* Checks if a component identified with HostInformation is running.
	*
	* @param componentInfo	HostInformation for a given component.
	* @return				Returns 1 if a component is running, 0 if not running or not found.
	*/	
	virtual int				IsComponentRunning(HostInformation componentInfo);

	/**
	* Retrieves resource information about a component identified by componentName.
	*
	* @param componentName	The component's name.
	* @return				HostResources with memory and cpu information regarding componentName.
	*/

	virtual HostResources	GetProcessResources(string componentName);

	/**
	* Retrieves resource information about the machine (node) the Component Factory is running.
	*
	* @return				HostResources with memory and cpu information regarding the machine (node).
	*/	
	virtual HostResources	GetNodeResources();

	/**
	* Retrieves all resource information the Component Factory has (includes node and components).
	*
	* @return				String with all resource information that can be "decoded" with HostResourceSpace::ParseHostResourcesFromString().
	*/	
	virtual string			GetAllComponentResources();

	/**
	* Returns a map (pid, HostResources) of all resource information on the ProcessFactory.
	*
	* @return map<int, HostResources>	Map of all resource information (pid, HostResources).
	*/
	virtual map<int, HostResources>	GetMapComponentResources();
};

// -------------------------------------------------------
//			class ProcessFactoryCommand
// -------------------------------------------------------
/**
* ProcessFactoryCommand is a simple command interface to the ProcessFactory
*/
class ProcessFactoryCommand : public Thread
{
public:
	/**
	* Uses a ProcessFactory reference to call ProcessFactory's class functions for print, quit, etc.
	*
	* @param cf		The ProcessFactory object.
	*/
	ProcessFactoryCommand(ProcessFactory &cf) : componentFactory(cf)
	{
		start();
	}
	/**
	* Empty destructor.
	*/
	~ProcessFactoryCommand() 
	{}

private:
	inline void printCommand()
	{
		cout << "ProcessFactory commands: print-info (p), quit-clean (q), stop-immediately (s), help (h)." << endl;
	}

	inline virtual void run()
	{
		printCommand();

		while(true)
		{
			string command;
			try
			{
				istream &ret = getline(cin, command);
				if(ret.eof() || ret.bad() || !ret.good())
				{
					throw string("End of file!");
					break;
				}
			}
			catch(...)
			{
				//cout << "ProcessFactoryCommand: WARNING! getline returned End of file! Stopping command line!" << endl;
				cout << "ProcessFactoryCommand::run(): WARNING! ProcessFactory user interface (UI) is off." << endl;
				break;
			}

			if(strncmp(command.c_str(), "quit", 1) == 0)
			{
				cout << "Do you want to quit and kill all components (k), or, just quit (q)? " ;
				string confirm;
				getline(cin, confirm);
				if(strncmp(confirm.c_str(), "quit", 1) == 0)
				{
					cout << "Quitting ProcessFactory without killing running components..." << endl;
					componentFactory.cmdQuit();
					break;
				}
				else if(strncmp(confirm.c_str(), "kill", 1) == 0)
				{
					cout << "Quitting ProcessFactory and killing running components..." << endl;
					componentFactory.cmdKillAll();
					msleep(2000);
					componentFactory.cmdQuit();
					break;
				}
				else printCommand();
			}
			else if(strncmp(command.c_str(), "print-info", 1) == 0)
			{
				componentFactory.cmdPrintHostInformation(cout);
			}
			else if(strncmp(command.c_str(), "help", 1) == 0)
			{
				printCommand();
			}
			else if(strncmp(command.c_str(), "stop-immediately", 1) == 0)
			{
				cout << "Stop-immediatly leaves all components started by this factory running! Really stop (y,n)? " << endl;
				string confirm;
				getline(cin, confirm);
				if(strncmp(confirm.c_str(), "yes", 1) == 0)
				{
					#ifdef USE_GCC
						kill(getpid(), SIGSEGV);
					#else
						_exit(0);
					#endif
				}
				else printCommand();
			}
			else if(!command.empty())
			{
				cout << "Command: " << command << " is not supported" << endl;
			}
		}
	}

private:
	ProcessFactory &componentFactory;
};

// -------------------------------------------------------
//	class used to notify system manager of dead processes
// -------------------------------------------------------
class NotifySystemManagerThread : public Thread
{
	typedef map<int, HostResources>			MapComponentResources;
public:
	NotifySystemManagerThread(InterfaceHandle handle) 
		: systemManagerInterfaceHandle(handle)
		, systemProxy(NULL)
		, newArrival(false)
		, stopNotifyThread(false)
	{
		start();
	}
	~NotifySystemManagerThread() {}

private:
	virtual void run()
	{
		list<HostInformation> localList;

		while(!stopNotifyThread)
		{
			guard.lock();
			//monitor.Enter();
				if(localList.empty() && listHostInformation.empty() && !newArrival)
					monitor.wait(&guard);
				
				if(stopNotifyThread)
				{
					guard.unlock();
					//monitor.Exit();
					break;
				}

				while(!listHostInformation.empty())
				{
					localList.push_back(listHostInformation.front());
					listHostInformation.pop_front();
				}
				
				newArrival = false;

				MapComponentResources sendMap = mapComponentResources;
			guard.unlock();
			//monitor.Exit();

			// start sending only local variables!
			this->sendHostRegistrations(localList);
			this->sendResourceMap(sendMap);
		}
	}

	// mutex in -> mutex out
	void sendHostRegistrations(list<HostInformation> &listHostInformation)
	{
		bool printOutError = true; // for printout-debugging only

		while(!listHostInformation.empty())
		{
			try
			{
				if(stopNotifyThread) break;

				HostInformation info = listHostInformation.front();
				
				SystemManagerProxy *systemProxy = getOrCreateProxy(systemManagerInterfaceHandle);
				if(systemProxy == NULL)
				{
					throw ("");
				}
				else if(!systemProxy->IsConnected())
				{
					throw("");
				}
				HostInformation retInfo = systemProxy->HostRegistration(info);
				listHostInformation.pop_front();
			}
			catch(...)
			{
				if(printOutError)
				{
					cout << "NotifySystemManagerThread::run(): Failed to contact SystemManager " << systemManagerInterfaceHandle << endl;
					printOutError = false;
				}
				msleep(500);
			}
		}
	}

	void sendResourceMap(MapComponentResources sendMap)
	{
		try
		{
			if(!sendMap.empty())
			{
				SystemManagerProxy *systemProxy = getOrCreateProxy(systemManagerInterfaceHandle);
				if(systemProxy == NULL)
				{
					throw("");
				}
				else if(!systemProxy->IsConnected())
				{
					throw("");
				}
				systemProxy->SetResourceInformation(ProcessFactoryConfig::factoryInfo, sendMap);
			}
		}
		catch(...)
		{
			//cout << "UpdateResourceThread::run(): Failed to contact SystemManager " << systemManagerInterfaceHandle << endl;
		}
	}

	SystemManagerProxy* getOrCreateProxy(InterfaceHandle systemManagerInterfaceHandle)
	{
		/*bool createNewProxy = false;
		if(systemProxy != NULL) if(systemProxy->IsConnected() == false) createNewProxy = true;

		if(systemProxy == NULL || createNewProxy)
		{
			try
			{
				if(systemProxy != NULL) delete systemProxy;
				
				systemProxy = new SystemManagerProxy(systemManagerInterfaceHandle, true);
			}
			catch(...)
			{
				//cout << "getOrCreateProxy(): WARNING! Unable to contact SystemManager " << systemManagerInterfaceHandle << endl;
				systemProxy = NULL;
				return NULL;
			}
		}

		//if(systemProxy == NULL)
		//	cout << "getOrCreateProxy(): WARNING! Unable to contact SystemManager " << systemManagerInterfaceHandle << endl;
		*/
		
		if(systemProxy == NULL) //delete systemProxy;
			systemProxy = new SystemManagerProxy(systemManagerInterfaceHandle, true);

		return systemProxy;
	}

public:

	inline void addAndWakeup(const HostInformation &info)
	{
		MutexLocker lock(&guard);
		//monitor.Enter();
		newArrival = true;
		listHostInformation.push_back(info);
		monitor.wakeAll();

		//monitor.NotifyAll();
		//monitor.Exit();
	}

	inline void addAndWakeup(const MapComponentResources &newMapComponentResources)
	{
		MutexLocker lock(&guard);
		//monitor.Enter();
		newArrival = true;
		mapComponentResources = newMapComponentResources;
		monitor.wakeAll();
		//monitor.NotifyAll();
		//monitor.Exit();
	}

	inline void stopThread(bool b)	{ stopNotifyThread = b; }

private:
	InterfaceHandle			systemManagerInterfaceHandle;
	SystemManagerProxy		*systemProxy;
	bool					newArrival;

	mutable Mutex			guard;
	WaitCondition			monitor;
	
	list<HostInformation>	listHostInformation;
	MapComponentResources	mapComponentResources;
	bool					stopNotifyThread;
};

} // namespace ProcessFactorySpace

#endif

