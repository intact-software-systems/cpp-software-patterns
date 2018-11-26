#ifndef IntactMiddleware_GroupManagerBase_h_IsIncluded
#define IntactMiddleware_GroupManagerBase_h_IsIncluded

#include <string>

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/ServiceConnectionManager.h"
#include"IntactMiddleware/Export.h"
namespace IntactMiddleware
{
class DLL_STATE GroupConnections : public MicroMiddleware::RmiObjectBase
{
public:
    typedef std::map<std::string, GroupInformation>		MapGroupNameGroupInformation;

	GroupConnections()
	{}
	GroupConnections(const MapGroupNameGroupInformation &mapJoined, const MapGroupNameGroupInformation &mapMonitored)
		: mapJoinedGroupInformations_(mapJoined)
		, mapMonitoredGroupInformations_(mapMonitored)
	{}
	~GroupConnections()
	{}
	
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteInt32(mapJoinedGroupInformations_.size());
		for(MapGroupNameGroupInformation::iterator it = mapJoinedGroupInformations_.begin(), it_end = mapJoinedGroupInformations_.end(); it != it_end; ++it)
		{
			writer->WriteString(it->first);

			GroupInformation &info = it->second;
			info.Write(writer);
		}

		writer->WriteInt32(mapMonitoredGroupInformations_.size());
		for(MapGroupNameGroupInformation::iterator it = mapMonitoredGroupInformations_.begin(), it_end = mapMonitoredGroupInformations_.end(); it != it_end; ++it)
		{
			writer->WriteString(it->first);

			GroupInformation &info = it->second;
			info.Write(writer);
		}
	}
	
	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		int size = reader->ReadInt32();
		for(int i = 0; i < size; i++)
		{
            std::string groupName = reader->ReadString();
			GroupInformation info;
			info.Read(reader);
            mapJoinedGroupInformations_.insert(std::pair<std::string, GroupInformation>(groupName, info));
		}
	
		size = reader->ReadInt32();
		for(int i = 0; i < size; i++)
		{
            std::string groupName = reader->ReadString();
			GroupInformation info;
			info.Read(reader);
            mapMonitoredGroupInformations_.insert(std::pair<std::string, GroupInformation>(groupName, info));
		}
	}
	
	virtual char* toRaw()
	{
		return NULL;
	}

    std::map<std::string, GroupInformation>& GetJoinedGroups() 				{ return mapJoinedGroupInformations_; }
    std::map<std::string, GroupInformation>& GetMonitoredGroups()			{ return mapMonitoredGroupInformations_; }


    std::map<std::string, GroupInformation> GetJoinedGroups() 		const 	{ return mapJoinedGroupInformations_; }
    std::map<std::string, GroupInformation> GetMonitoredGroups()	const 	{ return mapMonitoredGroupInformations_; }


	void SetJoinedGroups(const MapGroupNameGroupInformation &mapJoined) 		{ mapJoinedGroupInformations_ = mapJoined; }
	void SetMonitoredGroups(const MapGroupNameGroupInformation &mapMonitored)	{ mapMonitoredGroupInformations_ = mapMonitored; }

private:
	MapGroupNameGroupInformation   	mapJoinedGroupInformations_;
	MapGroupNameGroupInformation   	mapMonitoredGroupInformations_;
};

// -------------------------------------------------------
//				GroupManager Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the GroupManager.
*/

class DLL_STATE GroupManagerInterface : public RmiInterface
{
public:
    typedef std::map<std::string, GroupInformation>		MapGroupNameGroupInformation;
    typedef std::set<GroupHandle>						SetGroupHandle;
    typedef std::set<std::string>						SetNames;

public:
	GroupManagerInterface() { }
	~GroupManagerInterface() { }

	enum MethodID 
	{
		Method_JoinGroup = 11,
		Method_JoinGroups,
		Method_LeaveGroup,
		Method_LeaveGroups,
		Method_GroupChange,
		Method_GroupInformation,
		Method_AliveChecker,
		Method_AliveCheckerMaps,
		Method_AllGroupInformation
	};

