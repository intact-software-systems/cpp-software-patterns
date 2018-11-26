#ifndef MonitorFactoryResources_h_IS_INCLUDED
#define MonitorFactoryResources_h_IS_INCLUDED

#include"SystemManager/IncludeExtLibs.h"
#include <map>

namespace SystemManagerSpace
{
/**
* Retrieves resource consumptions from component factories. 
* It is used internally in the SystemManager.
*/
class MonitorFactoryResources : public Thread
{
private:
	typedef map<string, ProcessFactoryProxy*>	MapProcessFactoryProxy;
	typedef map<string, HostResources>			MapComponentResources;
	typedef map<string, HostInformation>		MapComponentInformation;

private:
	MapComponentResources					mapComponentResources;
	MapComponentInformation					mapComponentFactories;
	
	MapComponentInformation					mapAddProcessFactory;
	MapComponentInformation					mapRemoveProcessFactory;

	int										monitorInterval;
	Mutex									guard;
	Mutex									updateFactoryProxyGuard;

	bool									monitorResources;

	// These are shared with system manager to synchronize factory communication.
	MapProcessFactoryProxy				    &mapHostProcessFactoryProxy;
	Mutex									&smMutexProxyGuard;

private:
	ProcessFactoryProxy*	getOrCreateFactoryProxy(const string &factoryHost, const int &factoryPort);
	void					updateFactoryMaps();

public:
	/**
	* Constructor for MonitorFactoryResources.
	*
	* @param mapFactoryProxy		Maps component factory name with ProcessFactoryProxy and is shared with SystemManager.
	* @param smMutex				Mutex shared with SystemManager to access and update mapFactoryProxy.
	* @param interval				Monitor interval decides how frequent resources are polled from each ProcessFactory.
	* @param autoStart				autoStarts thread, default is true.
	*/
	MonitorFactoryResources(MapProcessFactoryProxy &mapFactoryProxy, Mutex &smMutex, int interval = 1000, bool autoStart = true);

	/**
	* Stops resource monitor.
	*/
	~MonitorFactoryResources();

	/**
	* Main loop that polls resources from ComponentFactories.
	*/
	virtual void	run();

	/**
	* Shuts down monitor safely. Used by SystemManager.
	*/
	void			Quit();

	/**
	* Adds factory to monitor.
	*
	* @param info	HostInformation that describes ProcessFactory to add.
	*/
	void			AddFactory(const HostInformation &info);

	/**
	* Removes factory from monitoring.
	*
	* @param info	HostInformation that describes ProcessFactory to remove.
	*/
	void			RemoveFactory(const HostInformation &info);

	/**
	* Retrieves all resources from all ComponentFactories including components.
	*
	* @return string	String containing all resource information. Parse string with HostResourcesSpace::ParseHostResourcesFromString.
	*/
	string			GetAllFactoryResources();

	/**
	* Retrieves resources from given componentName.
	*
	* @return HostResources		HostResources for componentName.
	*/
	HostResources	GetHostResources(const string &componentName);

	/**
	* Retrieves all resources from all ComponentFactories including components.
	*
	* @param mapResources	Map of all the registered components with resource information.
	*/
	MapComponentResources GetMapAllResources();

	/**
	* Sets updated HostResources, received from ProcessFactory.
	*
	* @param HostInformation	Factory's HostInformation
	* @param HostResources		Updated HostResources.
	*/
	void SetHostResources(HostInformation &factoryInfo, map<int, HostResources> &mapHostResources);
};

} // namespace SystemManagerSpace

#endif // MonitorFactoryResources_h_IS_INCLUDED

