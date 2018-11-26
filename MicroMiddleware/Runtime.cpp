#include "MicroMiddleware/Runtime.h"
#include "MicroMiddleware/IncludeExtLibs.h"
#include "MicroMiddleware/CommonDefines.h"
#include "MicroMiddleware/MiddlewareException.h"

#include <signal.h>

#ifdef USE_GCC
#include <execinfo.h>
#endif

namespace MicroMiddleware
{

bool Runtime::Initialized = false;
InterfaceHandle Runtime::lookupServerHandle		= InterfaceHandle();
InterfaceHandle Runtime::localHostHandle		= InterfaceHandle();
HostInformation Runtime::componentHostInformation = HostInformation();

void Runtime::Initialize()
{
	BaseLib::InterruptHandling::InitSignalHandlers();
    cout << COMPONENT_PROCESS_ID << " " << Thread::processId() << endl;

	BaseLib::ObjectManager::createObjectManager();

#ifdef WIN32
	WSADATA wsaData;    
	WORD wVersionRequested = MAKEWORD(2, 2);     
	int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
		throw runtime_error("Could not initialise WSA!");
#endif

	Initialized = true;
}

void Runtime::Initialize(int argc, char* argv[])
{
	Runtime::Initialize();
	
	if(Runtime::HasLookupServerInfo(argc, argv))
	{
		Runtime::GetLookupServerHandle(argc, argv);
	}

	Runtime::GetHostInformation(argc, argv);
}

void Runtime::Release(int maxWaitCounter) 
{
#if 0
	    int err = WSACleanup();
	    if ( err != 0 )
		    throw runtime_error("Could cleanup WSA!");
#endif
        Initialized = false;
}

bool Runtime::HasLookupServerInfo(int argc, char* argv[])
{
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], PREFIX_LOOKUP_HOST) == 0 ||
			strcmp(argv[i], PREFIX_LOOKUP_PORT) == 0)
		{
			return true;
		}
	}
	return false;
}

InterfaceHandle Runtime::GetLookupServerHandle(int argc, char* argv[])
{
	if(argc < 4)
		throw MiddlewareException("Illegal number of command line arguments");

	int lookupServerPort = -1;
	string lookupServerHost;

	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], PREFIX_LOOKUP_HOST) == 0)
		{
			lookupServerHost = string(argv[i+1]);		
		}
		else if(strcmp(argv[i], PREFIX_LOOKUP_PORT) == 0)
		{
			lookupServerPort = atoi(argv[i+1]);
		}
	}

	if(lookupServerPort == -1)
		throw MiddlewareException("Could not identify LookupServer port from component arguments");
	else if(lookupServerHost.empty())
		throw MiddlewareException("Could not identify LookupServer host from component arguments");

	Runtime::lookupServerHandle = InterfaceHandle(lookupServerPort, lookupServerHost);

	return Runtime::lookupServerHandle;
}

InterfaceHandle Runtime::GetLookupServerHandle()
{
	return Runtime::lookupServerHandle;
}

string Runtime::GetInstanceName(int argc, char* argv[])
{
    if(argc<2)
        throw MiddlewareException("Illegal number of command line arguments");

	// -- new implementation --
	for(int i = 1; i < argc; i++)
		if(strcmp(argv[i], PREFIX_INSTANCE_NAME) == 0)
			return string(argv[i+1]);

	// -- old implementation --
    return argv[1];
}

HostInformation Runtime::GetHostInformation(string instanceName)
{
	vector<HostInformation> vectorHostInformation;
	HostInformationSpace::ParseHostInformationFromDisc(instanceName, vectorHostInformation);

	if(vectorHostInformation.empty()) return HostInformation(instanceName);

	if(vectorHostInformation.size() > 1)
		throw MiddlewareException("Illegal number of host information entries found in " + instanceName );

	for(vector<HostInformation>::iterator it = vectorHostInformation.begin(), it_end = vectorHostInformation.end(); it != it_end; ++it)
		return *it;

	return HostInformation(instanceName);
}

HostInformation Runtime::GetHostInformation(int argc, char* argv[])
{
	stringstream hostInfoArguments;
	bool foundHostInfoArg = false;
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], PREFIX_HOST_INFO) == 0)
			foundHostInfoArg = true;
		else if(strncmp(argv[i], "--",2) == 0 && foundHostInfoArg)
			break;
		else if(foundHostInfoArg)
			hostInfoArguments << argv[i] << " ";
	}

	if(hostInfoArguments.str().empty()) return HostInformation();

	vector<HostInformation> vectorHostInformation;
	HostInformationSpace::ParseHostInformationFromDisc(hostInfoArguments, vectorHostInformation);

	if(vectorHostInformation.empty()) return HostInformation();

	if(vectorHostInformation.size() > 1)
		throw MiddlewareException("ParseComponentArguments(argc,argv): Illegal number of host information entries found.");

	for(vector<HostInformation>::iterator it = vectorHostInformation.begin(), it_end = vectorHostInformation.end(); it != it_end; ++it)
	{
		HostInformation &info = *it;
		info.SetOnlineStatus(HostInformation::ONLINE);
		info.SetProcessId(Thread::processId());

		string ip = info.GetHostIP();
		if(ip.empty() == true) 
			ip = info.GetHostName();

		Runtime::localHostHandle = InterfaceHandle(info.GetPortNumber(), ip);
		Runtime::componentHostInformation = info;
		return info;
	}

	return HostInformation();
}

HostInformation Runtime::GetHostInformation()
{
	return Runtime::componentHostInformation;
}

std::string Runtime::GetComponentName()
{
	return Runtime::componentHostInformation.GetComponentName();
}

void Runtime::SetHostInformation(HostInformation hostInfo)
{
	IDEBUG() << "Updated hostInformation to : " << hostInfo;
	ASSERT(hostInfo.GetComponentId() > -1);
	Runtime::componentHostInformation = hostInfo;
}

InterfaceHandle Runtime::GetInterfaceHandle()
{
	return Runtime::localHostHandle;
}

void Runtime::ShutDown()
{
//#if 1 
//	RunTimeSpace::Interrupt::CatchInterrupts(SIGTERM);
//#else
//	Runtime::Release(0);
//	exit(1);
//#endif
}

}; // namespace MicroMiddleware


