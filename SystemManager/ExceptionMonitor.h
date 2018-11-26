#ifndef SystemManager_ExceptionMonitor_IsIncluded
#define SystemManager_ExceptionMonitor_IsIncluded

#include"SystemManager/IncludeExtLibs.h"
#include<map>
#include<fstream>

namespace SystemManagerSpace
{
// --------------------------------------------------------
//			class ExceptionMonitor
// --------------------------------------------------------
/**
* ExceptionMonitor's main functionality: 
*	- Receive exceptions that occured on components
*	- Store exceptions and make available for SystemManagerGUI
*/
class ExceptionMonitor : public ExceptionMulticastInterface,
						public Thread
{
private:
	typedef map<long long, ExceptionMessage>	MapTimeStampExceptionMessage;
	typedef vector<ExceptionMessage>			VectorTimeStampExceptionMessage;

private:
	//MapTimeStampExceptionMessage	mapTimeStampExceptionMessage;
	VectorTimeStampExceptionMessage vectorTimeStampExceptionMessage;
	Mutex							updateLock;
	ExceptionMulticastSubscriber	*exceptionSubscriber;
	bool							stopExceptionMonitor;
	
	/**
	* Main thread-loop for the ExceptionMonitor. 
	*/
	virtual void run();

	/**
	* Returns InterfaceHandle for ExceptionMonitor. Dummy implementation.
	*
	* @return InterfaceHandle		The SystemManager's InterfaceHandle (Port, Host).
	*/
	inline virtual InterfaceHandle GetInterfaceHandle() { return InterfaceHandle(); }

public:
	/**
	* Constructor for ExceptionMonitor.
	*
	* @param name		The ExceptionMonitor name is only used by Component class.
	*/
	ExceptionMonitor(string name);
	
	/**
	* Shuts down ExceptionMonitor.
	*/
	~ExceptionMonitor();

	/**
	* Prints the ExceptionMonitor's database.
	*/
	void PrintAll(std::ostream &ostr);

	/**
	* Clears ExceptionMonitor's database.
	*/
	void ClearAll();

	/**
	* SystemManager receives exceptions that occured on components implemented in LightweightMiddleware.
	*
	* @param exceptionMessage	Contains the HostInformation of the component where the exception occured, 
	*							and a message that describes the exception.
	*/
	virtual void PostException(ExceptionMessage &exceptionMessage);

	/**
	* Returns middleware exceptions that have been multicast by components and received by SystemManager.
	*
	* @param lastTimeStamp		Timestamp of last received exception message, can be empty if no previously received exception message.
	* @return					map containing middleware exceptions received by SystemManager, organized by timestamp.
	*/
	MapTimeStampExceptionMessage GetMiddlewareExceptions(long long &lastTimeStamp);
};
} // namespace SystemManagerSpace

#endif // EXCEPTION_MONITOR_H

