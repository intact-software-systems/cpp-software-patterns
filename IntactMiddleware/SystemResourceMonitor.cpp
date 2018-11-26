#include"IntactMiddleware/SystemResourceMonitor.h"
#include"IntactMiddleware/ServiceConnectionManager.h"

namespace IntactMiddleware
{
SystemResourceMonitor* SystemResourceMonitor::systemResourceMonitor_ = NULL;

/* -------------------------------------------------------
	class SystemResourceMonitor
-------------------------------------------------------*/
SystemResourceMonitor::SystemResourceMonitor(InterfaceHandle handle, bool autoStart)
	: runThread_(true)
{
	if(autoStart) start();
}


void SystemResourceMonitor::init(InterfaceHandle managerHandle)
{
	// TODO
}

SystemResourceMonitor* SystemResourceMonitor::GetOrCreate(InterfaceHandle *handle)
{
	if(SystemResourceMonitor::systemResourceMonitor_)
		return SystemResourceMonitor::systemResourceMonitor_;

	if(handle == NULL)
	{
		// TODO
	}
	else
	{
		SystemResourceMonitor::systemResourceMonitor_ = new SystemResourceMonitor(*handle);
	}

	return SystemResourceMonitor::systemResourceMonitor_;
}

/* -------------------------------------------------------
		run():

	- Retrieve data from managers
	- InterfaceManager (proxies/stubs)
	- SocketManager/FileDescriptorManager
	- ConnectionManager (TCP/UDP/sockets)
	- ThreadManager
	- ComponentManager
	- ObjectManager
-------------------------------------------------------*/
void SystemResourceMonitor::run()
{
	try
	{
		while(runThread_)
		{
			bool ret = waitCondition_.wait(&mutex_, 1000);
		
		}
	}
	catch(Exception ex)
	{
		IWARNING() << "Exception caught : " << ex.what();
	}

}

} // namespace IntactMiddleware


