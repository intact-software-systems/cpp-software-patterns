#include "ProcessFactory/IncludeExtLibs.h"
#include "ProcessFactory/ProcessFactory.h"

using namespace std;

// -------------------------------------------------------
//			namespace ProcessFactorySpace
// -------------------------------------------------------
namespace ProcessFactorySpace
{
void Usage()
{
	cout << endl << "Usage: ProcessFactory " << endl;
	cout << "-smh systemManagerHost [-smp systemManagerPort] " << endl;
	cout << "[-p factoryPort] " << endl;
	cout << "[-d display (noX = no xterm)] " << endl;
	cout << "[-l debugLevel (0 = no debug, 1 = stdout to exec.debug, or, xterm not closed)] " << endl;
	cout << "[-a localIp (In case of multiple network interfaces, it is possible to specify which ip to use)" << endl;
	cout << "[--bindir location of component binaries.]" << endl;
	cout << "[--logdir location of debug log files.]" << endl;
	cout << "[--debugtool (valgrind = a tool to measure efficiency of programs. Requires debugLevel = 1. NB! Makes components very slow!)]" << endl;
	cout << endl ;
	
	cout << "Defaults:" << endl;
	//cout << "-smh systemManagerHost = " << ProcessFactoryConfig::systemManagerHost << endl;
	cout << "-smp systemManagerPort = " << ProcessFactoryConfig::systemManagerPort << endl;
	cout << "-p factoryPort = " << ProcessFactoryConfig::factoryPort << endl;
	cout << "-d display = " << ProcessFactoryConfig::display << endl;
	cout << "-l debugLevel = "  << ProcessFactoryConfig::debugLevel << endl;
	cout << "-a localIp = " << ProcessFactoryConfig::networkInterfaceIP << endl;
	cout << "--bindir = "  << ProcessFactoryConfig::binDir << endl;
	cout << "--logdir = "  << ProcessFactoryConfig::logDir << endl;
	cout << "--debugtool = " << ProcessFactoryConfig::debugTool << endl;
	cout << endl << endl;
}

void parseArguments(int argc, char**argv)
{
	if((argc > 13) || (argc == 2 && strncmp(argv[1], "help", 4) == 0))
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
				ProcessFactoryConfig::factoryPort = atoi(argv[++i]);
			else if(strncmp(argv[i], "-smh", 4) == 0)
				ProcessFactoryConfig::systemManagerHost = string(argv[++i]);
			else if(strncmp(argv[i], "-smp", 4) == 0)
				ProcessFactoryConfig::systemManagerPort = atoi(argv[++i]);
			else if(strncmp(argv[i], "-d", 2) == 0)
				ProcessFactoryConfig::display = string(argv[++i]);
			else if(strncmp(argv[i], "-l", 2) == 0)
				ProcessFactoryConfig::debugLevel = atoi(argv[++i]);
			else if(strncmp(argv[i], "-a", 2) == 0)
				ProcessFactoryConfig::networkInterfaceIP = string(argv[++i]);
			else if(strcmp(argv[i], "--bindir") == 0) {
#ifdef USE_GCC
				ProcessFactoryConfig::binDir = string(argv[++i])+"/";
#else
				ProcessFactoryConfig::binDir = string(argv[++i])+"\\";
#endif
			} else if(strcmp(argv[i], "--logdir") == 0) {
#ifdef USE_GCC
				ProcessFactoryConfig::logDir = string(argv[++i])+"/";
#else
				ProcessFactoryConfig::logDir = string(argv[++i])+"\\";
#endif
			}
			else if(strcmp(argv[i], "--debugtool") == 0)
				ProcessFactoryConfig::debugTool = string(argv[++i]);
		}

		if(!ProcessFactoryConfig::networkInterfaceIP.empty() && !NetworkFunctions::isIPAddressOnHost(TcpServer::GetLocalHostName(), ProcessFactoryConfig::networkInterfaceIP))
		{
			cout << "WARNING! " << ProcessFactoryConfig::networkInterfaceIP << " was not recognized as a valid ip-address on host " << TcpServer::GetLocalHostName() << endl;
			ProcessFactoryConfig::networkInterfaceIP = string("");
		}

