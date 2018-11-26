#include"MembershipManager/GroupManager.h"

#define LATE_CHECKIN_DEADLINE_SECONDS	2

// --------------------------------------------------------
//			class GroupManager
// implements MicroMiddleware/GroupManagerInterface.h 
// 			-> Which are RMIs that are called from clients
//
// GroupManager is centralized
//
// 1. Update group view
// 2. Update group overlay
// 3. Respond to client joining/leaving
// 4. Respond to clients that have changed neighbors.
// --------------------------------------------------------

GroupManager::GroupManager(bool autoStart) : runThread_(true)
{
	if(autoStart) start();
}

GroupManager::~GroupManager()
{
	// TODO
}

// --------------------------------------------------------
//		main thread function	
// --------------------------------------------------------
void GroupManager::run()
{
	IWARNING()  << " running! " << endl;

	try
	{
		lock();
		
		while(runThread_)
		{
			waitForChanges_.wait(&mutexUpdate_, 1000);

			if(runThread_ == false) 
			{
				unlock();
				break;
			}

			for(MapAliveCheck::iterator it = mapAliveCheck_.begin(), it_end = mapAliveCheck_.end(); it != it_end; ++it)
			{
				if(it->second > time(NULL) - LATE_CHECKIN_DEADLINE_SECONDS) 
				{
					ASSERT(mapHostInformation_.count(it->first));
					mapHostInformation_[it->first].SetOnlineStatus(HostInformation::ONLINE);
				}
				else if(mapHostInformation_[it->first].GetOnlineStatus() == HostInformation::ONLINE)
				{
					IWARNING()  << " " << it->first << " is assumed OFFLINE (missed several alive messages)" << endl;
					IWARNING()  << " " << it->second << " <= " << time(NULL) - LATE_CHECKIN_DEADLINE_SECONDS << endl;
					mapHostInformation_[it->first].SetOnlineStatus(HostInformation::OFFLINE);
				}

				addOrUpdateHostInfo(mapHostInformation_[it->first]);
			}
		}
	}
	catch(Exception ex)
	{
		IWARNING()  << "Exception caught " << ex.msg() << endl;
		unlock();
	}
}

void GroupManager::StopGroupManager(bool waitForTermination)
{
	lock();
		runThread_ = false;
		wakeAll();
	unlock();
	
	if(isRunning() && waitForTermination)
		while(wait() == false)
		{
			IWARNING()  << "Wait() for thread termination timed out!" << endl;
			ASSERT(isRunning());
		}
}

// --------------------------------------------------------
//			class GroupManager
// --------------------------------------------------------
GroupInformation GroupManager::GroupChange(HostInformation hostInfo, string currentGroupName, string newGroupName)
{
	MutexLocker lock(&mutexUpdate_);

	if(addOrUpdateHostInfo(hostInfo) == 0)
	{
		IWARNING()  << "WARNING! " << hostInfo.GetComponentName() << " Changing groups but not registered!" << endl;
	}

	removeHostFromGroup(hostInfo, currentGroupName);
	addHostToGroup(hostInfo, newGroupName);

	createOrUpdateTimeStamp(hostInfo.GetComponentName());

	wakeAll();
	
	return mapGroupInformation_[newGroupName];
}

GroupInformation GroupManager::GetGroupInformation(string groupName)
{
	MutexLocker lock(&mutexUpdate_);

	if(mapGroupInformation_.count(groupName) <= 0)
		return GroupInformation();
	
	return mapGroupInformation_[groupName];
}

GroupInformation GroupManager::AliveChecker(HostInformation hostInfo, string currentGroupName)
{
	MutexLocker lock(&mutexUpdate_);

	if(mapGroupInformation_.count(currentGroupName) <= 0)
	{	
		IWARNING()  << "WARNING! group not found! " << currentGroupName << endl;
		return GroupInformation();
	}

	GroupInformation &groupInfo = mapGroupInformation_[currentGroupName];
	bool ret = groupInfo.UpdateHost(hostInfo);
	ASSERT(ret == true);

	createOrUpdateTimeStamp(hostInfo.GetComponentName());
	wakeAll();

	return groupInfo;
}


