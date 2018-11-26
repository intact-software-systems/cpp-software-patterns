#ifndef IntactMiddleware_GroupManagerConnection_h_IsIncluded
#define IntactMiddleware_GroupManagerConnection_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/GroupManagerBase.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{
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
class DLL_STATE GroupManagerConnection : protected Thread
{
public:
	GroupManagerConnection(const HostInformation &info, const InterfaceHandle &groupManagerHandle, const GroupHandle &groupHandle, bool autoStart = true);
	virtual ~GroupManagerConnection();

//public:
//	static GroupManagerConnection *GetOrCreate(InterfaceHandle *groupHandle = NULL);
public:
	GroupInformation WaitForGroupChange(GroupInformation &currGroupInfo);

	map<string, GroupInformation> GetAllGroupInformation();
	
	bool JoinGroup();
	void LeaveGroup();
	
	bool IsInGroup();
	bool StopConnection(bool wait = true);
	bool StartConnection(bool wait = false);

public:
	inline void WaitForTermination() 	{ Thread::wait(); }
	inline GroupInformation GetGroupInformation()		const { MutexLocker lock(&mutex_); return groupInformation_; }
	inline HostInformation GetHostInformation()			const { MutexLocker lock(&mutex_); return myHostInformation_; }

	inline bool lockedAndUnlockedWait(int64 ms)
	{
		mutex_.lock();
		bool wokenUp = waitCondition__.wait(&mutex_, ms);
		mutex_.unlock();
		return wokenUp;
	}

private:
	virtual void run();
	
	bool joinGroup();
	void leaveGroup();
	void connectToGroupManager(bool waitForConnection = true);

private:
	inline bool runThread()			const { MutexLocker lock(&mutex_); return runThread_; }
	inline bool automaticLogin()	const { MutexLocker lock(&mutex_); return automaticJoin_; }

private:
	GroupManagerClient 		*groupManagerClient_;
	HostInformation			myHostInformation_;
	InterfaceHandle			groupManagerHandle_;
	
	GroupHandle				groupHandle_;
	GroupInformation		groupInformation_;

private:
	bool					isInGroup_;
	bool 					automaticJoin_;
	bool 					runThread_;

private:
	mutable Mutex			mutex_;
	WaitCondition			waitCondition__;

//private:
//	static GroupManagerConnection *groupManagerConnection_;
};

} // namespace IntactMiddleware

#endif


