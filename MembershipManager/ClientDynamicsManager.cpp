#include "MembershipManager/ClientDynamicsManager.h"

ClientDynamicsManager::ClientDynamicsManager() 
	: membershipManagerConnection_(NULL)
	, groupManagerConnection_(NULL)
	, multicastManager_(NULL)
	, runThread_(true)
{
	start();
}

ClientDynamicsManager::~ClientDynamicsManager()
{
	StopAllThreads();
	
	MutexLocker lock(&mutexUpdate_);

	if(membershipManagerConnection_) 	delete membershipManagerConnection_;
	if(groupManagerConnection_)			delete groupManagerConnection_;
	if(multicastManager_)				delete multicastManager_;

	membershipManagerConnection_ = NULL;
	groupManagerConnection_ = NULL;
	multicastManager_ = NULL;
}

bool ClientDynamicsManager::StopAllThreads()
{
	lock();
	runThread_ = false;
	wakeAll();
	unlock();

	if(membershipManagerConnection_)
		membershipManagerConnection_->StopConnection(true);
	if(groupManagerConnection_)
		groupManagerConnection_->StopConnection(true);

	return this->wait();
}

/*
Create a client-based membership-manager implementation 
that automatically detects when clients join and leave.
	This class should include:
		-> All multicast objects (MulticastManager with client/server), etc.
		-> Maintain connection with MembershipManager, GroupManager, etc, 
			and update all objects based on need.
*/

void ClientDynamicsManager::run()
{
	while(membershipManagerConnection_ == NULL || groupManagerConnection_ == NULL || multicastManager_ == NULL)
		waitCondition_.wait(&mutexUpdate_);
	
	IWARNING()  << "RUNNING!" << endl;

	ASSERT(groupManagerConnection_);
	ASSERT(membershipManagerConnection_);
	ASSERT(multicastManager_);

	while(runThread_)
	{
		bool wokenUp = groupManagerConnection_->lockedAndUnlockedWait(1000000);
		
		MutexLocker lock(&mutexUpdate_);
		
		if(runThread_ == false) break;
		if(wokenUp == false) continue;
		
		groupInformation_ = groupManagerConnection_->GetGroupInformation();
	
		IWARNING()  << "GOT GROUP INFORMATION!" << endl;
		multicastManager_->AddOrRemoveMulticastClients(groupInformation_);
	}

	IWARNING()  << "ENDING!" << endl;
}

GroupInformation ClientDynamicsManager::GetGroupInformation()
{
	MutexLocker lock(&mutexUpdate_);
	ASSERT(groupManagerConnection_ != NULL);

	return groupManagerConnection_->GetGroupInformation(); 
}

HostInformation ClientDynamicsManager::GetHostInformation()
{
	MutexLocker lock(&mutexUpdate_);
	ASSERT(groupManagerConnection_ != NULL);

	return groupManagerConnection_->GetHostInformation(); 
}

void ClientDynamicsManager::StartMulticastManager(const InterfaceHandle &multicastServerHandle)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(multicastManager_ == NULL)
		multicastManager_ = new MulticastManager(multicastServerHandle);

	ASSERT(groupManagerConnection_!= NULL);
	
	multicastManager_->AddOrRemoveMulticastClients(groupManagerConnection_->GetGroupInformation());
	
	// TODO: Restart with new handle if already started!
	wakeAll();
}

void ClientDynamicsManager::RegisterInterface(int interfaceId, RpcServer *rpcServer)
{
	MutexLocker lock(&mutexUpdate_);
	ASSERT(multicastManager_ != NULL);
	ASSERT(groupManagerConnection_!= NULL);
	
	multicastManager_->RegisterInterface(interfaceId, rpcServer);
	//wakeAll();
}

bool ClientDynamicsManager::RegisterInGroup(const HostInformation &info, const InterfaceHandle &handle, const string &groupName)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(groupManagerConnection_ == NULL)
		groupManagerConnection_ = new GroupManagerConnection(info, handle, groupName);

	// TODO: if already registered in a group then
	// 1. Leave current group
	// 2. Join new group

	bool ret = groupManagerConnection_->StartConnection(true);
	
	wakeAll();
	return ret;
}

void ClientDynamicsManager::RegisterAsMember(const HostInformation &info, const InterfaceHandle &handle)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(membershipManagerConnection_ == NULL)
		membershipManagerConnection_ = new MembershipManagerConnection(info, handle);
	
	if(membershipManagerConnection_->IsLoggedIn() == false)
		membershipManagerConnection_->StartConnection(true);
	
	wakeAll();
}


