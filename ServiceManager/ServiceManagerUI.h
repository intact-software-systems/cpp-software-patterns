#ifndef ServiceManager_ServiceManagerUI_IsIncluded
#define ServiceManager_ServiceManagerUI_IsIncluded

#include "ServiceManager/ServiceManager.h"

namespace ServiceManagerSpace
{
// -------------------------------------------------------
//			class ServiceManagerUI
// -------------------------------------------------------
/**
* Class ServiceManagerCommand is a simple command interface to the ServiceManager.
*/
class ServiceManagerUI : public Thread
{
public:
	/**
	* Uses a ServiceManager reference to call ServiceManager's class functions for print, quit, etc.
	*
	* @param ls		The ServiceManager object.
	*/
	ServiceManagerUI(ServiceManager &ls);
	/**
	* Empty destructor.
	*/
	~ServiceManagerUI();

private:
	void printCommand();
	virtual void run();

private:
	ServiceManager &serviceManager;
};

} // namespace ServiceManagerSpace

#endif // ServiceManager_ServiceManagerUI_IsIncluded