	enum InterfaceId
	{
		GroupManagerInterfaceId = 11
	};

	// GroupName === ServiceName 
	virtual GroupInformation				JoinGroup(HostInformation hostInfo, GroupHandle group) = 0;
	virtual MapGroupNameGroupInformation	JoinGroups(HostInformation hostInfo, SetGroupHandle group) = 0;
	
	virtual int								LeaveGroup(HostInformation hostInfo, GroupHandle group) = 0;
	virtual int								LeaveGroups(HostInformation hostInfo, SetGroupHandle group) = 0;

	virtual GroupInformation 				GroupChange(HostInformation hostInfo, GroupHandle currentGroup, GroupHandle newGroup) = 0;
    virtual GroupInformation 				GetGroupInformation(std::string groupName) = 0;
	virtual GroupInformation 				AliveChecker(HostInformation hostInfo, GroupHandle currentGroup) = 0;
	virtual GroupConnections				AliveChecker(HostInformation &hostInfo, SetGroupHandle &setS, SetNames &setNames) = 0;
	
    virtual std::map<std::string, GroupInformation> 	GetAllGroupInformation() = 0;

	virtual int GetInterfaceId() const { return GroupManagerInterface::GroupManagerInterfaceId; }
};

class DLL_STATE GroupManagerClient : public RmiClient, public GroupManagerInterface
{
public:
    typedef std::map<std::string, GroupInformation>		MapGroupNameGroupInformation;
    typedef std::set<GroupHandle>						SetGroupHandle;
    typedef std::set<std::string>						SetNames;

public:
	RMICLIENT_COMMON(GroupManagerClient);

	virtual GroupInformation JoinGroup(HostInformation hostInfo, GroupHandle group)
	{
		BeginMarshal(GroupManagerInterface::Method_JoinGroup);
		hostInfo.Write(GetSocketWriter());
		group.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		GroupInformation groupInfo;
		groupInfo.Read(GetSocketReader());		
		EndUnmarshal();

		return groupInfo;
	}

    virtual std::map<std::string, GroupInformation> JoinGroups(HostInformation hostInfo, SetGroupHandle setNames)
	{
		BeginMarshal(GroupManagerInterface::Method_JoinGroups);
			hostInfo.Write(GetSocketWriter());
			GetSocketWriter()->WriteInt32(setNames.size());
			for(SetGroupHandle::iterator it = setNames.begin(), it_end = setNames.end(); it != it_end; ++it)
			{
				GroupHandle handle = *it;
				handle.Write(GetSocketWriter());
			}
		EndMarshal();

		BeginUnmarshal();
			MapGroupNameGroupInformation subscribedGroups;
			int size = GetSocketReader()->ReadInt32();
			for(int i = 0; i < size; i++)
			{
                std::string groupName = GetSocketReader()->ReadString();
				GroupInformation info;
				info.Read(GetSocketReader());
                subscribedGroups.insert(std::pair<std::string, GroupInformation>(groupName, info));
			}
		EndMarshal();

		return subscribedGroups;
	}

	virtual int LeaveGroup(HostInformation hostInfo, GroupHandle group)
	{
		BeginMarshal(GroupManagerInterface::Method_LeaveGroup);
		hostInfo.Write(GetSocketWriter());
		group.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();	
		EndUnmarshal();

		return ret;
	}

	virtual int LeaveGroups(HostInformation hostInfo, SetGroupHandle setNames) 
	{
		BeginMarshal(GroupManagerInterface::Method_LeaveGroups);
			hostInfo.Write(GetSocketWriter());
			GetSocketWriter()->WriteInt32(setNames.size());
			for(SetGroupHandle::iterator it = setNames.begin(), it_end = setNames.end(); it != it_end; ++it)
			{
				GroupHandle handle = *it;
				handle.Write(GetSocketWriter());
			}
		EndMarshal();

		BeginUnmarshal();
			int ret = GetSocketReader()->ReadInt32();
		EndMarshal();

		return ret;
	}

