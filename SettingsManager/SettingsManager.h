#ifndef SettingsManager_SettingsManager_h_IsIncluded
#define SettingsManager_SettingsManager_h_IsIncluded

#include"SettingsManager/IncludeExtLibs.h"
#include<map>
#include<fstream>

// --------------------------------------------------------
//			struct SettingsManagerConfig
// --------------------------------------------------------
struct SettingsManagerConfig
{
	static string networkInterfaceIP;
	static int settingsManagerPort;
	static string settingsConfigurationFile;
	static string settingsInformationFile;
	static string hostInformationBackup;
	static int loadConfigFileAtStartup;
}; 

class SettingsManagerCommand;

// --------------------------------------------------------
//			class SettingsManager
// --------------------------------------------------------
/**
* SettingsManager's main functionality:
*/
class SettingsManager : public SettingsManagerInterface, 
						public Thread
{
private:
	typedef map<string, HostSettings>						MapComponentSettings;
	typedef map<string, HostInformation>					MapComponentInformation;
	typedef map<string, SettingsListenerProxy*>				MapComponentSettingsListener;
	typedef set<string>										SetComponentName;

private:
	MapComponentSettings			mapComponentSettings;
	MapComponentInformation			mapComponentHostInformation;
	MapComponentSettingsListener	mapComponentSettingsListenerProxy;
	SetComponentName				settingsUpdated;

	ConfigurationFile				backupComponentSettings;
	Mutex							updateLock;
	WaitCondition					updateCond;
	ofstream						outComponentFile;
	
	SettingsManagerCommand			*settingsManagerCommand;
	bool							stopSettingsManager;

private:
	int		initSettingsManager(stringstream &ostr, const string &configFile, const string &componentFile, const string &backupHostFile);
	void	clearBackupFile(const string &backupFile);

	int		parseSettingsFile(stringstream &ostr, ConfigurationFile &configFile, const string &path);
	int		parseSettingsFilePath(ostream &ostr, MapComponentSettings &mapComponentSettings, ConfigurationFile &configFile, const string &path);
	void	parseHostInformationFromDisc(const string &filename);

	bool	backupHostSettings(ConfigurationFile &configFile, const HostSettings &settings, string path);
	int		restoreHostSettings(stringstream &ostr, ConfigurationFile &configFile, string path);

	SettingsListenerProxy*		getOrCreateSettingsListenerProxy(const HostInformation &info);

	void printSettingsForComponent(ostream &ostr, string componentName);
	void printAllComponentSettings(ostream &ostr);
	void printComponents(ostream &ostr);
	void printHostInformation(ostream &ostr);
	
	int loadComponentSettings(ostream &out, string xmlFile);

private:
	inline void Quit()			
	{ 
		MutexLocker lock(&updateLock); 
		stopSettingsManager = true; 
		updateCond.wakeAll();
	}

public:
	/**
	* Constructor for SettingsManager.
	*
	* @param name		The SettingsManager name is only used by Component class.
	*/
	SettingsManager(string name);

	/**
	* Shuts down SettingsManager.
	*/
	~SettingsManager();

	/**
	* Allows SettingsManagerCommand to access private functions in SettingsManager.
	*/
	friend class SettingsManagerCommand;
	
	/**
	* Returns InterfaceHandle for SettingsManager.
	*
	* @return InterfaceHandle		The SettingsManager's InterfaceHandle (Port, Host).
	*/
	inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(SettingsManagerConfig::settingsManagerPort, MicroMiddleware::MiddlewareSettings::GetHostName()); }

	/**
	* Main thread-loop for the SettingsManager. 
	*/
	virtual void run();
	
	virtual HostSettings RegisterSettingsListener(HostInformation info);
	virtual HostSettings GetHostSettings(string componentName);
	virtual HostSettings AliveChecker(HostInformation info);
	virtual void		 ShutDown();
	virtual MapComponentSettings GetAllComponentSettings();

	virtual void UpdateSettings(const string &streamSettings);
	virtual void UpdateComponentSettings(const string &componentName, const string &nameSpace, const map<string, string> &mapAttributeValue);
	virtual void LoadSettings(string streamSettings);
};

// -------------------------------------------------------
//			class SettingsManagerCommand
//	- a simple command interface to the SettingsManager
// -------------------------------------------------------
/**
* class SettingsManagerCommand is a simple command interface to the SettingsManager.
*/
class SettingsManagerCommand : public Thread
{
private:
	SettingsManager &settingsManager;

private:
	inline void printCommand()
	{
		cout << "SettingsManager commands: quit (q), help (h), print-components (p) load-settings (l), component-settings (c)." << endl;
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
				if(ret.eof() == true || ret.bad() == true || ret.good() == false) 
				{
					throw string("End of file!");
					break;
				}
			}
			catch(...)
			{
				//cout << "WARNING! getline returned End of file! Stopping command line!" << endl;
				cout << "SettingsManagerCommand::run(): WARNING! SettingsManager user interface (UI) is off." << endl;
				break;
			}

			if(strncmp(command.c_str(), "quit", 1) == 0)
			{
				cout << "Do you really want to quit (y/n)? " ;
				string confirm;
				getline(cin, confirm);
				if(strncmp(confirm.c_str(), "yes", 1) == 0)
				{
					cout << "Quitting SettingsManager..." << endl;
					settingsManager.Quit();
					break;
				}
				else printCommand();
			}
			else if(strncmp(command.c_str(), "load-settings", 1) == 0)
			{
				cout << "Configuration file you wish to load (.xml) ";
				if(!SettingsManagerConfig::settingsConfigurationFile.empty())
					cout << "(Type 'y' to load " << SettingsManagerConfig::settingsConfigurationFile << ")";
				cout << ": ";

				string xmlFile;
				getline(cin, xmlFile);
				if(xmlFile.empty())
				{
					printCommand();
					continue;
				}
				
				if(strcmp(xmlFile.c_str(), "y") == 0)
					xmlFile = SettingsManagerConfig::settingsConfigurationFile;

				if(settingsManager.loadComponentSettings(std::cout, xmlFile) == 0)
					cout << "Loaded " << xmlFile << " successfully! " << endl;
				else
					cout << "Loading of " << xmlFile << " failed!" << endl;
			}
			else if(strncmp(command.c_str(), "print-components", 1) == 0)
			{
				settingsManager.printComponents(std::cout);
				settingsManager.printHostInformation(std::cout);
			}
			else if(strncmp(command.c_str(), "component-settings", 1) == 0)
			{
				cout << "Please enter component-name ('a' for all): ";
				string componentName;
				getline(cin, componentName);

				if(componentName == "a")
				{
					settingsManager.printAllComponentSettings(std::cout);
				}
				else if(!componentName.empty())
				{
					settingsManager.printSettingsForComponent(std::cout, componentName);
				}
				else
				{
					cout << "No component-name given." << endl;
				}
			}
			else if(strncmp(command.c_str(), "help", 1) == 0)
			{
				printCommand();
			}
			else if(!command.empty())
			{
				cout << "Command: " << command << " is not supported" << endl;
			}
		}
	}

public:
	/**
	* Constructor for SettingsManagerCommand.
	* Uses a SettingsManager reference to call SettingsManager's class functions for kill, print, etc.
	*
	* @param sm		The SettingsManager object.
	*/
	SettingsManagerCommand(SettingsManager &sm) : settingsManager(sm)
	{
		start();
	}
	/**
	* Empty destructor.
	*/
	~SettingsManagerCommand() 
	{}
};

#endif // SETTINGS_MANAGER_H