		cout << endl;
		cout << "--- ProcessFactory Configurations ---" << endl;
		cout << "factoryPort = " << ProcessFactoryConfig::factoryPort << endl;
		cout << "systemManagerHost = " << ProcessFactoryConfig::systemManagerHost << endl;
		cout << "systemManagerPort = " << ProcessFactoryConfig::systemManagerPort << endl;
		cout << "display = " << ProcessFactoryConfig::display << endl;
		cout << "debugLevel = " << ProcessFactoryConfig::debugLevel << endl;
		cout << "Network IP = " << ProcessFactoryConfig::networkInterfaceIP << endl;
		cout << "bindir = " << ProcessFactoryConfig::binDir << endl;
		cout << "logdir = " << ProcessFactoryConfig::logDir << endl;
		cout << "debugtool = " << ProcessFactoryConfig::debugTool << endl;
		cout << "---------------------------------------" << endl << endl;
	

		// init HostInformation for factory process
		{
			HostInformation appInfo(MicroMiddleware::MiddlewareSettings::GetHostName(), MicroMiddleware::MiddlewareSettings::GetHostName(), ProcessFactoryConfig::factoryPort, HostInformation::COMPONENT_FACTORY);
			appInfo.SetExecutableName(argv[0]);
			appInfo.SetStartDescription(HostInformation::RESTART);
			appInfo.SetProcessId(Thread::processId());
			appInfo.SetHostIP(ProcessFactoryConfig::networkInterfaceIP);
	
			stringstream componentName;
			componentName << "factory." << MicroMiddleware::MiddlewareSettings::GetHostName();
			appInfo.SetComponentName(componentName.str());

			ProcessFactoryConfig::factoryInfo = appInfo;
		}
	}
	catch(...)
	{
		cout << endl << "ERROR: Invalid argument given! " << endl;
		Usage();
		exit(-1);
	}
}

} // namespace ProcessFactorySpace

/*ProcessFactory::ProcessFactory *componentFactory = NULL;

#ifdef USE_GCC
#include <signal.h>
// Process signal containing a component factory command.
void ProcessCommandSignal(int sig_no)
{
	if(componentFactory==NULL) return;

	if(sig_no == SIGUSR1 || sig_no == SIGTERM) 
	{
		// Kill all components registered in this ProcessFactory.
		componentFactory->cmdKillAll();
	}
	else 
	{
		cerr << "ProcessCommandSignal: unknown signal, sig_no=" << sig_no << endl;
	}
}

// Initialize interupt handlers for receiving commands through signals.
void InitializeCommandSignals()
{
	struct sigaction sa, osa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &ProcessCommandSignal;
	sigaction(SIGUSR1, &sa, &osa);
	sigaction(SIGTERM, &sa, &osa);
}
#endif
*/

// -------------------------------------------------------
//			main function ProcessFactory
// -------------------------------------------------------
int main(int argc, char *argv[])
{
    try 
	{
	    Runtime::Initialize();
		
//#ifdef USE_GCC
//		InitializeCommandSignals();
//#endif

		ProcessFactorySpace::parseArguments(argc, argv);

		ProcessFactorySpace::ProcessFactory *componentFactory = new ProcessFactorySpace::ProcessFactory(
					"ProcessFactory", 
					ProcessFactoryConfig::factoryPort, 
					InterfaceHandle(ProcessFactoryConfig::systemManagerPort, ProcessFactoryConfig::systemManagerHost));

		// -- Setup connection to SystemManager --
		if(!ProcessFactoryConfig::systemManagerHost.empty() && ProcessFactoryConfig::systemManagerPort > -1)
		{
			InterfaceHandle smHandle = InterfaceHandle(ProcessFactoryConfig::systemManagerPort, ProcessFactoryConfig::systemManagerHost);

			IDEBUG() << " Connecting to SystemManager: " << smHandle;
			SystemManagerFactoryConnection *systemManagerConnection = new SystemManagerFactoryConnection(ProcessFactoryConfig::factoryInfo, smHandle, true);
		}
        
		ProcessFactoryStub* componentFactoryStub = new ProcessFactoryStub(InterfaceHandle(MicroMiddleware::MiddlewareSettings::GetHostName(), ProcessFactoryConfig::factoryPort), componentFactory);
		componentFactory->start();
		componentFactory->wait();

		/*Runtime::KeepAlive();
		// We get here if/when this process has received a SIGTERM signal. Kill components managed by
		// this factory before exiting.*/

		cout << "Shutting down component factory." << endl << flush;
		componentFactory->cmdQuit();
		// Delete component factory in order to cleanup properly.
		delete componentFactory;
		
		Runtime::Release(2); // wait for 2.5 seconds before exiting.
    }
    catch(exception &x) {
        cout << "Exception: " << x.what() << endl;
        char c;
        cin >> c;

		Runtime::Release();
	}

    cout.flush();
    return 0;
}

