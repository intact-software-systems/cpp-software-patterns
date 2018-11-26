#include"ServiceManager/ServiceManager.h"

using namespace std;
using namespace IntactMiddleware;
using namespace MicroMiddleware;

// --------------------------------------------------------
//			namespace ServiceManagerSpace
// --------------------------------------------------------
namespace ServiceManagerSpace
{
void Usage()
{
	cout << endl << "Usage: ServiceManager [-p port] " << endl; 
	cout << "Default: port = " << ServiceManagerConfig::serviceManagerPort ;
	cout << endl << endl;
}

/*void parseArguments(int argc, char**argv)
{
	if((argc > 5) || (argc == 2 && strncmp(argv[1], "help", 4) == 0))
	{
		Usage();
		exit(-1);	
	}

	try
	{
		for(int i = 1; i < argc - 1; i++)
		{
			//cout << "Arg: " <<  argv[i] << " " << argv[i+1] << " ";
			if(strncmp(argv[i], "-p", 2) == 0)
				ServiceManagerConfig::serviceManagerPort = atoi(argv[++i]);
		}

		cout << endl;
		cout << "--- ServiceManager Configurations ---" << endl;
		cout << "Port number: \t" << ServiceManagerConfig::serviceManagerPort << endl;
		cout << "------------------------------------" << endl << endl;
	}
	catch(...)
	{
		cout << endl << "ERROR: Invalid argument given! " << endl;
		Usage();
		exit(-1);
	}
}*/

void parseArguments(const string &arguments)
{
	if(arguments.empty() == true) return;

	vector<string> argumentTokens;
	HostInformationSpace::TokenizeString(arguments, argumentTokens, " ");

	try
	{
		for(size_t i = 0; i < argumentTokens.size() - 1; i++)
		{
			string arg = argumentTokens[i];
			if(arg == "-p") 
				ServiceManagerConfig::serviceManagerPort = atoi(argumentTokens[++i].c_str());
			else if(arg == "help")
			{
				Usage();
				exit(0);
			}
		}
	}
	catch(...)
	{
		cout << endl << "ERROR: Invalid argument given! " << endl;
		Usage();
		exit(-1);
	}
}

void parseArguments(int argc, char**argv)
{
	stringstream argString;
	for(int i = 1; i < argc; i++)
		argString << argv[i] << " ";

	parseArguments(argString.str());
}


}; // namespace ServiceManagerSpace

// --------------------------------------------------------
//			Start ServiceManager
// --------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{
		Runtime::Initialize(argc, argv);

		HostInformation hostInfo = Runtime::GetHostInformation();
		if(hostInfo.GetComponentName().empty())
			ServiceManagerSpace::parseArguments(argc, argv);
		else
			ServiceManagerSpace::parseArguments(hostInfo.GetArgumentList());

        ServiceManagerSpace::ServiceManager* serviceManager = new ServiceManagerSpace::ServiceManager(MicroMiddleware::MiddlewareSettings::GetHostName());
		ServiceManagerStub* serviceManagerStub = new ServiceManagerStub(InterfaceHandle(MicroMiddleware::MiddlewareSettings::GetHostName(), ServiceManagerSpace::ServiceManagerConfig::serviceManagerPort), (ServiceManagerInterface*)serviceManager, true);

		serviceManager->start();
		serviceManager->wait();
		delete serviceManager;

		Runtime::Release();
	}
	catch(Exception &x)
	{
		cout << "Exception: " << x.msg() << endl;
		
		Runtime::Release();
		return -1;
	}
	return 0;
}


