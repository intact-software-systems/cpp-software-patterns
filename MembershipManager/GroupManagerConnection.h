#ifndef MembershipManager_GroupManagerConnection_h_IsIncluded
#define MembershipManager_GroupManagerConnection_h_IsIncluded

#include "MembershipManager/IncludeExtLibs.h"

using namespace std;

// --------------------------------------------------------
//			struct GroupManagerConnectionConfig
// --------------------------------------------------------
struct GroupManagerConnectionConfig
{
	static int groupManagerPort;
};

// --------------------------------------------------------
//			class GroupManagerConnection
// --------------------------------------------------------
/**
* Class GroupManagerConnection logs in the component.
*/
class GroupManagerConnection : protected Thread
{
public:
	GroupManagerConnection(const HostInformation &info, const InterfaceHandle &handle, const string &groupName, bool autoStart = false);
	virtual ~GroupManagerConnection();

	GroupInformation WaitForGroupChange(GroupInformation &currGroupInfo);

	map<string, GroupInformation> GetAllGroupInformation();
	
	bool JoinGroup();
	void LeaveGroup();
	
	bool IsInGroup();
	bool StopConnection(bool wait = true);
	bool StartConnection(bool wait = false);

public:
	inline void WaitForTermination() 	{ Thread::wait(); }
	inline GroupInformation GetGroupInformation()		{ MutexLocker lock(&classMutex_); return groupInformation_; }
	inline HostInformation GetHostInformation()			{ MutexLocker lock(&classMutex_); return myHostInformation_; }

	inline void lock()			{ classMutex_.lock(); }
	inline void unlock()		{ classMutex_.unlock(); }
	inline void wakeAll()		{ classCondition_.wakeAll(); }
	inline bool wait()			{ return classCondition_.wait(&classMutex_); }

	inline bool lockedAndUnlockedWait(int64 ms)
	{
		lock();
		bool wokenUp = classCondition_.wait(&classMutex_, ms);
		unlock();
		return wokenUp;
	}


private:
	virtual void run();
	
	bool joinGroup();
	void leaveGroup();
	void connectToGroupManager(bool waitForConnection = true);

private:
	inline bool runThread()			{ MutexLocker lock(&classMutex_); return runThread_; }
	inline bool automaticLogin()	{ MutexLocker lock(&classMutex_); return automaticJoin_; }

private:
	bool 					runThread_;
	bool					isInGroup_;
	bool 					automaticJoin_;

	GroupManagerClient 		*groupManagerClient_;
	HostInformation			myHostInformation_;
	InterfaceHandle			groupManagerHandle_;
	
	string					groupName_;
	GroupInformation		groupInformation_;

	Mutex					classMutex_;
	WaitCondition			classCondition_;
};

#endif





