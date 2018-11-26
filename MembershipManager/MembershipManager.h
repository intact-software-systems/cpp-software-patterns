#ifndef MembershipManager_MembershipManager_h_IsIncluded
#define MembershipManager_MembershipManager_h_IsIncluded

#include<map>
#include<sstream>
#include<fstream>

#include "MembershipManager/IncludeExtLibs.h"

// --------------------------------------------------------
//			class MembershipManager
// 	- keeps updated list of the members in the system.
// --------------------------------------------------------
class MembershipManager : public MembershipManagerInterface, public Thread
{
private:
	typedef map<string, HostInformation>	MapHostInformation;
	typedef map<string, int64>		MapAliveCheck;

public:
	MembershipManager(bool autorun = false);
	~MembershipManager();

	virtual void run();

	virtual int Login(HostInformation &info);
	virtual int Logout(string componentName);
	virtual int AliveChecker(string componentName);
	virtual MapHostInformation GetMembershipMap();

public:
	inline MapHostInformation	GetAllHostInformation()	const { return mapHostInformation_; }

private:
	inline void lock()			{ mutexUpdate_.lock(); }
	inline void unlock()		{ mutexUpdate_.unlock(); }

private:
	void createOrUpdateTimeStamp(string componentName);

private:
	MapHostInformation			mapHostInformation_;
	MapAliveCheck				mapAliveCheck_;
	WaitCondition				waitForChanges_;
	Mutex						mutexUpdate_;
	bool						runThread_;
};

#endif

