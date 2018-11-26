#ifndef IntactMiddleware_GroupConnectionManager_h_IsIncluded
#define IntactMiddleware_GroupConnectionManager_h_IsIncluded

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/GroupManagerBase.h"

#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{
/* -------------------------------------------------------
	GroupManager connections for stub and publisher
	Include:
		- Register Group as a group
		- Register GroupParticipant as a participant of a Group

	TODO:
		- Include ComponentBase when joining/monitoring groups
		- This way, the GroupConnectionManager can use callbacks whenever groups change
			- ComponentBase::GroupChanged(GroupInformation);
		-
 -------------------------------------------------------*/
class DLL_STATE GroupConnectionManager : public Thread
{
public:
	typedef std::map<string, GroupInformation>		MapGroupNameGroupInformation;
	typedef std::set<GroupHandle>					SetGroupHandle;
	typedef std::set<string>						SetGroupName;

private:
	GroupConnectionManager(HostInformation hostInfo, InterfaceHandle groupManagerHandle, bool autoStart = true);
	virtual ~GroupConnectionManager(void);

private:
	virtual void run();

public:
	static GroupConnectionManager* GetOrCreate(HostInformation *hostInfo = NULL);

public:
	bool WaitConnected();
	
	bool JoinGroup(GroupHandle groupHandle);
	bool JoinGroup(GroupHandle groupHandle, GroupInformation &groupInfo, bool wait = true);
	void LeaveGroup(GroupHandle groupHandle);
	void MonitorGroup(string groupName);

	MapGroupNameGroupInformation 	GetJoinedGroups() const;
	MapGroupNameGroupInformation 	GetMonitoredGroups() const;
	GroupConnections				GetGroupConnections() const;

	SetGroupHandle					GetGroupHandleSet() const;
	GroupInformation				GetGroupConnection(const string &groupName);

public:
	inline void Shutdown()					{ MutexLocker lock(&mutex_); runThread_ = false; }

protected:
	void initConnectionManager(InterfaceHandle groupManagerHandle);

private:
	mutable Mutex			mutex_;
	WaitCondition			waitCondition_;

	InterfaceHandle			groupManagerHandle_;
	GroupManagerClient		*groupManagerProxy_;

	bool					runThread_;

	HostInformation			hostInformation_;

private:
	SetGroupHandle					setGroupsToJoin_;
	SetGroupHandle					setGroupsToLeave_;

	SetGroupHandle					setJoinedGroups_;
	SetGroupName					setMonitoredGroups_;

	GroupConnections				groupConnections_;

private:
	static GroupConnectionManager *groupConnectionManager_;
};

}

#endif // namespace IntactMiddleware


