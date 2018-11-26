#ifndef ResourceMonitor_h_IS_INCLUDED
#define ResourceMonitor_h_IS_INCLUDED

#include "ProcessFactory/IncludeExtLibs.h"

#ifndef USE_GCC
#include "PdhQuery.h"
#endif

#include <map>

using namespace std;

namespace ProcessFactorySpace
{
    
class NotifySystemManagerThread;

/**
* Retrieves resource consumptions from each component registered on the ProcessFactory,
* and the node resources. It is used internally in the ProcessFactory.
*/

class ResourceMonitor : public Thread
{
private:
	typedef map<int, HostResources>			MapComponentResources;
	MapComponentResources					mapComponentResources;

	string									nodeHostName;
	int										monitorInterval;
	int										processId;
	Mutex									guard;
	bool									monitorResources;
	NotifySystemManagerThread				*notifySystemManagerThread;

#ifndef USE_GCC
	typedef map<int, PdhQuery*>				MapPidPdhQuery;
	MapPidPdhQuery							mapPidPdhQuery;
#endif

private:
	void			runOnWindows();
	void			runOnLinux();
	void			addNodeProcess();

#ifndef USE_GCC
	void			setNodeResources(PdhQuery *query, HostResources &res);
	void			setProcessResources(PdhQuery *query, HostResources &res);
#endif

public:
	/**
	* Constructor for ResourceMonitor.
	*
	* @param hostName				ProcessFactory's hostName.
	* @param interval				Monitor interval decides how frequent resources are polled.
	* @param autoStart				autoStarts thread, default is true.
	*/
	ResourceMonitor(string hostName, int interval = 1000, bool autoStart = true);
	
	/**
	* Stops resource monitor.
	*/
	~ResourceMonitor();

	inline void SetNotifySystemManagerThread(NotifySystemManagerThread *notify) { MutexLocker lock(&guard); notifySystemManagerThread = notify; }

	/**
	* Main loop that polls resources from node and components.
	*/
	virtual void run();

	/**
	* Adds process to monitor.
	*/
	void			AddProcess(const HostInformation &info);

	/**
	* Removes process to monitor.
	*/
	void			RemoveProcess(const HostInformation &info);

	/**
	* Retrives HostResources from process.
	*
	* @param id		Process identification (pid).
	* @return		HostResources for given id.
	*/
	HostResources	GetResources(int id = -1);

	/**
	* Retrieves all resources including components.
	*
	* @return string	String containing all resource information. Parse string with HostResourcesSpace::ParseHostResourcesFromString.
	*/
	string			GetStringAllResources();

	/**
	* Returns a map (pid, HostResources) of all resource information on the ProcessFactory.
	*
	* @return MapComponentResources		Map of all resource information (pid, HostResources).
	*/
	MapComponentResources GetMapAllResources();
};

} // namespace ProcessFactorySpace

#endif // ResourceMonitor_h_IS_INCLUDED

