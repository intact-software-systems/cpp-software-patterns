#include "IntactMiddleware/MiddlewareSettings.h"
#include "IntactMiddleware/ServiceConnectionManager.h"
#include "IntactMiddleware/GroupConnectionManager.h"
#include "IntactMiddleware/MembershipManagerConnection.h"
#include "IntactMiddleware/INetNATManager.h"
#include "IntactMiddleware/IntactSettings/IncludeLibs.h"

namespace IntactMiddleware
{
/* -------------------------------------------------------
 		class MiddlewareSettings
 -------------------------------------------------------*/

// -- Intact Services --
std::string MiddlewareSettings::networkInformationManagerService_ 	= std::string("NetworkInformationManager.stub");
std::string MiddlewareSettings::settingsManagerService_				= std::string("SettingsManager.stub");
std::string MiddlewareSettings::intactManagerService_				= std::string("IntactManager.stub");
std::string MiddlewareSettings::serviceManagerService_				= std::string("ServiceManager.stub");
std::string MiddlewareSettings::groupManagerService_				= std::string("GroupManager.stub");
std::string MiddlewareSettings::membershipManagerService_			= std::string("MembershipManager.stub");

// -- Intact Groups --
std::string MiddlewareSettings::networkInformationGroup_			= std::string("NetworkInformation.vivaldi.client.group");

/* -------------------------------------------------------
 		Connect to Intact Infrastructure
 -------------------------------------------------------*/
void MiddlewareSettings::Initialize(int argc, char **argv)
{
	MicroMiddleware::Runtime::Initialize(argc, argv);
	MiddlewareSettings::ConnectInfra(false);

	MicroMiddleware::INetConnectionManager::GetOrCreate();
	INetNATManager::GetOrCreate();
}

void MiddlewareSettings::Release()
{
	MicroMiddleware::Runtime::Release();
}

/* -------------------------------------------------------
 		Connect to Intact Infrastructure
 -------------------------------------------------------*/
void MiddlewareSettings::ConnectInfra(bool waitConnected)
{
	static Mutex mutex;
	MutexLocker lock(&mutex);

	// 1. Create connection to ServiceManager
	{
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		ASSERT(service);
		if(waitConnected)
		{
			iDebug() << COMPONENT_FUNCTION << " Waiting for connection to service manager ";
			service->WaitConnected();
		}
	}

	// 2. Connect and login to MembershipManager
	{
		MembershipManagerConnection *membershipProxy = MembershipManagerConnection::GetOrCreate();
		if(waitConnected)
		{
			iDebug() << COMPONENT_FUNCTION << " Waiting for connection to membership manager ";
			membershipProxy->WaitConnected();
		}
	}

	// 3. Create connection to GroupManager
	{
		GroupConnectionManager *group = GroupConnectionManager::GetOrCreate();
		ASSERT(group);
		if(waitConnected)
		{
			iDebug() << COMPONENT_FUNCTION << " Waiting for connection to group manager ";
			group->WaitConnected();
		}
	}
}

/* -------------------------------------------------------
 		Connect to SettingsManager
 -------------------------------------------------------*/
bool MiddlewareSettings::ConnectSettingsManager()
{
	ServiceConnectionManager *serviceManagerConnection = ServiceConnectionManager::GetOrCreate();

	HostInformation serviceInformation;
	bool waitConnected = true;
	serviceManagerConnection->SubscribeService(IntactSettings::GetSettingsManagerService(), serviceInformation, waitConnected);

	if(serviceInformation.IsValid() == false) return false;

	HostInformation myHostInformation = Runtime::GetHostInformation();
	InterfaceHandle settingsManagerHandle(serviceInformation.GetPortNumber(), serviceInformation.GetHostIP());
	bool ret = SettingsManagerConnection::SetupSettingsManagerConnections(settingsManagerHandle, myHostInformation);
	if(ret == false)
		iWarning() << COMPONENT_FUNCTION  << "WARNING! Can't connect to SettingsManager!";

	SettingsListener *settingsListener = SettingsListener::GetOrCreateSettingsListener();
	if(settingsListener == NULL)
	{
		iCritical() << COMPONENT_FUNCTION << "SettingsListener == NULL!! " ;
		return false;
	}
	return true;
}

} // namespace IntactMiddleware

