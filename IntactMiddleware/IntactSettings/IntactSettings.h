#pragma once

#include<string>

#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{
/* -------------------------------------------------------
 		class IntactSettings
 -------------------------------------------------------*/
class DLL_STATE IntactSettings
{
private:
	IntactSettings() 
	{}
	~IntactSettings()
	{}
	
public:
	static void Initialize(int argc, char **argv, bool connectToRouteManager = true);
	static void Release();

	static void ConnectInfra(bool waitConnected);
	static bool ConnectSettingsManager();

	// -- Intact Services --
	static std::string GetNetworkInformationManagerService()	{ return networkInformationManagerService_; }
	static std::string GetSettingsManagerService()				{ return settingsManagerService_; }
	static std::string GetIntactManagerService()				{ return intactManagerService_; }
	static std::string GetServiceManagerService()				{ return serviceManagerService_; }
	static std::string GetGroupManagerService()					{ return groupManagerService_; }
	static std::string GetMembershipManagerService()			{ return membershipManagerService_; }
	
	// -- Intact Groups --
	static std::string GetNetworkInformationGroup()				{ return networkInformationGroup_; }

	// -- Intact Components --
	static std::string GetSettingsManagerComponentName()		{ return settingsManagerComponentName_; }

private:
	// -- Intact Services --
	static std::string 	networkInformationManagerService_;
	static std::string 	settingsManagerService_;
	static std::string  intactManagerService_;
	static std::string  serviceManagerService_;
	static std::string  groupManagerService_;
	static std::string	membershipManagerService_;

	// -- Intact Groups --
	static std::string 	networkInformationGroup_;

	// -- Intact Components --
	static std::string 	settingsManagerComponentName_;
};

}


