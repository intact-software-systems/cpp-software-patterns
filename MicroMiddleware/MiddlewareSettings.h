#pragma once

#include<string>

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
/* -------------------------------------------------------
 		class MiddlewareSettings
 -------------------------------------------------------*/
class DLL_STATE MiddlewareSettings
{
private:
	MiddlewareSettings()
	{}
	~MiddlewareSettings()
	{}

public:
	static void Initialize(int argc, char **argv);
	static void Release();

public:
	static std::string GetHostName();

	// -- Services --
	static std::string 	GetRouteManagerService()		{ return routeManagerService_; }

	// -- Addresses --
	static std::string 	GetExceptionMulticastAddress()	{ return exceptionMulticastAddress_; }

	// -- Ports --
	static int			GetServiceManagerPort()			{ return serviceManagerPort_; }
	static int			GetSettingsManagerPort()		{ return settingsManagerPort_; }
	static int			GetSystemManagerPort()			{ return systemManagerPort_; }
	static int			GetProcessFactoryPort()			{ return processFactoryPort_; }
	static int			GetExceptionMulticastPort()		{ return exceptionMulticastPort_; }

private:
	// -- Services --
	static std::string 	routeManagerService_;

	// -- Addresses -
	static std::string 	exceptionMulticastAddress_;

	// -- Ports --
	static int 			serviceManagerPort_;
	static int 			settingsManagerPort_;
	static int 			systemManagerPort_;
	static int			processFactoryPort_;
	static int			exceptionMulticastPort_;
};

}
