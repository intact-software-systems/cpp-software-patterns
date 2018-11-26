#include "IntactMiddleware/RpcMulticastManager.h"
#include "IntactMiddleware/GroupConnectionManager.h"

namespace IntactMiddleware
{

RpcMulticastManager::RpcMulticastManager(HostInformation hostInfo) 
	: Thread()
	, multicastServer_(NULL)
	, multicastClient_(NULL)
	, myHostInformation_(hostInfo)
	, runThread_(true)
{
	if(myHostInformation_.GetComponentName().empty())
		myHostInformation_ = Runtime::GetHostInformation();
	ASSERT(myHostInformation_.GetComponentName().empty() == false);

	start();
}

RpcMulticastManager::~RpcMulticastManager()
{
	StopMulticast();
}

bool RpcMulticastManager::StopAllThreads()
{
	mutexUpdate_.lock();
		runThread_ = false;
	waitCondition_.wakeAll();
	mutexUpdate_.unlock();

	return this->wait();
}

/*---------------------------------------------------------
	- keeps track of current GroupInformation view
		and updates multicastClient_ if it changes
-----------------------------------------------------------*/
void RpcMulticastManager::run()
{
	// Assume we are logged in to MembershipManager
	GroupConnectionManager *groupConnectionManager = GroupConnectionManager::GetOrCreate();
	groupConnectionManager->WaitConnected();

	try
	{
		mutexUpdate_.lock();

		IWARNING()  << "RUNNING!" ;
		while(runThread_)
		{
			bool ret = waitCondition_.wait(&mutexUpdate_, 1000);
			if(runThread_ == false) break;
			if(groupHandle_.IsValid() == false) continue;

			ASSERT(multicastServer_ && multicastClient_);

			GroupInformation currGroupInformation = groupConnectionManager->GetGroupConnection(groupHandle_.GetGroupName());
			currGroupInformation.RemoveHost(myHostInformation_.GetComponentName());

			bool equalInfo = currGroupInformation.CompareTo(groupInformation_);

			groupInformation_ = currGroupInformation;
			if(equalInfo == true) continue;

			IDEBUG() << "Group changed!" ;
			groupInformation_.Print();
			ASSERT(groupInformation_.IsValid());
			multicastClient_->AddOrRemoveMulticastClients(groupInformation_);
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

GroupInformation RpcMulticastManager::GetGroupInformation()
{
	MutexLocker lock(&mutexUpdate_);
	return groupInformation_;
}

bool RpcMulticastManager::StartMulticast(const InterfaceHandle &multicastServerHandle)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(multicastClient_ != NULL && multicastServer_ != NULL) 
	{
		IDEBUG() << "WARNING! multicasting already started!" ;
		return false;
	}

	ASSERT(multicastClient_ == NULL);
	ASSERT(multicastServer_ == NULL);

	multicastServer_ = new RpcMulticastServerListener(multicastServerHandle, multicastClient_, true);
	multicastServer_->WaitStarted();

	multicastServerHandle_ = multicastServer_->GetInterfaceHandle();
	ASSERT(multicastServerHandle_.IsValid());

	IDEBUG() << " Server handle " << multicastServerHandle_ ;

	multicastClient_ = new RpcMulticastClient(multicastServerHandle_);
	multicastServer_->SetMulticastClient(multicastClient_);

	if(groupInformation_.IsValid())
	{
		groupInformation_.RemoveHost(myHostInformation_.GetComponentName());
		multicastClient_->AddOrRemoveMulticastClients(groupInformation_);
	}

	runThread_ = true;
	if(isRunning() == false) start();

	// TODO: Restart with new handle if already started!
	waitCondition_.wakeAll();

	return true;
}

bool RpcMulticastManager::StopMulticast()
{
	try
	{
		mutexUpdate_.lock();

		if(multicastClient_ == NULL && multicastServer_ == NULL) 
		{
			IDEBUG() << "WARNING! multicasting already stopped!" ;
			ASSERT(groupHandle_.IsValid() == false);
			mutexUpdate_.unlock();
			return false;
		}

		runThread_ = false;
		waitCondition_.wakeAll();
		mutexUpdate_.unlock();

		this->wait();

		mutexUpdate_.lock();

		if(multicastClient_ != NULL) delete multicastClient_;
		if(multicastServer_ != NULL) delete multicastServer_;
		
		multicastClient_ = NULL;
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

bool RpcMulticastManager::RegisterInterface(int interfaceId, RpcServer *rpcServer)
{
	MutexLocker lock(&mutexUpdate_);

	ASSERT(rpcServer);
	if(multicastServer_ == NULL) return false;

	bool ret = multicastServer_->RegisterInterface(interfaceId, rpcServer);
	if(ret) waitCondition_.wakeAll();
	return ret;
}

bool RpcMulticastManager::JoinGroup(string groupName)
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

