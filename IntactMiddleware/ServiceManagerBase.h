#ifndef IntactMiddleware_ServiceManagerBase_h_IsIncluded
#define IntactMiddleware_ServiceManagerBase_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{
// -------------------------------------------------------
//				ServiceManager Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the ServiceManager.
* The interface is used actively by each service implemented in a component.
*/
class DLL_STATE ServiceManagerInterface : public RmiInterface
{
public:
    typedef std::map<std::string, HostInformation>		MapServiceNameHostInformation;
    typedef std::set<std::string>						SetServiceName;

public:
	ServiceManagerInterface() { }
	~ServiceManagerInterface() { }

	enum MethodID {
		Method_ServiceRegistrationMap = 9,
		Method_ServiceRegistration = 10,
		Method_ServiceInformation = 11,
		Method_AliveChecker = 12,
		Method_AliveCheckerMaps = 13,
		Method_ShutDown = 14,
		Method_AllServiceInformation = 19,
		Method_ClearServices = 31,
	};

	enum InterfaceId
	{
		ServiceManagerInterfaceId = 10
	};

	virtual int GetInterfaceId() const { return ServiceManagerInterface::ServiceManagerInterfaceId; }

	/**
	* Registers the service given by HostInformation in the ServiceManager.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostInformation.
	*/
	virtual HostInformation ServiceRegistration(HostInformation info) = 0;
	
	/**
	* Registers or unregisters services in the ServiceManager.
	*/
	virtual void ServiceRegistration(MapServiceNameHostInformation &mapServices) = 0;

	/**
	* Retrieve HostInformation about a service identified by componentName.
	*
	* @param componentName	The name of the component (service) to retrieve HostInformation.
	* @return				HostInformation about the componentName.
	*/
    virtual HostInformation GetServiceInformation(std::string componentName) = 0;

	/**
	* Called by all component services to inform the ServiceManager that it is connected/disconnected.
	* The componentName is used if the caller requires information whether a service is currently online.
	*
	* @param info			The (callers) service' HostInformation.
	* @param componentName	Name of a service that the caller is inquiring whether it is online or not.
	* @return				componentName's HostInformation.
	*/
    virtual HostInformation	AliveChecker(HostInformation info, std::string componentName) = 0;

	/**
	* Called by ServiceConnectionManager to register service, and retrieve information about services.
	*/
    virtual std::map<std::string, HostInformation> AliveChecker(MapServiceNameHostInformation &map, SetServiceName &setS) = 0;

	/**
	* Shuts down the ServiceManager.
	*/
	virtual void ShutDown() = 0;
	
	/**
	* Returns all service information in the ServiceManager in a map (string, HostInformatino).
	*
	* @return				Map containing all service information.
	*/
    virtual map<std::string, HostInformation> GetAllServiceInformation() = 0;
	
	/**
	* Clears Offline Services.
	*/
	virtual void ClearServices() = 0;
};

// -------------------------------------------------------
//				Interface RmiClient
//			- Sender/publisher class
// -------------------------------------------------------
class DLL_STATE ServiceManagerProxy : virtual public RmiClient, virtual public ServiceManagerInterface
{
public:
    typedef std::map<std::string, HostInformation>		MapServiceNameHostInformation;
    typedef std::set<std::string>						SetServiceName;

public:
	RMICLIENT_COMMON(ServiceManagerProxy);

	virtual HostInformation ServiceRegistration(HostInformation info);
    virtual HostInformation GetServiceInformation(std::string componentName);
	virtual void ServiceRegistration(MapServiceNameHostInformation &mapService);
    virtual HostInformation AliveChecker(HostInformation info, std::string componentName);
    virtual std::map<std::string, HostInformation> AliveChecker(MapServiceNameHostInformation &mapService, SetServiceName &setNames);
	virtual void ShutDown();
    virtual map<std::string, HostInformation> GetAllServiceInformation();
	virtual void ClearServices();
};

// -------------------------------------------------------
//				ServiceManager Stub
//			- Receiver/consumer class
// -------------------------------------------------------
class DLL_STATE ServiceManagerStub : virtual public RmiServerListener
{
public:
    typedef std::map<std::string, HostInformation>		MapServiceNameHostInformation;
    typedef std::set<std::string>						SetServiceName;

public:
	ServiceManagerStub(const InterfaceHandle &handle, ServiceManagerInterface* serverInterface, bool autoStart)
		: RmiServerListener(handle, autoStart)
		, serviceManagerInterface(serverInterface)
	{ }
	virtual ~ServiceManagerStub() {}

	virtual void RMInvocation(int methodId, RmiBase* stubClient);

private:
	ServiceManagerInterface *serviceManagerInterface;
};


} // namespace end

#endif //IntactMiddleware_ServiceManagerBase_h_IsIncluded

