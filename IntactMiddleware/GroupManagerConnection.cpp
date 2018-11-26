#include"IntactMiddleware/GroupManagerConnection.h"
#include"IntactMiddleware/ServiceConnectionManager.h"
#include"IntactMiddleware/IntactSettings/IntactSettings.h"

namespace IntactMiddleware
{
// -------------------------------------------------------
//	static creation of GroupManagerConnection
// -------------------------------------------------------
/*GroupManagerConnection* GroupManagerConnection::groupManagerConnection_ = NULL;

GroupManagerConnection* GroupManagerConnection::GetOrCreate(InterfaceHandle *groupHandle)
{
	static Mutex staticLock;
	MutexLocker lock(&staticLock);

	if(GroupManagerConnection::groupManagerConnection_ == NULL)
	{
		if(groupHandle == NULL)
		{
			HostInformation myHostInfo = Runtime::GetHostInformation();
			myHostInfo.SetHostDescription(HostInformation::PROXY);
			myHostInfo.SetComponentName(myHostInfo.GetComponentName() + ".connectTo." + IntactSettings::GetGroupManagerService());

			HostInformation serviceInfo;
			ServiceConnectionManager *serviceManager = ServiceConnectionManager::GetOrCreate();
			bool ret = serviceManager->SubscribeServiceInformation(myHostInfo, IntactSettings::GetGroupManagerService(), serviceInfo);
			ASSERT(ret);

			HostInformation hostInfo = Runtime::GetHostInformation();
			GroupManagerConnection::groupManagerConnection_
				= new GroupManagerConnection(hostInfo, InterfaceHandle(serviceInfo.GetPortNumber(), serviceInfo.GetHostIP()));
		}
		else // groupHandle given as input
		{
			HostInformation hostInfo = Runtime::GetHostInformation();
			GroupManagerConnection::groupManagerConnection_= new GroupManagerConnection(hostInfo, *groupHandle);
		}
	}

	ASSERT(GroupManagerConnection::groupManagerConnection_);
	return GroupManagerConnection::groupManagerConnection_;
}*/

// -------------------------------------------------------
//	GroupManager connection
// -------------------------------------------------------
GroupManagerConnection::GroupManagerConnection(const HostInformation &info, const InterfaceHandle &groupManagerHandle, const GroupHandle &groupHandle, bool autoStart) 
	: groupManagerClient_(NULL)
	, myHostInformation_(info)
	, groupManagerHandle_(groupManagerHandle)
	, groupHandle_(groupHandle)
	, isInGroup_(false)
	, automaticJoin_(autoStart)
	, runThread_(true)
{ 
	groupManagerHandle_.SetHostName(NetworkFunctions::getIPAddress(groupManagerHandle_.GetHostName()));
	myHostInformation_.SetHostIP(NetworkFunctions::getIPAddress(myHostInformation_.GetHostName()));
	myHostInformation_.SetOnlineStatus(HostInformation::ONLINE);
	
	if(autoStart) StartConnection(false);
}

GroupManagerConnection::~GroupManagerConnection(void)
{ 
	LeaveGroup();
}

GroupInformation GroupManagerConnection::WaitForGroupChange(GroupInformation &currGroupInfo)
{
	mutex_.lock();
	
	while(currGroupInfo.CompareTo(groupInformation_) == true)
		waitCondition__.wait(&mutex_);
	
	GroupInformation newGroupInfo = groupInformation_;
	mutex_.unlock();
	return newGroupInfo;
}

//-----------------------------------------------------------------------------------------
//			main thread sends alive messages to GroupManager
//-----------------------------------------------------------------------------------------
void GroupManagerConnection::run()
{
	ASSERT(runThread() == true);

	IDEBUG() << "Running!" ;
	
	while(runThread())
	{
		try
		{
			if(automaticLogin() == true)
				JoinGroup();

			while(runThread())
			{
				bool wokenUp = lockedAndUnlockedWait(1000);
				if(IsInGroup() == false || runThread() == false) continue;
				
				GroupInformation currGroupInformation = groupManagerClient_->AliveChecker(myHostInformation_, groupHandle_);
			
				mutex_.lock();
					currGroupInformation.RemoveHost(myHostInformation_.GetComponentName());
					//currGroupInformation.Print();
					//groupInformation_.Print();
					bool equalInfo = currGroupInformation.CompareTo(groupInformation_);

					groupInformation_ = currGroupInformation;
					if(equalInfo == false)
					{
						IDEBUG() << "Group changed!" ;
						waitCondition__.wakeAll();
					}
					///else
					//	currGroupInformation.Print();
				mutex_.unlock();
			}
		}
		catch(Exception io)
		{
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << ". Attempting Reconnect." ;
			groupManagerClient_->WaitConnected();
		}
		catch(...)
		{
			groupManagerClient_->WaitConnected();
		}
	}
}

//-----------------------------------------------------------------------------------------
//		private functions -- not thread safe -> mutex from outside
//-----------------------------------------------------------------------------------------
void GroupManagerConnection::leaveGroup()
{
	if(isInGroup_ == false) 
	{
		IDEBUG() << "Already logged out from " << groupManagerHandle_ ;
		waitCondition__.wakeAll();
		return;
	}

	ASSERT(groupManagerClient_);

	if(isInGroup_ == true)
	{
		try
		{
			groupManagerClient_->LeaveGroup(myHostInformation_, groupHandle_);
			isInGroup_ = false;
		}
		catch(Exception io)
		{
			IDEBUG() << "Exception Caught! GroupManager leaveGroup failed: " << io.msg() << " Setting local status to OFFLINE anyway!" ;
			isInGroup_ = false;
		}
		catch(...)
		{
			IDEBUG() << "Exception caught! GroupManager leaveGrouop failed: Setting local status to OFFLINE anyway!" ;
			isInGroup_ = false;
		}
	}
	waitCondition__.wakeAll();
}

bool GroupManagerConnection::joinGroup()
{
	if(isInGroup_ == true) 
	{
		ASSERT(myHostInformation_.GetOnlineStatus() == HostInformation::ONLINE);
		IDEBUG() << "Already joined group " << groupHandle_<< " in to " << groupManagerHandle_ ;
		return true;
	}

	if(groupManagerClient_ == NULL)
		connectToGroupManager(true);

	int numAttempts = 0;
	while(runThread_)
	{
		try
		{
			if(!groupManagerClient_->WaitConnected())
			{
				IWARNING()  << "Sleeping!" ;
				Thread::sleep(1);
				continue;
			}
			
			//IDEBUG() << " " << myHostInformation_.GetComponentName() << " joining group " << groupHandle_ ;
			groupInformation_ = groupManagerClient_->JoinGroup(myHostInformation_, groupHandle_);
			groupInformation_.RemoveHost(myHostInformation_.GetComponentName());
			
			if(groupInformation_.GetGroupName() == groupHandle_.GetGroupName())
			{
				IDEBUG() << " " << myHostInformation_.GetComponentName() << " joined group " << groupHandle_ ;
				isInGroup_ = true; 
				waitCondition__.wakeAll();
				return isInGroup_;
			}
			else
			{
				isInGroup_ = false;
				IDEBUG() << " " << myHostInformation_.GetComponentName() << "ERROR! Could not join " << groupHandle_ ;
				Thread::sleep(1);
			}
		}
		catch(Exception io)
		{
			isInGroup_ = false;
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << "Attempting again (" << numAttempts++ << ")" ;
			Thread::sleep(1);
		}
		catch(...)
		{
			isInGroup_ = false;
			//cout << myHostInformation_.GetComponentName() << ": Could not connect to GroupManager. Attempting again (" << numAttempts << ")" ;
			cout << "." ;
			Thread::sleep(1);
		}
	}

	return isInGroup_;
}

void GroupManagerConnection::connectToGroupManager(bool waitForConnection)
{
	if(groupManagerClient_ == NULL)
	{
		groupManagerClient_ = new GroupManagerClient(groupManagerHandle_, true);
		groupManagerClient_->StartClient();
		//IDEBUG() << "Connecting GroupManager on host " << groupManagerHandle_ ;
	}
	else
	{
		IDEBUG() << "Connection to GroupManager is already being established!" ;
	}

	int numAttempts = 0;
	while(waitForConnection && groupManagerClient_->IsConnected() == false)
	{
		try
		{
			groupManagerClient_->WaitConnected();
			
			// -- debug start --
			// fail safe
			if(groupManagerClient_->IsConnected() == false)
			{
				throw Exception(" Could not connect ");
			}
			//else
			//{
			//	IDEBUG() << " Connected to GroupManager on host " << groupManagerHandle_ ;
			//}
			// -- debug end --
		}
		catch(Exception io)
		{
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << " Attempting again (" << numAttempts++ << ")" ;
			Thread::sleep(1);
		}
		catch(...)
		{
			//cout << myHostInformation_.GetComponentName() << ": Could not connect to GroupManager. Attempting again (" << numAttempts << ")" ;
			cout << "." ;
			Thread::sleep(1);
		}
	}

	IDEBUG() << " Connected to GroupManager on host " << groupManagerHandle_ ;
}

//-----------------------------------------------------------------------------------------
//		public functions -- thread safe -> mutexed
//-----------------------------------------------------------------------------------------
bool GroupManagerConnection::JoinGroup() 
{
	MutexLocker lock(&mutex_);
	return joinGroup();
}

void GroupManagerConnection::LeaveGroup()
{
	MutexLocker lock(&mutex_);
	leaveGroup();
}

bool GroupManagerConnection::IsInGroup()
{
	MutexLocker lock(&mutex_);
	return isInGroup_;
}

bool GroupManagerConnection::StopConnection(bool waitForStop)
{
	mutex_.lock();
	runThread_ = false;
	waitCondition__.wakeAll();
	mutex_.unlock();

	if(waitForStop == true) Thread::wait();

	return true;
}

bool GroupManagerConnection::StartConnection(bool waitForConnection)
{	
	mutex_.lock();

	runThread_ = true;
	if(isInGroup_ == false) 
		automaticJoin_ = true;
	
	if(Thread::isRunning() == false)
	{
		Thread::start();
		waitCondition__.wakeAll();
	}

	if(waitForConnection)
	{
		while(!isInGroup_ && runThread_)
		{
			bool wokenUp = waitCondition__.wait(&mutex_, 5000);
			if(isInGroup_ == true) break;
			
			// -- debug --
			if(wokenUp)
				IWARNING()  << "WARNING! Woken up but still waiting for connection to the GroupManager " << groupManagerHandle_ ;
			else
				IWARNING()  << " Still waiting for connection to the GroupManager " << groupManagerHandle_ ;
			// -- debug end --
		}
	}

	bool inGroup = isInGroup_;
	mutex_.unlock();

	return inGroup;
}


map<string, GroupInformation> GroupManagerConnection::GetAllGroupInformation()
{
	MutexLocker lock(&mutex_);

	map<string, GroupInformation> mapGroupInformation;
	try
	{
		if(groupManagerClient_ == NULL)
			connectToGroupManager(true);

		mapGroupInformation = groupManagerClient_->GetAllGroupInformation();
	}
	catch(Exception io)
	{
		IWARNING()  << " GroupManager connect exception: " << io.msg() ;
	}
	catch(...)
	{
		IWARNING()  << "GroupManager connect exception " ;
	}	
	return mapGroupInformation;
}

} // namespace IntactMiddleware

