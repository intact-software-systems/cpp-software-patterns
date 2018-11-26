#include "MicroMiddleware/MiddlewareSettings.h"
#include "MicroMiddleware/Runtime.h"
#include "MicroMiddleware/INet/INetConnectionManager.h"

namespace MicroMiddleware
{
/* -------------------------------------------------------
 	class MiddlewareSettings
 	- Initialize static variables
	
	- Variables are taken from CommonDefines.h

	#define LOOKUP_SERVER_PORT			5000
	#define SYSTEM_MANAGER_PORT			8888
	#define SETTINGS_MANAGER_PORT		6789
	#define COMPONENT_FACTORY_PORT		49999
	#define EXCEPTION_MULTICAST_PORT	37778
	#define EXCEPTION_MULTICAST_ADDRESS "237.3.4.101"
-------------------------------------------------------*/

// -- MicroMiddleware Services --
std::string MiddlewareSettings::routeManagerService_		= std::string("RouteManager.stub");

// -- addresses -
std::string MiddlewareSettings::exceptionMulticastAddress_	= std::string("237.3.4.101");

// -- ports --
int 		MiddlewareSettings::serviceManagerPort_			= 5000;
int 		MiddlewareSettings::settingsManagerPort_		= 6789;
int 		MiddlewareSettings::systemManagerPort_			= 8888;
int			MiddlewareSettings::processFactoryPort_			= 49999;
int			MiddlewareSettings::exceptionMulticastPort_		= 37778;

/* -------------------------------------------------------
   Initialize MicroMiddleware
 -------------------------------------------------------*/
void MiddlewareSettings::Initialize(int argc, char **argv)
{
	MicroMiddleware::Runtime::Initialize(argc, argv);

	MicroMiddleware::INetConnectionManager::GetOrCreate();
	//MicroMiddleware::INetNATManager::GetOrCreate();
}

void MiddlewareSettings::Release()
{
	MicroMiddleware::Runtime::Release();
}

std::string MiddlewareSettings::GetHostName()
{
	return TcpServer::GetLocalHostName();
}

} // namespace MicroMiddleware

