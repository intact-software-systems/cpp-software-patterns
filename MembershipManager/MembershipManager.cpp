#include"MembershipManager/MembershipManager.h"

#define LATE_CHECKIN_DEADLINE_SECONDS	3

// --------------------------------------------------------
//			class MembershipManager
// implements IntactMiddleware/MembershipManagerInterface.h 
// 			-> Which are RMIs that are called from clients
//
// MembershipManager is centralized
// 1. Hosts log in
// 2. Host log out or crash
// 3. Hosts report "I am alive"
// --------------------------------------------------------

MembershipManager::MembershipManager(bool autoRun) : runThread_(true)
{
	if(autoRun) start();
}

MembershipManager::~MembershipManager()
{
	// TODO
}

// --------------------------------------------------------
//			main thread function
// --------------------------------------------------------
void MembershipManager::run()
{
	ASSERT(runThread_ == true);

	IWARNING()  << " running! ";
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
					IWARNING()  << " " << it->first << " is assumed OFFLINE (missed several alive messages)";
					IWARNING()  << " " << it->second << " <= " << time(NULL) - LATE_CHECKIN_DEADLINE_SECONDS;
					mapHostInformation_[it->first].SetOnlineStatus(HostInformation::OFFLINE);
				}
			}

		}
	}
	catch(Exception ex)
	{
		IWARNING()  << "Exception caught " << ex.msg();
		unlock();
	}
}

// --------------------------------------------------------
//			class MembershipManager
// --------------------------------------------------------
int MembershipManager::Login(HostInformation &info)
{
	MutexLocker lock(&mutexUpdate_);

	if(mapHostInformation_.count(info.GetComponentName()))
	{
		IDEBUG() << "WARNING! " << info.GetComponentName() << " is already logged in!" << endl;
	}

	IDEBUG() << " " << info.GetComponentName() << " is logged in!" << endl;
	
	info.SetOnlineStatus(HostInformation::ONLINE);
	mapHostInformation_[info.GetComponentName()] = info;
	
	createOrUpdateTimeStamp(info.GetComponentName());

	waitForChanges_.wakeAll();

	return 1;
}

int MembershipManager::Logout(string componentName)
{
	MutexLocker lock(&mutexUpdate_);
	
	if(mapHostInformation_.count(componentName) <= 0)
	{
		IDEBUG() << "WARNING! " << componentName << " is already logged out!";
	}
	else
	{
		mapHostInformation_.erase(componentName);
		waitForChanges_.wakeAll();
	}

	return 1;
}

int MembershipManager::AliveChecker(string componentName)
{
	MutexLocker lock(&mutexUpdate_);
	
	ASSERT(componentName.empty() == false);
	
	createOrUpdateTimeStamp(componentName);

	return 1;
}

map<string, HostInformation> MembershipManager::GetMembershipMap()
{
	MutexLocker lock(&mutexUpdate_);
	return mapHostInformation_;
}

//-------------------------------------------------------------------
//			private functions
//-------------------------------------------------------------------
void MembershipManager::createOrUpdateTimeStamp(string componentName)
{
	if(mapAliveCheck_.count(componentName) <= 0)
		mapAliveCheck_.insert(pair<string, int64>(componentName, time(NULL)));
	else
		mapAliveCheck_[componentName] = time(NULL);
}


