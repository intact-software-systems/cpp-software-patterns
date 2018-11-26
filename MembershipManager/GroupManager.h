#ifndef MembershipManager_GroupManager_h_IsIncluded
#define MembershipManager_GroupManager_h_IsIncluded

#include<map>
#include<sstream>
#include<fstream>

#include "MembershipManager/IncludeExtLibs.h"

using namespace std;

// --------------------------------------------------------
//			class GroupManager
//	- keeps the groups up-to-date.
// 	- GroupName == ServiceName 
//--------------------------------------------------------
class GroupManager : public GroupManagerInterface, public Thread
{
private:
	typedef set<string>						SetGroup;
	typedef map<string, SetGroup>			MapComponentNameGroups;
	typedef map<string, HostInformation> 	MapHostInformation;
	typedef map<string, GroupInformation>	MapGroupInformation;
	typedef map<string, int64>		MapAliveCheck;

public:
	GroupManager(bool autoStart = false);
	~GroupManager();

	void StopGroupManager(bool waitForTermination = true);

	virtual int					LeaveGroup(HostInformation hostInfo, string groupName);
	virtual GroupInformation	JoinGroup(HostInformation hostInfo, string groupName);
	virtual GroupInformation 	GroupChange(HostInformation hostInfo, string currentGroupName, string newGroupName);
	
	virtual GroupInformation 	GetGroupInformation(string groupName);
	virtual GroupInformation 	AliveChecker(HostInformation hostInfo, string currentGroupName);
	virtual MapGroupInformation	GetAllGroupInformation();

protected:
	virtual void run();

private:
	inline void lock()			{ mutexUpdate_.lock(); }
	inline void unlock()		{ mutexUpdate_.unlock(); }
	inline void wakeAll()		{ waitForChanges_.wakeAll(); }

private:
	bool removeHostFromGroup(const HostInformation &hostInfo, const string &groupName);
	bool addHostToGroup(const HostInformation &hostInfo, const string &groupName);
	int addOrUpdateHostInfo(const HostInformation &hostInfo);
	void createOrUpdateTimeStamp(string componentName);

private:
	MapGroupInformation			mapGroupInformation_;
	MapHostInformation			mapHostInformation_;
	MapAliveCheck				mapAliveCheck_;
	MapComponentNameGroups		mapComponentNameGroups_;

	Mutex						mutexUpdate_;
	WaitCondition				waitForChanges_;
	bool						runThread_;
};

#endif

