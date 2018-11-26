#ifndef SystemManager_SystemMangerUI_h_IsIncluded
#define SystemManager_SystemMangerUI_h_IsIncluded

#include"SystemManager/IncludeExtLibs.h"
#include"SystemManager/SystemManager.h"

#include<map>
#include<fstream>

namespace SystemManagerSpace
{
// -------------------------------------------------------
//			class SystemManagerUI
//	- a simple command interface to the SystemManager
// -------------------------------------------------------
/**
* class SystemManagerUI is a simple command interface to the SystemManager.
*/
class SystemManagerUI : public Thread
{
public:
	/**
	* Constructor for SystemManagerUI.
	* Uses a SystemManager reference to call SystemManager's class functions for kill, print, etc.
	*
	* @param sm		The SystemManager object.
	*/
	SystemManagerUI(SystemManager &sm);
	
	/**
	* Empty destructor.
	*/
	~SystemManagerUI();

private:
	void printCommand();
	virtual void run();

private:
	SystemManager &systemManager;
};

} // namespace SystemManagerSpace

#endif //SystemManager_SystemMangerUI_h_IsIncluded

