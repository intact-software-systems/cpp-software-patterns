#include"MembershipManager/MembershipManagerConnection.h"

// -------------------------------------------------------
//	MembershipManager connection
// -------------------------------------------------------
MembershipManagerConnection::MembershipManagerConnection(const HostInformation &info, InterfaceHandle membershipHandle, bool autoStart) 
	: membershipManagerProxy_(NULL)
	, myHostInformation_(info)
	, membershipManagerHandle_(membershipHandle)
	, isLoggedIn_(false)
	, automaticLogin_(autoStart)
	, runThread_(autoStart)
{ 
	membershipManagerHandle_.SetHostName(HostInformationSpace::GetIPAddress(membershipManagerHandle_.GetHostName()));
	myHostInformation_.SetHostIP(HostInformationSpace::GetIPAddress(myHostInformation_.GetHostName()));
	myHostInformation_.SetOnlineStatus(HostInformation::OFFLINE);
	
	if(autoStart) StartConnection(false);
}

MembershipManagerConnection::~MembershipManagerConnection(void)
{ 
	Logout();
}

//-----------------------------------------------------------------------------------------
//			main thread sends alive messages to MembershipManager
//-----------------------------------------------------------------------------------------
void MembershipManagerConnection::run()
{
	IWARNING()  << " Started thread ";

	if(automaticLogin() == true)
	{
		Login();
		wakeAll();
	}

	ASSERT(runThread() == true);

	while(runThread())
	{
		try
		{
			bool wokenUp = lockedAndUnlockedWait(1000);
			if(IsLoggedIn() == false || runThread() == false) continue;
			
			//IWARNING()  << "Sending alive checker " << myHostInformation_.GetComponentName();
			int ret = membershipManagerProxy_->AliveChecker(myHostInformation_.GetComponentName());
		}
		catch(Exception io)
		{
			cout << myHostInformation_.GetComponentName() << ": MembershipManager connect exception: " << io.msg() << ". Attempting Reconnect." << endl;
			Thread::sleep(1);
		}
		catch(...)
		{
			//cout << myHostInformation_.GetComponentName() << ": Lost connection to MembershipManager. Attempting Reconnect." << endl;
			Thread::sleep(1);
		}
	}
}
//-----------------------------------------------------------------------------------------
//		private functions -- not thread safe -> mutex from outside
//-----------------------------------------------------------------------------------------
void MembershipManagerConnection::logout()
{
	if(isLoggedIn_ == false) 
	{
		ASSERT(myHostInformation_.GetOnlineStatus() == HostInformation::OFFLINE);
		IDEBUG() << "Already logged out from " << membershipManagerHandle_;
		wakeAll();
		return;
	}

	myHostInformation_.SetOnlineStatus(HostInformation::OFFLINE);
	
	if(membershipManagerProxy_ == NULL) return;

	if(isLoggedIn_ == true)
	{
		try
		{
			membershipManagerProxy_->Logout(myHostInformation_.GetComponentName());
			isLoggedIn_ = false;
		}
		catch(Exception io)
		{
			cout << myHostInformation_.GetComponentName() << "Exception Caught! MembershipManager logout failed: " << io.msg() << " Setting local status to OFFLINE anyway!" << endl;
			isLoggedIn_ = false;
		}
		catch(...)
		{
			cout << myHostInformation_.GetComponentName() << "Exception caught! MembershipManager logout failed: Setting local status to OFFLINE anyway!" << endl;
			isLoggedIn_ = false;
		}
	}

	wakeAll();
}

bool MembershipManagerConnection::login()
{
	if(isLoggedIn_ == true) 
	{
		ASSERT(myHostInformation_.GetOnlineStatus() == HostInformation::ONLINE);
		IDEBUG() << "Already logged in to " << membershipManagerHandle_ << endl;
		return true;
	}

	if(membershipManagerProxy_ == NULL)
		membershipManagerProxy_ = new MembershipManagerClient(membershipManagerHandle_, true);

	//IDEBUG() << "Connecting MembershipManager on host " << membershipManagerHandle_ << endl;
	int numAttempts = 0;
	while(runThread_)
	{
		try
		{
			if(!membershipManagerProxy_->WaitConnected())
			{
				Thread::sleep(1);
				continue;
			}
			
			myHostInformation_.SetOnlineStatus(HostInformation::ONLINE);
			myHostInformation_.SetTimeStamp(HostInformationSpace::GetTimeStamp());
			int ret = membershipManagerProxy_->Login(myHostInformation_);

			IDEBUG() << " " << myHostInformation_.GetComponentName() << " connected to MembershipManager " << ret;
			return true;
		}
		catch(Exception io)
		{
			cout << myHostInformation_.GetComponentName() << ": MembershipManager connect exception: " << io.msg() << "Attempting again (" << numAttempts++ << ")" << endl;
			Thread::sleep(1);
		}
		catch(...)
		{
			//cout << myHostInformation_.GetComponentName() << ": Could not connect to MembershipManager. Attempting again (" << numAttempts << ")" << endl;
			cout << "." ;
			Thread::sleep(1);
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------------
//		public functions -- thread safe -> mutexed
//-----------------------------------------------------------------------------------------
bool MembershipManagerConnection::Login() 
{
	MutexLocker lock(&classMutex_);
	return (isLoggedIn_ = login());
}

void MembershipManagerConnection::Logout()
{
	MutexLocker lock(&classMutex_);
	logout();
}

bool MembershipManagerConnection::IsLoggedIn()
{
	MutexLocker lock(&classMutex_);
	return isLoggedIn_;
}

bool MembershipManagerConnection::StopConnection(bool waitForStop)
{
	lock();
	runThread_ = false;
	wakeAll();
	unlock();

	if(waitForStop == true) Thread::wait();
}

bool MembershipManagerConnection::StartConnection(bool waitForConnection)
{	
	lock();

	runThread_ = true;
	if(isLoggedIn_ == false) 
		automaticLogin_ = true;
	
	if(Thread::isRunning() == false)
	{
		Thread::start();
		wakeAll();
	}

	if(waitForConnection)
	{
		while(!isLoggedIn_ && runThread_)
		{
			bool wokenUp = classCondition_.wait(&classMutex_, 1000);
			if(isLoggedIn_ == true) break;
			
			// -- debug --
			if(wokenUp)
				IWARNING()  << "WARNING! Woken up but still waiting for connection to the MembershipManager " << membershipManagerHandle_;
			else
				IWARNING()  << " Still waiting for connection to the MembershipManager " << membershipManagerHandle_;
			// -- debug end --
		}
	}

	bool loggedIn = isLoggedIn_;
	unlock();

	return loggedIn;
}

