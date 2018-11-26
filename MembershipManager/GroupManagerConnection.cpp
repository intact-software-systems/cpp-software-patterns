
#include"MembershipManager/GroupManagerConnection.h"

// -------------------------------------------------------
//	GroupManager connection
// -------------------------------------------------------
GroupManagerConnection::GroupManagerConnection(const HostInformation &info, const InterfaceHandle &groupHandle, const string &groupName, bool autoStart) 
	: groupManagerClient_(NULL)
	, myHostInformation_(info)
	, groupManagerHandle_(groupHandle)
	, groupName_(groupName)
	, isInGroup_(false)
	, automaticJoin_(autoStart)
	, runThread_(autoStart)
{ 
	groupManagerHandle_.SetHostName(HostInformationSpace::GetIPAddress(groupManagerHandle_.GetHostName()));
	myHostInformation_.SetHostIP(HostInformationSpace::GetIPAddress(myHostInformation_.GetHostName()));
	myHostInformation_.SetOnlineStatus(HostInformation::ONLINE);
	
	if(autoStart) StartConnection(false);
}

GroupManagerConnection::~GroupManagerConnection(void)
{ 
	LeaveGroup();
}

GroupInformation GroupManagerConnection::WaitForGroupChange(GroupInformation &currGroupInfo)
{
	lock();
	
	while(currGroupInfo.CompareTo(groupInformation_) == true)
		wait();
	
	GroupInformation newGroupInfo = groupInformation_;
	unlock();
	return newGroupInfo;
}

//-----------------------------------------------------------------------------------------
//			main thread sends alive messages to GroupManager
//-----------------------------------------------------------------------------------------
void GroupManagerConnection::run()
{
	ASSERT(runThread() == true);

	IDEBUG() << "Running!" << endl;
	
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
				
				GroupInformation currGroupInformation = groupManagerClient_->AliveChecker(myHostInformation_, groupName_);
			
				lock();
					currGroupInformation.RemoveHost(myHostInformation_.GetComponentName());
					//currGroupInformation.Print();
					//groupInformation_.Print();
					bool equalInfo = currGroupInformation.CompareTo(groupInformation_);

					groupInformation_ = currGroupInformation;
					if(equalInfo == false)
					{
						IDEBUG() << "Group changed!" << endl;
						wakeAll();
					}
					///else
					//	currGroupInformation.Print();
				unlock();
			}
		}
		catch(Exception io)
		{
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
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
		IDEBUG() << "Already logged out from " << groupManagerHandle_ << endl;
		wakeAll();
		return;
	}

	ASSERT(groupManagerClient_);

	if(isInGroup_ == true)
	{
		try
		{
			groupManagerClient_->LeaveGroup(myHostInformation_, groupName_);
			isInGroup_ = false;
		}
		catch(Exception io)
		{
			IDEBUG() << "Exception Caught! GroupManager leaveGroup failed: " << io.msg() << " Setting local status to OFFLINE anyway!" << endl;
			isInGroup_ = false;
		}
		catch(...)
		{
			IDEBUG() << "Exception caught! GroupManager leaveGrouop failed: Setting local status to OFFLINE anyway!" << endl;
			isInGroup_ = false;
		}
	}
	wakeAll();
}

