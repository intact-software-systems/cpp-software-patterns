#ifndef IntactMiddleware_MiddlewareSettings_h_IsIncluded
#define IntactMiddleware_MiddlewareSettings_h_IsIncluded

#include<string>

namespace IntactMiddleware
{
/* -------------------------------------------------------
 		class MiddlewareSettings
 -------------------------------------------------------*/
class MiddlewareSettings
{
private:
	MiddlewareSettings() 
	{}
	~MiddlewareSettings()
	{}
	
public:
	static void Initialize(int argc, char **argv);
	static void Release();

	static void ConnectInfra(bool waitConnected);
	static bool ConnectSettingsManager();

public:
	static std::string GetNetworkInformationManagerService()	{ return networkInformationManagerService_; }
	static std::string GetSettingsManagerService()				{ return settingsManagerService_; }
	static std::string GetIntactManagerService()				{ return intactManagerService_; }
	static std::string GetServiceManagerService()				{ return serviceManagerService_; }
	static std::string GetGroupManagerService()					{ return groupManagerService_; }
	static std::string GetMembershipManagerService()			{ return membershipManagerService_; }
	static std::string GetNetworkInformationGroup()				{ return networkInformationGroup_; }

private:
	static std::string 	networkInformationManagerService_;
	static std::string 	settingsManagerService_;
	static std::string  intactManagerService_;
	static std::string  serviceManagerService_;
	static std::string  groupManagerService_;
	static std::string	membershipManagerService_;

	static std::string 	networkInformationGroup_;
};

}; // namespace IntactMiddleware

#endif //

