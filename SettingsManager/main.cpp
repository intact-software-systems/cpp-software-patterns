#include "SettingsManager/IncludeExtLibs.h"
#include "SettingsManager/SettingsManager.h"

// --------------------------------------------------------
//			namespace SettingsManagerSpace
// --------------------------------------------------------
namespace SettingsManagerSpace
{
void Usage()
{
	cout << endl;
	cout << "Usage: " << endl;
	cout << "\tSettingsManager [-p settingsManagerPort] [-s settingsConfigurationFile.xml] ";
	cout << "[-l 1/0] " << endl;
	cout << "[-a localIp (In case of multiple network interfaces, it is possible to specify which ip to use)" << endl;
	cout << endl ;

	cout << "Defaults: " << endl;
	cout << "\t-p settingsManagerPort         = " << SettingsManagerConfig::settingsManagerPort << endl;
	cout << "\t-s settingsConfigurationFile   = " << SettingsManagerConfig::settingsConfigurationFile << endl;
	cout << "\t-l load config file at startup = " << SettingsManagerConfig::loadConfigFileAtStartup << endl;
	cout << "\t-a localIp                     = " << SettingsManagerConfig::networkInterfaceIP << endl;
	cout << endl << endl;
}

void parseArguments(const std::string &arguments)
{
	vector<string> argumentTokens;
	HostInformationSpace::TokenizeString(arguments, argumentTokens, " ");
	if(argumentTokens.empty()) return;

	try
	{
		for(size_t i = 0; i < argumentTokens.size() - 1; i++)
		{
			string arg = argumentTokens[i];
			if(strncmp(arg.c_str(), "-p", 2) == 0)
				SettingsManagerConfig::settingsManagerPort = atoi(argumentTokens[++i].c_str());
			else if(strncmp(arg.c_str(), "-s", 2) == 0)
				SettingsManagerConfig::settingsConfigurationFile = argumentTokens[++i];
			else if(strncmp(arg.c_str(), "-l", 2) == 0)
				SettingsManagerConfig::loadConfigFileAtStartup = atoi(argumentTokens[++i].c_str());
			else if(strncmp(arg.c_str(), "-a", 2) == 0)
				SettingsManagerConfig::networkInterfaceIP = argumentTokens[++i];
		}

		cout << endl;
		cout << "--- SettingsManager Configurations ---" << endl;
		cout << "settingsManagerPort         = " << SettingsManagerConfig::settingsManagerPort << endl;
		cout << "settingsConfigurationFile   = " << SettingsManagerConfig::settingsConfigurationFile << endl;
		cout << "Load config file at startup = " << SettingsManagerConfig::loadConfigFileAtStartup << endl;
		cout << "Network IP                  = " << SettingsManagerConfig::networkInterfaceIP << endl;
		cout << "------------------------------------" << endl << endl;
	
		if(!SettingsManagerConfig::networkInterfaceIP.empty() && !NetworkFunctions::isIPAddressOnHost(TcpServer::GetLocalHostName(), SettingsManagerConfig::networkInterfaceIP))
		{
			cout << "WARNING! " << SettingsManagerConfig::networkInterfaceIP << " was not recognized as a valid ip-address on host " << TcpServer::GetLocalHostName() << endl;
			SettingsManagerConfig::networkInterfaceIP = string("");
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
	if((argc > 7) || (argc == 2 && strncmp(argv[1], "help", 4) == 0))
	{
		Usage();
		exit(-1);
	}
	
	stringstream argString;
	for(int i = 1; i < argc; i++)
		argString << argv[i] << " ";

	parseArguments(argString.str());
}

}; // namespace SettingsManagerSpace

// --------------------------------------------------------
//			Start SettingsManager
// --------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{
		Runtime::Initialize(argc, argv);
 
 		HostInformation componentInfo = Runtime::GetHostInformation();
		if(componentInfo.IsValid())
			SettingsManagerSpace::parseArguments(componentInfo.GetArgumentList());
		else
			SettingsManagerSpace::parseArguments(argc, argv);

		SettingsManager* settingsManager = new SettingsManager(MicroMiddleware::MiddlewareSettings::GetHostName());
		SettingsManagerStub* settingsManagerStub = new SettingsManagerStub(InterfaceHandle(SettingsManagerConfig::settingsManagerPort, MicroMiddleware::MiddlewareSettings::GetHostName()), (SettingsManagerInterface*)settingsManager, true);
		
		settingsManager->start();
		
		{	// registering service 
			IDEBUG() << " connecting to ServiceManager" ;
			ServiceConnectionManager *serviceManager = ServiceConnectionManager::GetOrCreate();
			serviceManager->WaitConnected();
			
			HostInformation myHostInformation = Runtime::GetHostInformation();
			myHostInformation.SetExecutableName(argv[0]);
			myHostInformation.SetStartDescription(HostInformation::RESTART);
			myHostInformation.SetHostIP(SettingsManagerConfig::networkInterfaceIP);
			myHostInformation.SetProcessId(Thread::processId());
			myHostInformation.SetHostDescription(HostInformation::STUB);
			myHostInformation.SetPortNumber(SettingsManagerConfig::settingsManagerPort);
			myHostInformation.SetComponentName(IntactMiddleware::IntactSettings::GetSettingsManagerService());

			IDEBUG() << "Registering service : " << myHostInformation ;
			serviceManager->RegisterService(myHostInformation);
		}

		settingsManager->wait();
		delete settingsManager;

		Runtime::Release();
	}
	catch(exception &x)
	{
		cout << "Exception: " << x.what() << endl;
		ExceptionMulticast::PostException(x.what(), MiddlewareException::GENERAL_EXCEPTION);
		
		Runtime::Release();
		return -1;
	}

	return 0;
}