int GroupManager::LeaveGroup(HostInformation hostInfo, string groupName)
{
	MutexLocker lock(&mutexUpdate_);

	if(addOrUpdateHostInfo(hostInfo) == 0)
		IWARNING()  << "WARNING! " << hostInfo.GetComponentName() << " removing from group " << groupName << " but not registered!" << endl;
	
	wakeAll();

	return (int)removeHostFromGroup(hostInfo, groupName);
}

GroupInformation GroupManager::JoinGroup(HostInformation hostInfo, string groupName)
{
	MutexLocker lock(&mutexUpdate_);

	IWARNING()  << " Joining group name : " << groupName << endl;

	if(addOrUpdateHostInfo(hostInfo) == 0)
	{
		IWARNING()  << "WARNING! Adding " << hostInfo.GetComponentName() << " to group " << groupName << " but component is not registered!" << endl;
	}	

	addHostToGroup(hostInfo, groupName);
	createOrUpdateTimeStamp(hostInfo.GetComponentName());
	
	wakeAll();

	return mapGroupInformation_[groupName];
}

map<string, GroupInformation> GroupManager::GetAllGroupInformation()
{
	MutexLocker lock(&mutexUpdate_);

	return mapGroupInformation_;
}

// --------------------------------------------------------
//			private functions
// --------------------------------------------------------
void GroupManager::createOrUpdateTimeStamp(string componentName)
{
	if(mapAliveCheck_.count(componentName) <= 0)
		mapAliveCheck_.insert(pair<string, int64>(componentName, time(NULL)));
	else
		mapAliveCheck_[componentName] = time(NULL);
}

bool GroupManager::removeHostFromGroup(const HostInformation &hostInfo, const string &groupName)
{
	if(mapGroupInformation_.count(groupName) <= 0)
	{
		IWARNING()  << "WARNING! " << groupName<< " is not registered!" << endl;
		return false;
	}
	
	GroupInformation &groupInfo = mapGroupInformation_[groupName];
	if(groupInfo.IsMember(hostInfo.GetComponentName()))
	{
		mapComponentNameGroups_[hostInfo.GetComponentName()].erase(groupName);

		groupInfo.RemoveHost(hostInfo.GetComponentName());
		return true;
	}
	else
	{
		IWARNING()  << "WARNING! " << hostInfo.GetComponentName() << " is not a member of " << groupName << endl;
		return false;
	}
	return false;
}

bool GroupManager::addHostToGroup(const HostInformation &hostInfo, const string &groupName)
{
	if(mapGroupInformation_.count(groupName) <= 0)
	{
		IWARNING()  << "WARNING! " << groupName << " is not registered. Adding it!" << endl;
		mapGroupInformation_.insert(pair<string, GroupInformation>(groupName, GroupInformation(groupName)));
	}
	
	GroupInformation &groupInfo = mapGroupInformation_[groupName];
	if(!groupInfo.IsMember(hostInfo.GetComponentName()))
	{
		mapComponentNameGroups_[hostInfo.GetComponentName()].insert(groupName);
		groupInfo.AddHost(hostInfo);
		return true;
	}
	else
	{
		IWARNING()  << "WARNING! " << hostInfo.GetComponentName() << " is already a member of " << groupName << endl;
		return false;
	}
	return false;
}

int GroupManager::addOrUpdateHostInfo(const HostInformation &hostInfo)
{
	if(mapHostInformation_.count(hostInfo.GetComponentName()) <= 0)
	{
		mapHostInformation_.insert(pair<string, HostInformation>(hostInfo.GetComponentName(), hostInfo));
		return 0;
	}
	
	mapHostInformation_[hostInfo.GetComponentName()] = hostInfo;
	
	ASSERT(mapComponentNameGroups_.count(hostInfo.GetComponentName()));
	//IWARNING()  << " " << hostInfo << endl;

	const SetGroup &setGroup = mapComponentNameGroups_[hostInfo.GetComponentName()];
	for(SetGroup::const_iterator it = setGroup.begin(), it_end = setGroup.end(); it != it_end; ++it)
	{
		string groupName = *it;
		ASSERT(mapGroupInformation_.count(groupName));
		mapGroupInformation_[groupName].UpdateHost(hostInfo);
		GroupInformation &groupInfo = mapGroupInformation_[groupName];
		groupInfo.UpdateHost(hostInfo);

		//IWARNING()  << " updating group " << groupName << endl;
		//groupInfo.Print();
	}

	return 1;
}

