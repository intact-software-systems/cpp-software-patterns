#include "ServiceManager/ServiceManagerUI.h"

namespace ServiceManagerSpace
{
// -------------------------------------------------------
//			class ServiceManagerUI
// -------------------------------------------------------
ServiceManagerUI::ServiceManagerUI(ServiceManager &ls) : serviceManager(ls)
{
	start();
}

ServiceManagerUI::~ServiceManagerUI() 
{

}
// -------------------------------------------------------
//			the UI
// -------------------------------------------------------
void ServiceManagerUI::printCommand()
{
    cout << "ServiceManager commands: print-info (p), clear-services (c), quit (q), help (h)." << endl;
}

void ServiceManagerUI::run()
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
			//cout << "ServiceManagerCommand: WARNING! getline returned End of file! Stopping command line!" << endl;
			cout << "ServiceManagerCommand::run(): WARNING! ServiceManager user interface (UI) is off." << endl;
			break;
		}

		if(strncmp(command.c_str(), "quit", 1) == 0)
		{
			cerr << "Do you really want to quit (y/n)? " ;
			string confirm;
			getline(cin, confirm);
			if(strncmp(confirm.c_str(), "yes", 1) == 0)
			{
				cerr << "Quitting ServiceManager..." << endl;
				serviceManager.Quit();
				break;
			}
			else printCommand();
		}
		else if(strncmp(command.c_str(), "clear-services", 1) == 0)
		{
			serviceManager.ClearServices();
		}
		else if(strncmp(command.c_str(), "print-info", 1) == 0)
		{
			serviceManager.cmdPrintHostInformation(cerr);
		}
		else if(strncmp(command.c_str(), "help", 1) == 0)
		{
			printCommand();
		}
		else if(!command.empty())
		{
			cerr << "Command: " << command << " is not supported" << endl;
		}
	}
}

} // namespace ServiceManagerSpace


