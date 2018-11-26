#include "NetworkInformationClient/IncludeExtLibs.h"
#include "NetworkInformationClient/VivaldiClient.h"

using namespace NetworkInformationClient;

// -------------------------------------------------------
//		namespace NetworkInformationSpace
// -------------------------------------------------------
namespace NetworkInformationSpace
{
void Usage()
{
	cout << endl << "Usage: NetworkInformationClient " << endl;
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
			if(arg == "-vms")	// vivaldi manager service (name)
				VivaldiClientConfig::vivaldiManagerService = argumentTokens[++i];
			else if(arg == "-vcp")	// vivaldi client port
				VivaldiClientConfig::vivaldiClientPort = atoi(argumentTokens[++i].c_str());
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
		
		HostInformation myHostInfo = Runtime::GetHostInformation();
		ASSERT(myHostInfo.GetProcessId() > 0);
		if(myHostInfo.GetComponentName().empty())
		{
			NetworkInformationSpace::parseArguments(argc, argv);
			IDEBUG() << " Not implemented !" ;
		}
		else
		{
			string arguments = myHostInfo.GetArgumentList();
			NetworkInformationSpace::parseArguments(arguments);
			
			InterfaceHandle lookupHandle = Runtime::GetLookupServerHandle();
			IDEBUG() << " contacting service manager " << lookupHandle ;
			
			ServiceConnectionManager *serviceManager = ServiceConnectionManager::GetOrCreate();
			serviceManager->WaitConnected();

			IDEBUG() << " subscribing to : " << VivaldiClientConfig::vivaldiManagerService ;
			HostInformation myProxyInfo = myHostInfo;
			myProxyInfo.SetComponentName(myProxyInfo.GetComponentName() + ".connectTo." + VivaldiClientConfig::vivaldiManagerService);
			myProxyInfo.SetHostDescription(HostInformation::PROXY);
			HostInformation serviceVivaldiManager;
			bool ret = serviceManager->SubscribeService(myProxyInfo, VivaldiClientConfig::vivaldiManagerService, serviceVivaldiManager);

			IDEBUG() << " received vivaldi manager info " << serviceVivaldiManager ;
			
			InterfaceHandle vivaldiManagerHandle(serviceVivaldiManager.GetPortNumber(), serviceVivaldiManager.GetHostIP());
			IDEBUG() << " vivaldi manager handle " << vivaldiManagerHandle ;

			IDEBUG() << " Connecting to membership manager " ;
			MembershipManagerConnection *membershipProxy = MembershipManagerConnection::GetOrCreate();
			membershipProxy->WaitConnected();
			Runtime::SetHostInformation(membershipProxy->GetHostInformation());
			
			VivaldiClient *vivaldiClient = new VivaldiClient(Runtime::GetHostInformation(), vivaldiManagerHandle);
			vivaldiClient->start();
			
			string clientStubName = string(myHostInfo.GetComponentName() + ".stub.VivaldiClient");
			VivaldiClientStub *vivaldiClientStub = new VivaldiClientStub(clientStubName, InterfaceHandle(), vivaldiClient, true);
			ret = vivaldiClientStub->WaitStarted();
			ASSERT(ret);

			GroupHandle groupHandle(IntactSettings::GetNetworkInformationGroup(), vivaldiClientStub->GetInterfaceHandle());
			GroupConnectionManager::GetOrCreate()->JoinGroup(groupHandle);
			IDEBUG() << "Joining group : " << groupHandle ;
			
			vivaldiClient->wait();
			delete vivaldiClient;
		}

		Runtime::Release();
	}
	catch(BaseLib::Exception &ex)
	{
		IDEBUG() << " Exception " << ex.msg() ;
		return -1;
	}

	return 0;
}

