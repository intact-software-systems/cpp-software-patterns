#include "IntactMiddleware/INetMulticastManager.h"
#include "IntactMiddleware/GroupConnectionManager.h"

namespace IntactMiddleware
{

INetMulticastManager::INetMulticastManager(IntactComponent *intactComponent, HostInformation hostInfo)
	: Thread("IntactMiddleware.INetMulticastManager")
	, intactComponent_(intactComponent)
	, multicastStub_(NULL)
	, multicastServer_(NULL)
	, multicastProxy_(NULL)
	, myHostInformation_(hostInfo)
	, runThread_(true)
{
	if(myHostInformation_.GetComponentName().empty())
		myHostInformation_ = Runtime::GetHostInformation();
	ASSERT(myHostInformation_.GetComponentName().empty() == false);

	start();
}

INetMulticastManager::~INetMulticastManager()
{
	StopMulticast();
}

bool INetMulticastManager::StopAllThreads()
{
	mutexUpdate_.lock();
		runThread_ = false;
	waitCondition_.wakeAll();
	mutexUpdate_.unlock();

	return this->wait();
}

/*---------------------------------------------------------
	- keeps track of current GroupInformation view
		and updates multicastProxy_ if it changes
-----------------------------------------------------------*/
void INetMulticastManager::run()
{
	// Assume we are logged in to MembershipManager
	GroupConnectionManager *groupConnectionManager = GroupConnectionManager::GetOrCreate();
	groupConnectionManager->WaitConnected();

	try
	{
		mutexUpdate_.lock();

		IDEBUG() << "RUNNING!";

		while(runThread_)
		{
			bool ret = waitCondition_.wait(&mutexUpdate_, 1000);
			if(runThread_ == false) break;
			if(groupHandle_.IsValid() == false) continue;

			ASSERT(multicastServer_ && multicastProxy_);

			GroupInformation currGroupInformation = groupConnectionManager->GetGroupConnection(groupHandle_.GetGroupName());
			currGroupInformation.RemoveHost(myHostInformation_.GetComponentName());

			bool equalInfo = currGroupInformation.CompareTo(groupInformation_);

			groupInformation_ = currGroupInformation;
			if(equalInfo == true) continue;

			// -- debug start --
			IDEBUG() << "Group changed!";
			groupInformation_.Print();
			ASSERT(groupInformation_.IsValid());
			ASSERT(interfaceName_.empty() == false);
			// -- debug end --

			multicastProxy_->RegisterOrRemoveProxies(groupInformation_, interfaceName_);

			// TODO: Get current list of registered proxies and update ServiceConnectionManager!
			// TODO: iterate through proxies and add as SubscriberServiceInformation
			// TODO: also get removed proxies and remove as subscribers to services!
		}
	}
	catch(Exception ex)
	{
		IDEBUG() << "Exception caught " << ex.msg() ;
	}
	catch(...)
	{
		IDEBUG() << "Unhandled cxception caught!" ;
	}

	IWARNING()  << "ENDING!" ;
}

GroupInformation INetMulticastManager::GetGroupInformation()
{
	MutexLocker lock(&mutexUpdate_);
	return groupInformation_;
}

bool INetMulticastManager::StartMulticast(INetStub *stub, const std::string &interfaceName, const InterfaceHandle &multicastServerHandle)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(multicastProxy_ != NULL && multicastServer_ != NULL) 
	{
		IWARNING() << "WARNING! multicasting already started!";
		return false;
	}

	// -- debug --
	ASSERT(multicastProxy_ == NULL);
	ASSERT(multicastServer_ == NULL);
	ASSERT(multicastStub_ == NULL);
	ASSERT(interfaceName_.empty());
	ASSERT(interfaceName.empty() == false);
	// -- debug --

	// -- Initialize multicast server --
	INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
	ASSERT(connectionManager);

	multicastServer_ = connectionManager->GetOrCreateServerListener(false, multicastServerHandle_);
	ASSERT(multicastServer_);
	
	multicastServerHandle_ = multicastServer_->GetInterfaceHandle();

	// -- debug --
	IDEBUG()  << " Server handle " << multicastServerHandle_;
	ASSERT(multicastServerHandle_.IsValid());
	// -- debug --
	
	bool ret = connectionManager->RegisterInterface(multicastServerHandle_, stub);
	ASSERT(ret);

	multicastStub_ = stub;

	// -- Initialize multicastProxy --
	multicastProxy_ = new INetMulticastProxy();
	interfaceName_ = interfaceName;

	if(groupInformation_.IsValid())
	{
		groupInformation_.RemoveHost(myHostInformation_.GetComponentName());
		multicastProxy_->RegisterOrRemoveProxies(groupInformation_, interfaceName_);
	}

	// -- start thread --
	runThread_ = true;
	if(isRunning() == false) start();

	waitCondition_.wakeAll();

	return true;
}

bool INetMulticastManager::StopMulticast()
{
	try
	{
		mutexUpdate_.lock();

		if(multicastProxy_ == NULL && multicastServer_ == NULL) 
		{
			IDEBUG() << "WARNING! multicasting already stopped!" ;
			ASSERT(groupHandle_.IsValid() == false);
			mutexUpdate_.unlock();
			return false;
		}

		ASSERT(multicastProxy_ != NULL);
		ASSERT(multicastServer_ != NULL);
		ASSERT(multicastStub_ != NULL);
		
		runThread_ = false;
		waitCondition_.wakeAll();
		mutexUpdate_.unlock();

		this->wait();

		mutexUpdate_.lock();
		
		ASSERT(multicastServer_);
		multicastServer_->Stop(true);		// waits until TCP Server is listening

		delete multicastProxy_;
		delete multicastServer_;
		
		multicastProxy_ = NULL;
		multicastServer_ = NULL;

		multicastServerHandle_ = InterfaceHandle();

		if(groupHandle_.IsValid())
		{
			GroupConnectionManager *groupConnectionManager = GroupConnectionManager::GetOrCreate();
			groupConnectionManager->LeaveGroup(groupHandle_);
		}
	}
	catch(...)
	{
		IDEBUG() << "ERROR! Unhandled exception caught!" ;
		mutexUpdate_.unlock();
		return false;
	}

	mutexUpdate_.unlock();
	return true;
}

bool INetMulticastManager::JoinGroup(string groupName)
{
	ASSERT(groupName.empty() == false);

	MutexLocker lock(&mutexUpdate_);
	
	if(groupHandle_.IsValid())
	{
		IDEBUG() << "WARNING! Already joined " << groupHandle_ << "! Implement leaving and joining? " ;
		return false;
	}
	if(multicastServer_ == NULL) 
	{
		IDEBUG() << "ERROR! Start multicasting before joining group!" ;
		return false;
	}
	if(multicastServerHandle_.IsValid() == false)
	{
		IDEBUG() << "ERROR! Start multicastServer before joining group!" ;
		return false;
	}

	// TODO: if already registered in a group then
	// 1. Leave current group
	// 2. Join new group
	GroupConnectionManager *groupConnectionManager = GroupConnectionManager::GetOrCreate();
	groupConnectionManager->WaitConnected();

	groupHandle_ = GroupHandle(groupName, multicastServerHandle_);
	ASSERT(groupHandle_.IsValid());
	
	IDEBUG() << " Group handle " << groupHandle_ ;
	
	groupConnectionManager->JoinGroup(groupHandle_);
	
	waitCondition_.wakeAll();
	return true;
}

} // namespace IntactMiddleware