	virtual GroupInformation GroupChange(HostInformation hostInfo, GroupHandle currentGroup, GroupHandle newGroup)
	{
		BeginMarshal(GroupManagerInterface::Method_GroupChange);
		hostInfo.Write(GetSocketWriter());
		currentGroup.Write(GetSocketWriter());
		newGroup.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		GroupInformation groupInfo;
		groupInfo.Read(GetSocketReader());		
		EndUnmarshal();

		return groupInfo;
	}
	
    virtual GroupInformation GetGroupInformation(std::string groupName)
	{
		BeginMarshal(GroupManagerInterface::Method_GroupInformation);
		GetSocketWriter()->WriteString(groupName);
		EndMarshal();

		BeginUnmarshal();
		GroupInformation groupInfo;
		groupInfo.Read(GetSocketReader());		
		EndUnmarshal();

		return groupInfo;
	}

	virtual GroupInformation AliveChecker(HostInformation hostInfo, GroupHandle group)
	{
		BeginMarshal(GroupManagerInterface::Method_AliveChecker);
		hostInfo.Write(GetSocketWriter());
		group.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		GroupInformation groupInfo;
		groupInfo.Read(GetSocketReader());		
		EndUnmarshal();

		return groupInfo;
	}
	
	virtual GroupConnections AliveChecker(HostInformation &hostInfo, SetGroupHandle &setHandles, SetNames &setNames) 
	{
		BeginMarshal(GroupManagerInterface::Method_AliveCheckerMaps);
			hostInfo.Write(GetSocketWriter());
			GetSocketWriter()->WriteInt32(setHandles.size());
			for(SetGroupHandle::iterator it = setHandles.begin(), it_end = setHandles.end(); it != it_end; ++it)
			{
				GroupHandle handle = *it;
				handle.Write(GetSocketWriter());
			}

			GetSocketWriter()->WriteInt32(setNames.size());
			for(SetNames::iterator it = setNames.begin(), it_end = setNames.end(); it != it_end; ++it)
				GetSocketWriter()->WriteString(*it);
		EndMarshal();

		BeginUnmarshal();
			GroupConnections groupConnections;
			groupConnections.Read(GetSocketReader());

			/*MapGroupNameGroupInformation subscribedGroups;
			int size = GetSocketReader()->ReadInt32();
			for(int i = 0; i < size; i++)
			{
				string groupName = GetSocketReader()->ReadString();
				GroupInformation info;
				info.Read(GetSocketReader());
                subscribedGroups.insert(std::pair<string, GroupInformation>(groupName, info));
			}*/
		EndMarshal();

		return groupConnections;
	}

    virtual std::map<std::string, GroupInformation> GetAllGroupInformation()
	{
		BeginMarshal(GroupManagerInterface::Method_AllGroupInformation);
		EndMarshal();

		BeginUnmarshal();
		
        std::map<std::string, GroupInformation> mapServiceGroupInfo;
		int sz = GetSocketReader()->ReadInt32();
		for(int i = 0; i < sz; i++)
		{
            std::string name = GetSocketReader()->ReadString();
			GroupInformation groupInfo;
			groupInfo.Read(GetSocketReader());

            mapServiceGroupInfo.insert(std::pair<std::string, GroupInformation>(name, groupInfo));
		}

		EndUnmarshal();

		return mapServiceGroupInfo;
	}
/*protected:
	virtual void clientConnected() 
	{ 
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RegisterService(clientInformation_);
	}

	virtual void clientDisconnected() 
	{ 
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RemoveService(clientInformation_.GetComponentName());
	}
*/
};

