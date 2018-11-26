#include"SystemManager/IncludeExtLibs.h"
#include"SystemManager/SystemManager.h"

using namespace std;
using namespace SystemManagerSpace;

// --------------------------------------------------------
//			namespace SystemManagerSpace
// --------------------------------------------------------
namespace SystemManagerSpace
{

void Usage()
{
	cout << endl;
	cout << "Usage: " << endl;
	cout << "\tSystemManager [-p systemManagerPort] [-s systemConfigurationFile.xml] ";
	cout << "[-l 1/0] " << endl;

	cout << "Defaults: " << endl;
	cout << "\t-p systemManagerPort           = " << SystemManagerConfig::systemManagerPort << endl;
	cout << "\t-s systemConfigurationFile     = " << SystemManagerConfig::systemConfigurationFile << endl;
	cout << "\t-l load config file at startup = " << SystemManagerConfig::loadConfigFileAtStartup << endl;
	cout << endl << endl;
}

void parseArguments(int argc, char**argv)
{
	if((argc > 7) || (argc == 2 && strncmp(argv[1], "help", 4) == 0))
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
				SystemManagerConfig::systemManagerPort = atoi(argv[++i]);
			else if(strncmp(argv[i], "-s", 2) == 0) {
				SystemManagerConfig::systemConfigurationFile = string(argv[++i]);
				SystemManagerConfig::initialConfigurationFile = SystemManagerConfig::systemConfigurationFile;
			}
			else if(strncmp(argv[i], "-l", 2) == 0)
				SystemManagerConfig::loadConfigFileAtStartup = atoi(argv[++i]);
		}

		cout << endl;
		cout << "--- SystemManager Configurations ---" << endl;
		cout << "systemManagerPort           = " << SystemManagerConfig::systemManagerPort << endl;
		cout << "systemConfigurationFile     = " << SystemManagerConfig::systemConfigurationFile << endl;
		cout << "Load config file at startup = " << SystemManagerConfig::loadConfigFileAtStartup << endl;
		cout << "------------------------------------" << endl << endl;
	}
	catch(...)
	{
		cout << endl << "ERROR: Invalid argument given! " << endl;
		Usage();
		exit(-1);
	}
}
}; // namespace SystemManagerSpace

SystemManager* systemManager = NULL;

#ifdef USE_GCC
#include <signal.h>
/** Process signal containing a system manager command. */
void ProcessCommandSignal(int sig_no)
{
	if(systemManager==NULL) return;

	if(sig_no == SIGUSR1) 
	{
		// Kill all components registered in the System Manager.
		string errorMsg;
		systemManager->KillAll(errorMsg);

		if(errorMsg.length()>0)
			cerr << errorMsg << endl;
	}
	else if(sig_no == SIGUSR2) 
	{
		// Restart System Manager
		string errorMsg;
		int numErr = systemManager->RestartDefault(errorMsg);

		if(numErr > 0)
			cerr << "ProcessCommandSignal(): There were " << numErr << " errors during restart!" << endl;

		if(errorMsg.length()>0)
			cerr << errorMsg << endl;
	}
	else if(sig_no == SIGTERM) 
	{
		// Kill all components registered in the System Manager.
		string errorMsg;
		systemManager->KillAll(errorMsg);

		if(errorMsg.length()>0)
			cerr << errorMsg << endl;

		Runtime::Release(2);
		exit(1);
	}
	else 
	{
		cerr << "ProcessCommandSignal: unknown signal, sig_no=" << sig_no << endl;
	}
}

/** Initialize interupt handlers for receiving commands through signals. */
void InitializeCommandSignals()
{
	struct sigaction sa, osa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &ProcessCommandSignal;
	sigaction(SIGUSR1, &sa, &osa);
	sigaction(SIGUSR2, &sa, &osa);
	sigaction(SIGTERM, &sa, &osa);
}
#endif

// --------------------------------------------------------
//			Start SystemManager
// --------------------------------------------------------
int main(int argc, char** argv)
{
	try
	{
		Runtime::Initialize();

#ifdef USE_GCC
		InitializeCommandSignals();
#endif
		SystemManagerSpace::parseArguments(argc, argv);

		InterfaceHandle smHandle = InterfaceHandle(SystemManagerConfig::systemManagerPort, MicroMiddleware::MiddlewareSettings::GetHostName());
		
		IDEBUG() << " system manager " << smHandle << " host-name " << TcpServer::GetLocalHostName();

        systemManager = new SystemManager(MicroMiddleware::MiddlewareSettings::GetHostName());
		SystemManagerStub* systemManagerStub = new SystemManagerStub(smHandle, (SystemManagerInterface*)systemManager, true);

		systemManager->start();
		systemManager->wait();

		/*
		// We get here if/when this process has received a SIGTERM signal. Kill all components managed by
		// this system manager before exiting.
		string errorMsg;
		systemManager->KillAll(errorMsg);
		if(errorMsg.length()>0)	cerr << errorMsg << endl;*/	

		IDEBUG() << "Shutting down system manager.";
		
		systemManager->Quit();
		// Delete system manager in order to cleanup properly (ComponentInformation.txt)
		delete systemManager;

		Runtime::Release(5); // wait for 2.5 seconds before exiting.
	}
	catch(exception &x)
	{
		cout << "Exception: " << x.what() << endl;
		
		Runtime::Release();
		return -1;
	}

	return 0;
}