bool GroupManagerConnection::joinGroup()
{
	if(isInGroup_ == true) 
	{
		ASSERT(myHostInformation_.GetOnlineStatus() == HostInformation::ONLINE);
		IDEBUG() << "Already joined group " << groupName_ << " in to " << groupManagerHandle_ << endl;
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
				IWARNING()  << "Sleeping!" << endl;
				Thread::sleep(1);
				continue;
			}
			
			//IDEBUG() << " " << myHostInformation_.GetComponentName() << " joining group " << groupName_ << endl;
			groupInformation_ = groupManagerClient_->JoinGroup(myHostInformation_, groupName_);
			groupInformation_.RemoveHost(myHostInformation_.GetComponentName());
			
			if(groupInformation_.GetGroupName() == groupName_)
			{
				IDEBUG() << " " << myHostInformation_.GetComponentName() << " joined group " << groupName_ << endl;
				isInGroup_ = true; 
				wakeAll();
				return isInGroup_;
			}
			else
			{
				isInGroup_ = false;
				IDEBUG() << " " << myHostInformation_.GetComponentName() << "ERROR! Could not join " << groupName_ << endl;
				Thread::sleep(1);
			}
		}
		catch(Exception io)
		{
			isInGroup_ = false;
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << "Attempting again (" << numAttempts++ << ")" << endl;
			Thread::sleep(1);
		}
		catch(...)
		{
			isInGroup_ = false;
			//cout << myHostInformation_.GetComponentName() << ": Could not connect to GroupManager. Attempting again (" << numAttempts << ")" << endl;
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
		//IDEBUG() << "Connecting GroupManager on host " << groupManagerHandle_ << endl;
	}
	else
	{
		IDEBUG() << "Connection to GroupManager is already being established!" << endl;
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
			//	IDEBUG() << " Connected to GroupManager on host " << groupManagerHandle_ << endl;
			//}
			// -- debug end --
		}
		catch(Exception io)
		{
			IDEBUG() << ": GroupManager connect exception: " << io.msg() << " Attempting again (" << numAttempts++ << ")" << endl;
			Thread::sleep(1);
		}
		catch(...)
		{
			//cout << myHostInformation_.GetComponentName() << ": Could not connect to GroupManager. Attempting again (" << numAttempts << ")" << endl;
			cout << "." ;
			Thread::sleep(1);
		}
	}

	IDEBUG() << " Connected to GroupManager on host " << groupManagerHandle_ << endl;
}

//-----------------------------------------------------------------------------------------
//		public functions -- thread safe -> mutexed
//-----------------------------------------------------------------------------------------
bool GroupManagerConnection::JoinGroup() 
{
	MutexLocker lock(&classMutex_);
	return joinGroup();
}

void GroupManagerConnection::LeaveGroup()
{
	MutexLocker lock(&classMutex_);
	leaveGroup();
}

bool GroupManagerConnection::IsInGroup()
{
	MutexLocker lock(&classMutex_);
	return isInGroup_;
}

bool GroupManagerConnection::StopConnection(bool waitForStop)
{
	lock();
	runThread_ = false;
	wakeAll();
	unlock();

	if(waitForStop == true) Thread::wait();
}

bool GroupManagerConnection::StartConnection(bool waitForConnection)
{	
	lock();

	runThread_ = true;
	if(isInGroup_ == false) 
		automaticJoin_ = true;
	
	if(Thread::isRunning() == false)
	{
		Thread::start();
		wakeAll();
	}

	if(waitForConnection)
	{
		while(!isInGroup_ && runThread_)
		{
			bool wokenUp = classCondition_.wait(&classMutex_, 5000);
			if(isInGroup_ == true) break;
			
			// -- debug --
			if(wokenUp)
				IWARNING()  << "WARNING! Woken up but still waiting for connection to the GroupManager " << groupManagerHandle_ << endl;
			else
				IWARNING()  << " Still waiting for connection to the GroupManager " << groupManagerHandle_ << endl;
			// -- debug end --
		}
	}

	bool inGroup = isInGroup_;
	unlock();

	return inGroup;
}


map<string, GroupInformation> GroupManagerConnection::GetAllGroupInformation()
{
	MutexLocker lock(&classMutex_);

	map<string, GroupInformation> mapGroupInformation;
	try
	{
		if(groupManagerClient_ == NULL)
			connectToGroupManager(true);

		mapGroupInformation = groupManagerClient_->GetAllGroupInformation();
	}
	catch(Exception io)
	{
		IWARNING()  << " GroupManager connect exception: " << io.msg() << endl;
	}
	catch(...)
	{
		IWARNING()  << "GroupManager connect exception " << endl;
	}	
	return mapGroupInformation;
}


