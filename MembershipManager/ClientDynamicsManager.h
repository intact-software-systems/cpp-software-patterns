#ifndef MembershipManager_ClientDynamicsManager_h_IsIncluded
#define MembershipManager_ClientDynamicsManager_h_IsIncluded

#include"MembershipManager/IncludeExtLibs.h"

#include"MembershipManager/MembershipManagerConnection.h"
#include"MembershipManager/GroupManagerConnection.h"

/*
Create a client-based membership-manager implementation 
that automatically detects when clients join and leave.
	This class should include:
		-> All multicast objects (MulticastManager with client/server), etc.
		-> Maintain connection with MembershipManager, GroupManager, etc, 
			and update all objects based on need.
*/

#include "MembershipManager/Export.h"

DLL_STATE class ClientDynamicsManager : public Thread
{
public:
	ClientDynamicsManager();
	~ClientDynamicsManager();

private:
	virtual void run();

public:
	GroupInformation	GetGroupInformation();
	HostInformation		GetHostInformation();

	void StartMulticastManager(const InterfaceHandle &multicastServerHandle);
	
	void RegisterInterface(int interfaceId, RpcServer *rpcServer);
	void RegisterAsMember(const HostInformation &info, const InterfaceHandle &handle);
	bool RegisterInGroup(const HostInformation &info,const InterfaceHandle &handle, const string &groupName);

	inline MulticastManager*				GetMulticastManager()				const { return multicastManager_; }
	inline GroupManagerConnection*			GetGroupManagerConnection()			const { return groupManagerConnection_; }
	inline MembershipManagerConnection*		GetMembershipManagerConnection()	const { return membershipManagerConnection_; }

private:
	bool StopAllThreads();
	
	inline void lock()			{ mutexUpdate_.lock(); }
	inline void unlock()		{ mutexUpdate_.unlock(); }
	inline void wakeAll()		{ waitCondition_.wakeAll(); }

private:
	MulticastManager			*multicastManager_;
	GroupManagerConnection 		*groupManagerConnection_;
	MembershipManagerConnection	*membershipManagerConnection_;
	
	WaitCondition				waitCondition_;
	Mutex						mutexUpdate_;
	bool						runThread_;
	GroupInformation			groupInformation_;
};

#endif


