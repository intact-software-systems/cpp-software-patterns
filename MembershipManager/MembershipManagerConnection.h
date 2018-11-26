#ifndef MembershipManager_MembershipManagerConnection_h_IsIncluded
#define MembershipManager_MembershipManagerConnection_h_IsIncluded

#include "MembershipManager/IncludeExtLibs.h"

using namespace std;

// --------------------------------------------------------
//			struct MembershipManagerConnectionConfig
// --------------------------------------------------------
struct MembershipManagerConnectionConfig
{
	static int membershipManagerPort;
};

// --------------------------------------------------------
//			class MembershipManagerConnection
// --------------------------------------------------------
/**
* Class MembershipManagerConnection logs in the component.
*/
class MembershipManagerConnection : protected Thread
{
public:
	MembershipManagerConnection(const HostInformation &info, InterfaceHandle handle, bool autorStart = false);
	virtual ~MembershipManagerConnection();

	bool Login();
	void Logout();
	
	bool IsLoggedIn();
	bool StopConnection(bool wait = true);
	bool StartConnection(bool wait = false);

	void WaitForTermination() 	{ Thread::wait(); }

private:
	virtual void run();
	
	bool login();
	void logout();

private:
	inline bool runThread()			{ MutexLocker lock(&classMutex_); return runThread_; }
	inline bool automaticLogin()	{ MutexLocker lock(&classMutex_); return automaticLogin_; }

	inline void lock()			{ classMutex_.lock(); }
	inline void unlock()		{ classMutex_.unlock(); }
	inline void wakeAll()		{ classCondition_.wakeAll(); }

	inline bool lockedAndUnlockedWait(int64 ms)
	{
		lock();
		bool wokenUp = classCondition_.wait(&classMutex_, ms);
		unlock();
		return wokenUp;
	}

private:
	bool 					runThread_;
	bool					isLoggedIn_;
	bool 					automaticLogin_;

	MembershipManagerClient *membershipManagerProxy_;
	HostInformation			myHostInformation_;
	InterfaceHandle			membershipManagerHandle_;
	
	Mutex					classMutex_;
	WaitCondition			classCondition_;
};

#endif



