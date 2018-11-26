#ifndef SystemManager_SystemManger_h_IsIncluded
#define SystemManager_SystemManger_h_IsIncluded

#include"SystemManager/IncludeExtLibs.h"
#include"SystemManager/ResourceMonitor.h"
#include"SystemManager/ExceptionMonitor.h"

#include<map>
#include<fstream>

namespace SystemManagerSpace
{
// --------------------------------------------------------
//			struct SystemManagerConfig
// --------------------------------------------------------
struct SystemManagerConfig
{
	static int systemManagerPort;
	static string systemConfigurationFile;  // current config file
	static string initialConfigurationFile; // config given at startup, used when SystemManager retsart
	static string componentInformationFile;
	static int loadConfigFileAtStartup;
}; 

class SystemManagerUI;

// --------------------------------------------------------
//			class SystemManager
// --------------------------------------------------------
/**
* SystemManager's main functionality: 
*	- Maintain list of ComponentFactories 
*	- Start, stop and monitor components on ComponentFactories
*/
class SystemManager : public ComponentBase
					, public SystemManagerInterface
					, public Thread
{
private:
	typedef map<string, HostInformation>					MapComponentInformation;
	typedef map<string, ProcessFactoryProxy*>				MapProcessFactoryProxy;
	typedef map<string, MapComponentInformation >			MapHostComponentInformation;
	typedef map<string, int64>						        MapStringInt;

private:
	MapHostComponentInformation		mapHostComponentInformation;
	MapComponentInformation			mapHostProcessFactory;
	MapProcessFactoryProxy			mapHostProcessFactoryProxy;
	MapHostComponentInformation		verifyComponentsAreRunning;
	
	HostInformation					lookupServerInfo;
	HostInformation					settingsManagerInfo;

	Mutex							updateLock;
	ofstream						outComponentFile;

	MapStringInt					mapAliveCheck;
	MapStringInt					mapComponentNameStarts;
	
	SystemManagerUI					*systemManagerCommand;
	MonitorFactoryResources			monitorFactoryResources;

	ExceptionMonitor				exceptionMonitor;

	bool							stopSystemManager;
	int64					timeCounter;

private:
	void						initSystemManager(string configFile, string componentFile);
	int							parseConfigFilePath(ostream &ostr, MapHostComponentInformation &mapHostComponents, const ConfigurationFile &configFile, string path, HostInformation::HostDescription hostDescription);
	void						parseHostInformationFromDisc(string &filename);
	void						clearComponentInformationFile();
	void						checkIfFactoriesAreConnected();

	MapHostComponentInformation verifySystemManagerData(const MapHostComponentInformation &verifyComponentsAreRunning);
	bool						startComponentsOnFactory(const HostInformation &factoryInfo, MapComponentInformation &mapComp, ostream &errorMsg);
	string						startComponentOnFactory(ProcessFactoryProxy *proxy, HostInformation &componentInfo);
	MapHostComponentInformation startAllComponents(MapHostComponentInformation &, ostream &errorMsg);
	int							loadComponents(const MapHostComponentInformation &mapStartComponents, bool load, ostream &errorMsg);
	void						updateSystemManagerOnlineHosts(const MapHostComponentInformation &mapStartedComponents);
	ProcessFactoryProxy*		getOrCreateFactoryProxy(string factoryHostName, int factoryPortNumber);

public: // Method below is public since it may be called from main().
	inline void Quit()									{ MutexLocker lock(&updateLock); stopSystemManager = true; }

private:
	int cmdStartServiceManager(ostream &ostr);
	int cmdStartComponents(ostream &ostr, string xmlFile);
	int cmdParseStartupScript(ostream &ostr, MapHostComponentInformation &mapHostComponents, string xmlFile);
	void cmdKillAllComponents(ostream &errorMsg);
	int cmdClearDataBase(ostream &errorMsg);
	bool cmdKillComponent(string componentName);
	void cmdMonitorResources();

	// print functions
	void cmdPrintHostInformation(ostream &ostr);
	void cmdPrintAllHostInformation(ostream& ostr);
	void printFactoryInformation(ostream &ostr, HostInformation::OnlineStatus status);
	void printHostInformation(ostream &ostr, HostInformation::OnlineStatus status);

public:
	/**
	* Constructor for SystemManager.
	*
	* @param name		The SystemManager name is only used by Component class.
	*/
	SystemManager(string name);

	/**
	* Shuts down SystemManager.
	*/
	~SystemManager();

	/**
	* Allows SystemManagerUI to access private functions in SystemManager.
	*/
	friend class SystemManagerUI;
	
	/**
	* Returns InterfaceHandle for SystemManager.
	*
	* @return InterfaceHandle		The SystemManager's InterfaceHandle (Port, Host).
	*/
	inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(SystemManagerConfig::systemManagerPort, MicroMiddleware::MiddlewareSettings::GetHostName()); }

