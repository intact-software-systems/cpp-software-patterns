#include "NetworkInformationManager/IncludeExtLibs.h"
#include "NetworkInformationManager/VivaldiManager.h"

using namespace NetworkInformationManager;

// -------------------------------------------------------
//		namespace NetworkInformationSpace
// -------------------------------------------------------
namespace NetworkInformationSpace
{
void Usage()
{
	cout << endl << "Usage: NetworkInformationManager " << endl;
	cout << endl ;
	
	cout << "Defaults:" << endl;
	cout << endl << endl;
}

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
			if(arg == "-vmp") 			// vivaldi manager port
				VivaldiManagerConfig::vivaldiManagerPort = atoi(argumentTokens[++i].c_str());
			else if(arg == "-vms")		// vivaldi manager service (name)
				VivaldiManagerConfig::vivaldiManagerService = argumentTokens[++i];
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

}; // namespace NetworkInformationSpace

int main(int argc, char **argv)
{
	try
	{
		Runtime::Initialize(argc, argv);
		
		HostInformation hostInfo = Runtime::GetHostInformation();
		if(hostInfo.GetComponentName().empty())
		{
			IDEBUG() << "Not supported!";
			NetworkInformationSpace::parseArguments(argc, argv);
			// TODO: when component is not started from SystemManager, ComponentFactory
		}
		else
		{
			IntactMiddleware::IntactSettings::ConnectInfra(true);

			IDEBUG() << " starting VivaldiManager";
			VivaldiManager *vivaldiManager = new VivaldiManager();
			vivaldiManager->start();
		
			VivaldiManagerServer *vivaldiServer = new VivaldiManagerServer(VivaldiManagerConfig::vivaldiManagerService, InterfaceHandle(), vivaldiManager, true);

			vivaldiManager->wait();
		}

		Runtime::Release();
	}
	catch(BaseLib::Exception &ex)
	{
		IDEBUG() << " Exception " << ex.msg();
		return -1;
	}

	return 0;
}

