#include "IntactMiddleware/IncludeExtLibs.h"

#include "IntactMiddleware/IntactSettings/IncludeLibs.h"
#include "IntactMiddleware/ServiceConnectionManager.h"
#include "IntactMiddleware/GroupConnectionManager.h"
#include "IntactMiddleware/MembershipManagerConnection.h"

namespace IntactMiddleware {
/* -------------------------------------------------------
 		class IntactSettings
	
#define SETTINGS_MANAGER_NAME		"component.SettingsManager"
 -------------------------------------------------------*/

// -- Intact Services --
std::string IntactSettings::networkInformationManagerService_ = std::string("NetworkInformationManager.stub");
std::string IntactSettings::settingsManagerService_           = std::string("SettingsManager.stub");
std::string IntactSettings::intactManagerService_             = std::string("IntactManager.stub");
std::string IntactSettings::serviceManagerService_            = std::string("ServiceManager.stub");
std::string IntactSettings::groupManagerService_              = std::string("GroupManager.stub");
std::string IntactSettings::membershipManagerService_         = std::string("MembershipManager.stub");

// -- Intact Groups --
std::string IntactSettings::networkInformationGroup_ = std::string("NetworkInformation.vivaldi.client.group");

// -- Intact Components --
std::string IntactSettings::settingsManagerComponentName_ = std::string("SettingsManager.component");


/* -------------------------------------------------------
 		Connect to Intact Infrastructure
 -------------------------------------------------------*/
void IntactSettings::Initialize(int argc, char** argv, bool connectToRouteManager)
{
    MicroMiddleware::MiddlewareSettings::Initialize(argc, argv);
    IntactSettings::ConnectInfra(false);

    if(connectToRouteManager)
    {
        // -- Register INetManager as a subscriber to RouteManager service --
        string proxyName(MicroMiddleware::Runtime::GetComponentName() + ".proxy." + MicroMiddleware::MiddlewareSettings::GetRouteManagerService());
        RouteManagerProxy* routeManagerProxy = new RouteManagerProxy(proxyName, MicroMiddleware::MiddlewareSettings::GetRouteManagerService());

        bool ret = ServiceConnectionManager::GetOrCreate()->SubscribeService(routeManagerProxy, false);
        ASSERT(ret);

        MicroMiddleware::INetNATManager* natManager = MicroMiddleware::INetNATManager::GetOrCreate();
        natManager->SetRouteManager(routeManagerProxy);
    }
}

void IntactSettings::Release()
{
    iWarning() << COMPONENT_FUNCTION << "TODO: Release infrastructure!";
    MicroMiddleware::MiddlewareSettings::Release();
}

/* -------------------------------------------------------
 		Connect to Intact Infrastructure
 -------------------------------------------------------*/
void IntactSettings::ConnectInfra(bool waitConnected)
{
    static Mutex mutex;
    MutexLocker  lock(&mutex);

    // 1. Create connection to ServiceManager
    {
        ServiceConnectionManager* service = ServiceConnectionManager::GetOrCreate();
        ASSERT(service);
        if(waitConnected)
        {
            iDebug() << COMPONENT_FUNCTION << " Waiting for connection to service manager ";
            service->WaitConnected();
        }
    }

    // 2. Connect and login to MembershipManager
    {
        MembershipManagerConnection* membershipProxy = MembershipManagerConnection::GetOrCreate();
        ASSERT(membershipProxy);
        if(waitConnected)
        {
            iDebug() << COMPONENT_FUNCTION << " Waiting for connection to membership manager ";
            membershipProxy->WaitConnected();
        }
    }

    // 3. Create connection to GroupManager
    {
        GroupConnectionManager* group = GroupConnectionManager::GetOrCreate();
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
bool IntactSettings::ConnectSettingsManager()
{
    ServiceConnectionManager* serviceManagerConnection = ServiceConnectionManager::GetOrCreate();

    HostInformation serviceInformation;
    bool            waitConnected                      = true;
    serviceManagerConnection->SubscribeService(IntactSettings::GetSettingsManagerService(), serviceInformation, waitConnected);

    if(!serviceInformation.IsValid()) return false;

    HostInformation myHostInformation = Runtime::GetHostInformation();
    InterfaceHandle settingsManagerHandle(serviceInformation.GetPortNumber(), serviceInformation.GetHostIP());
    bool            ret               = SettingsManagerConnection::SetupSettingsManagerConnections(settingsManagerHandle, myHostInformation);

    if(!ret)
    {
        iWarning() << COMPONENT_FUNCTION << "WARNING! Can't connect to SettingsManager!";
    }

    SettingsListener* settingsListener = SettingsListener::GetOrCreateSettingsListener();
    if(settingsListener == NULL)
    {
        iCritical() << COMPONENT_FUNCTION << "SettingsListener == NULL!! ";
        return false;
    }
    return true;
}

} // namespace IntactMiddleware