// -------------------------------------------------------
//				GroupManager Stub
//			- Receiver/consumer class
// -------------------------------------------------------
class DLL_STATE GroupManagerServer : public RmiServerListener
{
public:
    typedef std::map<std::string, GroupInformation>		MapGroupNameGroupInformation;
	typedef std::set<GroupHandle>					SetGroupHandle;
    typedef std::set<std::string>						SetNames;

public:
	GroupManagerServer(const InterfaceHandle &handle, GroupManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(handle, autoStart, mode)
		, groupManager_(serverInterface)
	{ }
    GroupManagerServer(const std::string &serviceName, const InterfaceHandle &handle, GroupManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(serviceName, handle, autoStart, mode)
		, groupManager_(serverInterface)
	{ }

	virtual ~GroupManagerServer() {}

	virtual void RMInvocation(int methodId, RmiBase* rmiServer)
	{
		switch(methodId)
		{
			case GroupManagerInterface::Method_GroupChange:
			{
				HostInformation hostInfo;
				hostInfo.Read(rmiServer->GetSocketReader());
				
				GroupHandle currentGroup, newGroup;
				currentGroup.Read(rmiServer->GetSocketReader());
				newGroup.Read(rmiServer->GetSocketReader());

				rmiServer->EndUnmarshal();
				
				GroupInformation info = groupManager_->GroupChange(hostInfo, currentGroup, newGroup);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_GroupChange);
				info.Write(rmiServer->GetSocketWriter());
				rmiServer->EndMarshal();
				break;
			}
			case GroupManagerInterface::Method_GroupInformation:
			{	
                std::string groupName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();
				
				GroupInformation info = groupManager_->GetGroupInformation(groupName);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_GroupInformation);
				info.Write(rmiServer->GetSocketWriter());
				rmiServer->EndMarshal();
				break;
			}
			case GroupManagerInterface::Method_AliveChecker:
			{
				HostInformation hostInfo;
				hostInfo.Read(rmiServer->GetSocketReader());
				
				GroupHandle group;
				group.Read(rmiServer->GetSocketReader());
				rmiServer->EndUnmarshal();
				
				GroupInformation info = groupManager_->AliveChecker(hostInfo, group);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_AliveChecker);
				info.Write(rmiServer->GetSocketWriter());
				rmiServer->EndMarshal();	
				break;
			}
			case GroupManagerInterface::Method_AliveCheckerMaps:
			{
					HostInformation hostInfo;
					hostInfo.Read(rmiServer->GetSocketReader());

					SetGroupHandle setHandles;
					int size = rmiServer->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						GroupHandle handle;
						handle.Read(rmiServer->GetSocketReader());
						setHandles.insert(handle);
					}

					SetNames setNames;
					size = rmiServer->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
                        std::string group = rmiServer->GetSocketReader()->ReadString();
						setNames.insert(group);
					}
				rmiServer->EndUnmarshal();

