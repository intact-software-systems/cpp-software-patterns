#include "MicroMiddleware/INet/INetInterfaceManager.h"
#include "MicroMiddleware/INet/INetStub.h"
#include "MicroMiddleware/INet/INetProxy.h"

namespace MicroMiddleware
{

INetInterfaceManager* INetInterfaceManager::interfaceManager_ = NULL;

/*-----------------------------------------------------------------------
               INetInterfaceManager constructor/destructor 
------------------------------------------------------------------------- */
INetInterfaceManager::INetInterfaceManager(const std::string &name) 
	: Thread(name)
	, runThread_(true)
{
	start();
}

INetInterfaceManager::~INetInterfaceManager()
{

}

/*-----------------------------------------------------------------------
              static create functions 
------------------------------------------------------------------------- */
INetInterfaceManager* INetInterfaceManager::GetOrCreate()
{
	static Mutex mutex;
	MutexLocker lock(&mutex);

	if(INetInterfaceManager::interfaceManager_ == NULL)
		INetInterfaceManager::interfaceManager_ = new INetInterfaceManager();

	return INetInterfaceManager::interfaceManager_;
}

/*-----------------------------------------------------------------------
             main thread function 
	TODO:
------------------------------------------------------------------------- */
void INetInterfaceManager::run()
{
	try
	{
		IDEBUG() << "running";
		mutex_.lock();

		while(runThread_)
		{
			bool ret = waitCondition_.wait(&mutex_, 1000);
			if(!runThread_) break;
		}
	}
	catch(Exception ex)
	{
		ICRITICAL() << "ERROR! Exception caught " << ex.what();
	}
	catch(...)
	{
		IFATAL() << "INetInterfaceManager::run(): ERROR! Unhandled exception caught";
	}

	mutex_.unlock();
}
// -----------------------------------------------------------------------------------
//	Register and remove proxies and stubs 
// -----------------------------------------------------------------------------------
bool INetInterfaceManager::Register(INetInterface *inet)
{
	MutexLocker lock(&mutex_);
	return true;	
}

bool INetInterfaceManager::Remove(INetInterface *inet)
{
	MutexLocker lock(&mutex_);
	return true;
}


} // namespace MicroMiddleware


