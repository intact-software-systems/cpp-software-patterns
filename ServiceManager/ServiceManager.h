#ifndef ServiceManager_ServiceManager_IsIncluded
#define ServiceManager_ServiceManager_IsIncluded

#include<map>
#include<list>
#include<sstream>
#include<fstream>

#include "ServiceManager/IncludeExtLibs.h"

namespace ServiceManagerSpace
{

// --------------------------------------------------------
//			struct ServiceManagerConfig
// --------------------------------------------------------
struct ServiceManagerConfig
{
	static int serviceManagerPort;
	static string hostInformationFile;
};

class ServiceManagerUI;

// --------------------------------------------------------
//			class ServiceManager
// --------------------------------------------------------
/**
* Class ServiceManager keeps track of the available services in each system.
* It monitors the running state of each service, which is either ONLINE or OFFLINE.
*/
class ServiceManager : public ServiceManagerInterface, public Thread
{
private:
	typedef map<string, HostInformation>		MapHostInformation;
	typedef map<string, int>					MapAliveCheck;	
	typedef list<string>						ListAddresses;
	//typedef std::map<string, HostInformation>	MapServiceNameHostInformation;
	typedef std::set<string>					SetServiceName;

	MapHostInformation				mapHostInformation;
	ListAddresses					multicastAddresses;
	ListAddresses					usedMulticastAddresses;
	static int						portNumberAssigner;
	Mutex							updateLock;
	
	MapAliveCheck					mapAliveCheck;
	int								timeCounter;
	bool							stopServiceManager;
	ServiceManagerUI				*serviceManagerUI;
	
	ofstream						outFile;
	string							fileName;

private:
	void cmdPrintHostInformation(ostream &ostr);

private:
	inline void Quit()	{ MutexLocker lock(&updateLock); stopServiceManager = true; }

private:
	void initServiceManager();

	void serviceRegistration(HostInformation &info);
	void publisherOnline(HostInformation &info);
	void publisherOffline(HostInformation &info);
	void hostOnline(HostInformation &info);
	void hostOffline(HostInformation &info);

	HostInformation*	findHostInformation(const HostInformation &info);
	void				removeHostInformation(const HostInformation &info);
	void				parseHostInformationFromDisc(string &filename);
	void				recycleMulticastAddress(const string &multicastAddress);
	void				usedMulticastAddress(const string &multicastAddress);

public:
	/**
	* Constructor for ServiceManager.
	*
	* @param name		The ServiceManager name is only used by Component class.
	*/
	ServiceManager(const string &name);
	
	/**
	* Shuts down ServiceManager.
	*/
	virtual ~ServiceManager(void);
	
	/**
	* Allows ServiceManagerUI to access private functions in ServiceManager.
	*/
	friend class ServiceManagerUI;

	/**
	* Returns InterfaceHandle for SystemManager.
	*
	* @return InterfaceHandle		The SystemManager's InterfaceHandle (Port, Host).
	*/
	inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(ServiceManagerConfig::serviceManagerPort, MicroMiddleware::MiddlewareSettings::GetHostName()); }

	/**
	* Main thread-loop for the ServiceManager. 
	* Maintains a list of running services in the system.
	*/
	virtual void run();
	
	/**
	* Registers the service given by HostInformation in the ServiceManager.
	*
	* @param info		HostInformation includes the component descriptions.
	* @return			Updated HostInformation.
	*/
	virtual HostInformation ServiceRegistration(HostInformation info);
	
	/**
	* Registers or unregisters services in the ServiceManager.
	*/
	virtual void ServiceRegistration(MapHostInformation &mapServices);

	/**
	* Retrieve HostInformation about a service identified by serviceName.
	*
	* @param serviceName	The name of the component (service) to retrieve HostInformation.
	* @return				HostInformation about the serviceName.
	*/
	virtual HostInformation GetServiceInformation(string serviceName);

	/**
	* Called by all component services to inform the ServiceManager that it is connected/disconnected.
	* The serviceName is used if the caller requires information whether a service is currently online.
	*
	* @param info			The (callers) service' HostInformation.
	* @param serviceName	Name of a service that the caller is inquiring whether it is online or not.
	* @return				serviceName's HostInformation.
	*/
	virtual HostInformation AliveChecker(HostInformation info, string serviceName);	

	/**
	* Called by ServiceConnectionManager to register service, and retrieve information about services.
	*/
	virtual MapHostInformation AliveChecker(MapHostInformation &mapService, SetServiceName &setNames);

	/**
	* Shuts down the ServiceManager.
	*/
	virtual void ShutDown();

	/**
	* Clears Offline Services.
	*/
	virtual void ClearServices();

	/**
	* Returns all service information in the ServiceManager in a map (string, HostInformation).
	*
	* @return				Map containing all service information.
	*/
	virtual map<string, HostInformation> GetAllServiceInformation();
};

} // namespace ServiceManagerSpace

#endif // ServiceManager_ServiceManager_IsIncluded