	/**
	* Main thread-loop for the SystemManager. 
	* Maintains list of component factories, and running processes on each component factory.
	*/
	virtual void run();

	/**
	* Returns the InterfaceHandle for the LookupServer.
	*/
	virtual InterfaceHandle	GetLookupServerHandle();

	/**
	* Returns the InterfaceHandle for the SettingsManager.
	*/
	//virtual InterfaceHandle GetSettingsManagerHandle();

	/**
	* Registers the component given by HostInformation in the SystemManager.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostInformation.
	*/
	virtual HostInformation HostRegistration(HostInformation info);

	/**
	* Retrieve HostInformation about a component identified by componentName.
	*
	* @param componentName	The name of the component to retrieve HostInformation.
	* @return				HostInformation about the componentName.
	*/
	virtual HostInformation GetHostInformation(string componentName);

	/**
	* Called by all Component Factories to inform the SystemManager that it is connected/disconnected.
	*
	* @param info			The Component Factory's HostInformation.
	* @return				Updated HostInformation.
	*/
	virtual HostInformation AliveChecker(HostInformation info);

	/**
	* Kills all components registered in the System Manager.
	*
	* @param returnMsg		Error messages are stored.
	*/
	virtual void			KillAll(string &returnMsg);

	/**
	* Kills a component identified by componentName.
	*
	* @param componentName	The component's name.
	* @return				1 if a component was killed, 0 if not.
	*/
	virtual int				KillComponent(string componentName, string &returnMsg);

	/**
	* Starts the components that are given in the string xmlCommands. xmlCommands contains xml-code.
	*
	* @param xmlCommands	Contains the xml-code.
	* @param returnMsg		Error messages are stored.
	* @return				0 if success, > 0 if errors were found.
	*/
	virtual int				StartComponents(string xmlCommands, map<string, string> &mapErrorMessages);

	/**
	* Shuts down the System Manager.
	*/
	virtual void			ShutDown();

	/**
	* Returns a map (string, HostInformation) of all the component information in the SystemManager.
	*
	* @return 		Returns all the component information registered in the SystemManager.
	*/
	virtual MapComponentInformation GetAllComponentInformation();

	/**
	* Returns a map (string, HostResources) of all component's resource information in the SystemManager.
	*
	* @return		Returns all the component resources registered in the SystemManager.
	*/
	virtual map<string, HostResources> GetMapResourceInformation();

	/**
	* Returns middleware exceptions that have been multicast by components and received by SystemManager.
	*
	* @param lastTimeStamp		Timestamp of last received exception message, can be empty if no previously received exception message.
	* @return					map containing middleware exceptions received by SystemManager, organized by timestamp.
	*/
	virtual map<long long, ExceptionMessage> GetMiddlewareExceptions(long long &lastTimeStamp);

	/**
	* Returns component information retrieved from xmlCommands.
	*
	* @param xmlCommands	Xml-code that is to be parsed.		
	* @param returnMsg		Error messages are stored.
	* @return				map (string, HostInformation) containing all component information.
	*/
	virtual MapComponentInformation	ParseXmlFile(string xmlCommands, string &returnMsg);

	/**
	* Clears database and kills all components registered in the System Manager.
	*
	* @param returnMsg		Error messages are stored.
	* @return				Returns 1 on success, 0 if otherwise.
	*/
	virtual int				ClearAndKillAll(string &returnMsg);

	/**
	* Sends the current HostResources image for that ProcessFactory to the SystemManager.
	*
	* @param HostResources		The current HostResources image for that ProcessFactory.
	*/
	virtual void SetResourceInformation(HostInformation &factoryInfo, map<int, HostResources> &mapHostResources);

	/**
	* Kill all running components, clear database and start components in initialConfigurationFile.
	*
	* @param errorMsg		Error messages are stored
	* @return				Returns 0 on success, otherwise, number of errors detected.
	*/
	virtual int RestartDefault(string &errorMsg);
};

} // namespace SystemManagerSpace

#endif // SYSTEM_MANAGER_H