				GroupConnections groupConnections = groupManager_->AliveChecker(hostInfo, setHandles, setNames);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_AliveCheckerMaps);
					groupConnections.Write(rmiServer->GetSocketWriter());

					/*rmiServer->GetSocketWriter()->WriteInt32(subscribedGroups.size());
					for(MapGroupNameGroupInformation::iterator it = subscribedGroups.begin(), it_end = subscribedGroups.end(); it != it_end; ++it)
					{
						rmiServer->GetSocketWriter()->WriteString(it->first);

						GroupInformation &info = it->second;
						info.Write(rmiServer->GetSocketWriter());
					}*/
				rmiServer->EndMarshal();
				break;
			}
			case GroupManagerInterface::Method_LeaveGroup:
			{
				HostInformation hostInfo;
				hostInfo.Read(rmiServer->GetSocketReader());
				
				GroupHandle group;
				group.Read(rmiServer->GetSocketReader());
				rmiServer->EndUnmarshal();
				
				int ret = groupManager_->LeaveGroup(hostInfo, group);
				
				rmiServer->BeginMarshal(GroupManagerInterface::Method_LeaveGroup);
				rmiServer->GetSocketWriter()->WriteInt32(ret);
				rmiServer->EndMarshal();	
				break;			
			}
			case GroupManagerInterface::Method_LeaveGroups:
			{
					HostInformation hostInfo;
					hostInfo.Read(rmiServer->GetSocketReader());

					SetGroupHandle setNames;
					int size = rmiServer->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						GroupHandle handle;
						handle.Read(rmiServer->GetSocketReader());
						setNames.insert(handle);
					}
				rmiServer->EndUnmarshal();

				int ret = groupManager_->LeaveGroups(hostInfo, setNames);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_LeaveGroups);
					rmiServer->GetSocketWriter()->WriteInt32(ret); 
				rmiServer->EndMarshal();
				break;
			}
			case GroupManagerInterface::Method_JoinGroup:
			{
				HostInformation hostInfo;
				hostInfo.Read(rmiServer->GetSocketReader());
				
				GroupHandle group;
				group.Read(rmiServer->GetSocketReader());
				rmiServer->EndUnmarshal();
				
				GroupInformation groupInfo = groupManager_->JoinGroup(hostInfo, group);
				
				rmiServer->BeginMarshal(GroupManagerInterface::Method_JoinGroup);
				groupInfo.Write(rmiServer->GetSocketWriter());
				rmiServer->EndMarshal();	
				break;
			}
			case GroupManagerInterface::Method_JoinGroups:
			{
					HostInformation hostInfo;
					hostInfo.Read(rmiServer->GetSocketReader());

					SetGroupHandle setNames;
					int size = rmiServer->GetSocketReader()->ReadInt32();
					for(int i = 0; i < size; i++)
					{
						GroupHandle handle;
						handle.Read(rmiServer->GetSocketReader());
						setNames.insert(handle);
					}
				rmiServer->EndUnmarshal();

				MapGroupNameGroupInformation subscribedGroups = groupManager_->JoinGroups(hostInfo, setNames);

				rmiServer->BeginMarshal(GroupManagerInterface::Method_JoinGroups);
					rmiServer->GetSocketWriter()->WriteInt32(subscribedGroups.size());
					for(MapGroupNameGroupInformation::iterator it = subscribedGroups.begin(), it_end = subscribedGroups.end(); it != it_end; ++it)
					{
						rmiServer->GetSocketWriter()->WriteString(it->first);

						GroupInformation &info = it->second;
						info.Write(rmiServer->GetSocketWriter());
					}
				rmiServer->EndMarshal();
				break;
			}
			case GroupManagerInterface::Method_AllGroupInformation:
			{
				rmiServer->EndUnmarshal();
			
				MapGroupNameGroupInformation subscribedGroups = groupManager_->GetAllGroupInformation();

				rmiServer->BeginMarshal(GroupManagerInterface::Method_AllGroupInformation);
					rmiServer->GetSocketWriter()->WriteInt32(subscribedGroups.size());
					for(MapGroupNameGroupInformation::iterator it = subscribedGroups.begin(), it_end = subscribedGroups.end(); it != it_end; ++it)
					{
						rmiServer->GetSocketWriter()->WriteString(it->first);

						GroupInformation &info = it->second;
						info.Write(rmiServer->GetSocketWriter());
					}
				rmiServer->EndMarshal();
				break;
			}
			default:
				IWARNING() << " RMI not implemented " << methodId ;
				break;
		}
	}

protected:
	virtual void ServerStartedListening() 
	{ 
		MutexLocker lock(&mutex_);

		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RegisterService(serverInformation_);
	}

	virtual void ServerStoppedListening() 
	{ 
		MutexLocker lock(&mutex_);
		
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RemoveService(serverInformation_.GetComponentName());
	}


private:
	GroupManagerInterface *groupManager_;
};

} // namespace end


#endif //




