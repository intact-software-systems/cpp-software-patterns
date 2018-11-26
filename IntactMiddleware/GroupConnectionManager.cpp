#include"IntactMiddleware/GroupConnectionManager.h"
#include"IntactMiddleware/ServiceConnectionManager.h"
#include"IntactMiddleware/IntactSettings/IntactSettings.h"

namespace IntactMiddleware
{

GroupConnectionManager* GroupConnectionManager::groupConnectionManager_ = NULL;

/*--------------------------------------------------------------------------
TODO: Issues:
	- What if the network communication for a group (stub,publisher) goes down, how to detect it locally?
	- Create a SystemConnectionManager that controls the links between groups and network links?
	- Add RegisterGroup in each STUB constructor
	- Add RemoveGroup in each STUB destructor
	- Similarly for Proxies, etc
----------------------------------------------------------------------------*/
GroupConnectionManager::GroupConnectionManager(HostInformation hostInfo, InterfaceHandle handle, bool autoStart)
	: groupManagerHandle_(handle)
	, groupManagerProxy_(NULL)
	, runThread_(true)
	, hostInformation_(hostInfo)
{
	if(autoStart) start();
}

GroupConnectionManager::~GroupConnectionManager()
{
	if(groupManagerProxy_) delete groupManagerProxy_;

	groupManagerProxy_ = NULL;
}

GroupConnectionManager* GroupConnectionManager::GetOrCreate(HostInformation *hostInfo)
{
	static Mutex staticLock;
	MutexLocker lock(&staticLock);

	if(GroupConnectionManager::groupConnectionManager_ != NULL) return GroupConnectionManager::groupConnectionManager_;

	HostInformation myHostInfo = Runtime::GetHostInformation();
	myHostInfo.SetHostDescription(HostInformation::PROXY);
	myHostInfo.SetComponentName(myHostInfo.GetComponentName() + ".connectTo." + IntactSettings::GetGroupManagerService());

	HostInformation serviceInfo;
	ServiceConnectionManager *serviceManager = ServiceConnectionManager::GetOrCreate();
	bool ret = serviceManager->SubscribeService(myHostInfo, IntactSettings::GetGroupManagerService(), serviceInfo);
	ASSERT(ret);

	if(hostInfo == NULL)
	{
		HostInformation hostInfo = Runtime::GetHostInformation();
		ASSERT(hostInfo.GetComponentName().empty() == false);
		GroupConnectionManager::groupConnectionManager_ = new GroupConnectionManager(hostInfo, InterfaceHandle(serviceInfo.GetPortNumber(), serviceInfo.GetHostName()));
	}
	else
	{
		ASSERT(hostInfo->GetComponentName().empty() == false);
		GroupConnectionManager::groupConnectionManager_ = new GroupConnectionManager(*hostInfo, InterfaceHandle(serviceInfo.GetPortNumber(), serviceInfo.GetHostName()));
	}
	return GroupConnectionManager::groupConnectionManager_;
}

bool GroupConnectionManager::WaitConnected()
{
	mutex_.lock();
	while(groupManagerProxy_ == NULL)
	{
		IDEBUG() << "Waiting for groupManagerProxy to connect to GroupManager on " << groupManagerHandle_ ;
		waitCondition_.wait(&mutex_, 10000);
	}
	mutex_.unlock();

	IDEBUG() << " waiting for group manager proxy to connect! " ;
	ASSERT(groupManagerProxy_);
	return groupManagerProxy_->WaitConnected();
}

/*---------------------------------------------------------------------------
	main thread function
---------------------------------------------------------------------------*/
void GroupConnectionManager::run()
{
	IDEBUG() << "Connecting GroupManager on host " << groupManagerHandle_.GetHostName() << " ..." ;
	
	mutex_.lock();
	
	initConnectionManager(groupManagerHandle_);	
	
	while(runThread_)
	{
		try
		{
			bool ret = waitCondition_.wait(&mutex_, 1000);
			if(runThread_ == false) break;
	
			ASSERT(groupManagerProxy_);
			
			// 1. Register or remove groups through GroupManager::GroupRegistration(MapGroups);
			if(setGroupsToJoin_.empty() == false)
			{
				MapGroupNameGroupInformation mapJoinedGroups = groupManagerProxy_->JoinGroups(hostInformation_, setGroupsToJoin_);

				for(SetGroupHandle::iterator it = setGroupsToJoin_.begin(), it_end = setGroupsToJoin_.end(); it != it_end; ++it)
				{
					GroupHandle groupHandle = *it; //->first;
					if(mapJoinedGroups.count(groupHandle.GetGroupName()) <= 0)
					{
						IDEBUG() << "ERROR! Could not join group " << groupHandle ;
						continue;
					}
					GroupInformation &groupInfo = mapJoinedGroups[groupHandle.GetGroupName()];
					
					// -- debug start --
					ASSERT(groupInfo.GetGroupName().empty() == false);
					ASSERT(groupHandle.GetGroupName() == groupInfo.GetGroupName());
					//if(groupConnections_.GetJoinedGroups().count(groupHandle) > 0)
					//	IDEBUG() << "WARNING! Already registered to group " << groupHandle ;
					if(setJoinedGroups_.count(groupHandle) > 0)
						IDEBUG() << "WARNING! Already registered to group " << groupHandle ;
					// -- debug end --

					setJoinedGroups_.insert(groupHandle);
					groupConnections_.GetJoinedGroups()[groupHandle.GetGroupName()] = groupInfo;
				}
				
				setGroupsToJoin_.clear();
			}
			
			if(setGroupsToLeave_.empty() == false)
			{
				int ret = groupManagerProxy_->LeaveGroups(hostInformation_, setGroupsToLeave_);

				for(SetGroupHandle::iterator it = setGroupsToLeave_.begin(), it_end = setGroupsToLeave_.end(); it != it_end; ++it)
				{
					GroupHandle groupHandle = *it;
					
					// -- debug start --
					//if(groupConnections_.GetJoinedGroups().count(groupHandle) <= 0)
					//	IDEBUG() << "WARNING! Already left group " << groupHandle ;
					if(setJoinedGroups_.count(groupHandle) <= 0)
						IDEBUG() << "WARNING! Already left group " << groupHandle ;
					// -- debug end --
					
					setJoinedGroups_.erase(groupHandle);
					groupConnections_.GetJoinedGroups().erase(groupHandle.GetGroupName()); 
				}
				setGroupsToLeave_.clear();
			}
			else if(setJoinedGroups_.empty() == false || setMonitoredGroups_.empty() == false) // do not have to send keep alive every loop-around!!
			{
				// 2. Send Keep Alive messages to inform GroupManager the I am ONLINE, and,
				//    Subscribe to group information in the GroupManager
				//IDEBUG() << Runtime::GetHostInformation().GetComponentName() ;
				groupConnections_ = groupManagerProxy_->AliveChecker(hostInformation_, setJoinedGroups_, setMonitoredGroups_); 
			}

			waitCondition_.wakeAll();
		}
		catch(Exception io)
		{
			IDEBUG() << ": GroupManager connect exception: " << io.what() ;
			waitCondition_.wait(&mutex_, 1000);
		}
		catch(...)
		{
			waitCondition_.wait(&mutex_, 1000);
		}
	}
	mutex_.unlock();

	iFatal("ServiceConnectionManager::run(): Thread stopped!");
}

void GroupConnectionManager::LeaveGroup(GroupHandle groupHandle)
{
	MutexLocker lock(&mutex_);

	if(setGroupsToLeave_.count(groupHandle) > 0)
		IDEBUG() << "WARNING! Already leaving group " << groupHandle ;

	setGroupsToLeave_.insert(groupHandle);
}


bool GroupConnectionManager::JoinGroup(GroupHandle groupHandle)
{
	GroupInformation groupInfo;
	return JoinGroup(groupHandle, groupInfo, false);
}

bool GroupConnectionManager::JoinGroup(GroupHandle groupHandle, GroupInformation &groupInfo, bool wait)
{
	mutex_.lock();

	MapGroupNameGroupInformation &mapJoinedGroups = groupConnections_.GetJoinedGroups();

	if(mapJoinedGroups.count(groupHandle.GetGroupName()) > 0)
	{
		groupInfo = mapJoinedGroups[groupHandle.GetGroupName()];
		mutex_.unlock();
		return true;
	}

	if(setGroupsToJoin_.count(groupHandle) > 0)
		IDEBUG() << "WARNING! Already joining group " << groupHandle ;
	else
		setGroupsToJoin_.insert(groupHandle);
	
	if(wait)
	{
		waitCondition_.wakeAll();
		
		GroupInformation groupInfo;
		while(groupInfo.IsMember(hostInformation_.GetComponentName()) == false)
		{
			if(mapJoinedGroups.count(groupHandle.GetGroupName()) > 0)
				groupInfo = mapJoinedGroups[groupHandle.GetGroupName()];

			if(groupInfo.IsMember(hostInformation_.GetComponentName()) == false)
			{
				cout << COMPONENT_FUNCTION << " Waiting for group " << groupHandle ;
				waitCondition_.wait(&mutex_, 5000);
			}
		}
	}
	
	if(mapJoinedGroups.count(groupHandle.GetGroupName()) > 0)
	{
		groupInfo = mapJoinedGroups[groupHandle.GetGroupName()];
		mutex_.unlock();
		return true;
	}

	mutex_.unlock();
	return false;
}

void GroupConnectionManager::MonitorGroup(string groupName)
{
	MutexLocker lock(&mutex_); 
	
	if(setMonitoredGroups_.count(groupName) > 0)
	{
		IDEBUG() << "WARNING! Already monitoring group " << groupName ;
		return;
	}

	IDEBUG() << "Monitoring group " << groupName ;
	setMonitoredGroups_.insert(groupName);
	waitCondition_.wakeAll();
}

void GroupConnectionManager::initConnectionManager(InterfaceHandle groupManagerHandle)
{
	if(groupManagerProxy_ == NULL) 
	{
		groupManagerProxy_ = new GroupManagerClient(groupManagerHandle);
		waitCondition_.wakeAll();
	}
}

std::map<string, GroupInformation> GroupConnectionManager::GetJoinedGroups() const 
{ 
	MutexLocker lock(&mutex_); 
	return groupConnections_.GetJoinedGroups();
}

std::map<string, GroupInformation> GroupConnectionManager::GetMonitoredGroups() const
{
	MutexLocker lock(&mutex_); 
	return groupConnections_.GetMonitoredGroups();
}

GroupConnections GroupConnectionManager::GetGroupConnections() const
{
	MutexLocker lock(&mutex_);
	return groupConnections_;
}

std::set<GroupHandle> GroupConnectionManager::GetGroupHandleSet() const 
{ 
	MutexLocker lock(&mutex_); 
	return setJoinedGroups_; 
}

GroupInformation GroupConnectionManager::GetGroupConnection(const string &groupName)
{
	MutexLocker lock(&mutex_);
	
	MapGroupNameGroupInformation &mapJoinedGroups = groupConnections_.GetJoinedGroups();
	if(mapJoinedGroups.count(groupName) > 0)
		return mapJoinedGroups[groupName];
	
	MapGroupNameGroupInformation &mapMonitoredGroups = groupConnections_.GetMonitoredGroups();
	if(mapMonitoredGroups.count(groupName) > 0)
		return mapMonitoredGroups[groupName];
	
	IDEBUG() << "WARNING! Group not found " << groupName ;
	return GroupInformation();
}

} // namespace IntactMiddleware


