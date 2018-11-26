#include"SystemManager/SystemManagerUI.h"

namespace SystemManagerSpace
{

SystemManagerUI::SystemManagerUI(SystemManager &sm) : systemManager(sm)
{
	start();
}

SystemManagerUI::~SystemManagerUI() 
{}

void SystemManagerUI::printCommand()
{
	cout << "SystemManager commands: print-info (p), monitor-resources (m), load-xml (l), clear/kill-database (c), kill-all (k), stop-component (s), exception-list (e), init-service-manager (i), quit (q), help (h)." << endl;
}

void SystemManagerUI::run()
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
			}
		}
		catch(...)
		{
			cout << "SystemManagerUI::run(): WARNING! SystemManager user interface (UI) is off." << endl;
			break;
		}

		if(strncmp(command.c_str(), "monitor-all", 1) == 0)
		{
			systemManager.cmdMonitorResources();
		}
		else if(strncmp(command.c_str(), "kill-all", 1) == 0)
		{
			cout << "Do you really want to kill all components (y/n)? " ;
			string confirm;
			getline(cin, confirm);
			if(strncmp(confirm.c_str(), "yes", 1) == 0)
			{
				stringstream errorMsg;
				systemManager.cmdKillAllComponents(errorMsg);
			}
			else printCommand();
		}
		else if(strncmp(command.c_str(), "clear/kill-database", 1) == 0)
		{
			cout << "Do you really want to clear database and effectively kill all components (y/n)? " ;
			string confirm;
			getline(cin, confirm);
			if(strncmp(confirm.c_str(), "yes", 1) == 0)
			{
				stringstream errorMsg;
				systemManager.cmdKillAllComponents(errorMsg);
				systemManager.cmdClearDataBase(errorMsg);
			}
			else printCommand();
		}
		else if(strncmp(command.c_str(), "stop-component", 1) == 0)
		{
			cout << "Name of component you wish to stop (effectively kill): " ;
			string componentName;
			getline(cin, componentName);
			if(componentName.empty()) 
			{
				cout << "No components stopped." << endl;
				printCommand();
				continue;
			}

			if(!systemManager.cmdKillComponent(componentName))
			{
				cout << "Component " << componentName << " not found." << endl;
				printCommand();
				continue;
			}
		}
		else if(strncmp(command.c_str(), "load-xml", 1) == 0)
		{
			cout << "Configuration file you wish to load (.xml) ";
			if(!SystemManagerConfig::systemConfigurationFile.empty())
				cout << "(Type 'y' to load " << SystemManagerConfig::systemConfigurationFile << ")";
			cout << ": ";

			string xmlFile;
			getline(cin, xmlFile);
			if(xmlFile.empty())
			{
				printCommand();
				continue;
			}

			if(strcmp(xmlFile.c_str(), "y") == 0)
				xmlFile = SystemManagerConfig::systemConfigurationFile;

			if(systemManager.cmdStartComponents(std::cout, xmlFile) == 0)
			{
				cout << "Loaded " << xmlFile << " successfully! " << endl;
				cerr << "Please check the status of the components using 'print-info' command!" << endl;
			}
			else
				cout << "Loading of " << xmlFile << " failed!" << endl;
		}
		else if(strncmp(command.c_str(), "exception-list", 1) == 0)
		{
			systemManager.exceptionMonitor.PrintAll(cout);
		}
		else if(strncmp(command.c_str(), "init-service-manager", 1) == 0)
		{
			cout << "Starting ServiceManager...." << endl;
			int ret = systemManager.cmdStartServiceManager(cout);
			if(ret == 0)
				cout << "Successfully started ServiceManager!" << endl;
			else
				cout << "Could not start ServiceManager!" << endl;
		}
		else if(strncmp(command.c_str(), "quit", 1) == 0)
		{
			cout << "Do you really want to quit (y/n)? " ;
			string confirm;
			getline(cin, confirm);
			if(strncmp(confirm.c_str(), "yes", 1) == 0)
			{
				cout << "Quitting SystemManager..." << endl;
				systemManager.Quit();
				break;
			}
			else printCommand();
		}
		else if(strncmp(command.c_str(), "print-info", 1) == 0)
		{
			systemManager.cmdPrintHostInformation(cout);
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

} // namespace SystemManagerSpace

