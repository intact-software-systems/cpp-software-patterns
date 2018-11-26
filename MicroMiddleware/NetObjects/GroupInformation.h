#ifndef MicroMiddleware_GroupInformation_h_IsIncluded
#define MicroMiddleware_GroupInformation_h_IsIncluded

#include <stdlib.h>
#include <iostream>
#include <map>
#include <list>

#include "MicroMiddleware/IncludeExtLibs.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"
#include "MicroMiddleware/NetObjects/GroupHandle.h"

#include "MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class DLL_STATE GroupInformation : public RmiObjectBase
{
public:
	GroupInformation(std::string groupName = std::string("")) 
        : RmiObjectBase()
		, groupName_(groupName)
	{}
	GroupInformation(const GroupInformation &other) 
        : RmiObjectBase()
		, mapGroupMembers_(other.GetGroupMembers())
		, groupName_(other.GetGroupName())
	{}
	
	GroupInformation& operator=(const GroupInformation &other)
	{
		if(this != &other)
		{
			groupName_ = other.GetGroupName();
			mapGroupMembers_ = other.GetGroupMembers();
		}
		return *this;
	}
    virtual ~GroupInformation()
	{}
	
	typedef std::map<std::string, HostInformation> 	MapHostInformation;

public:
	bool RemoveHost(std::string componentName);
	void AddHost(const HostInformation &hostInfo);
	bool IsMember(std::string componentName, HostInformation::OnlineStatus status = HostInformation::ONLINE);
	bool UpdateHost(const HostInformation &hostInfo);
	bool UpdateHost(const HostInformation &hostInfo, const GroupHandle &groupHandle);

	HostInformation GetHostInformation(std::string componentName);

	bool CompareTo(GroupInformation &groupInfo);
	void Print(std::ostream &ostr = std::cout);
	
	std::map<std::string, HostInformation> 		GetMembersNotIn(GroupInformation &info);
	std::map<std::string, HostInformation> 		GetMembersChanged(GroupInformation &info);

	std::list<NetworkLib::NetAddress> 	GetNetAddresses() const;

public:
	inline std::string 				GetGroupName() 						const 	{ MutexLocker lock(&objectMutex_); return groupName_; }								
	inline MapHostInformation 	GetGroupMembers() 						const 	{ MutexLocker lock(&objectMutex_); return mapGroupMembers_; }
	
	inline MapHostInformation& 	GetGroupMembers() 								{ MutexLocker lock(&objectMutex_); return mapGroupMembers_; }
	
	inline void					SetGroupMembers(const MapHostInformation &vec) 	{ MutexLocker lock(&objectMutex_); mapGroupMembers_  = vec; }
	inline int 					GetNumberOfMembers() 							{ MutexLocker lock(&objectMutex_); return mapGroupMembers_.size(); }
	
	bool IsValid()														const 	{ MutexLocker lock(&objectMutex_); return !groupName_.empty(); }
	bool IsEmpty()														const 	{ MutexLocker lock(&objectMutex_); return mapGroupMembers_.empty(); }

public:
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		MutexLocker lock(&objectMutex_);

		writer->WriteString(groupName_);
		writer->WriteInt32(mapGroupMembers_.size());

		for(MapHostInformation::iterator it = mapGroupMembers_.begin(), it_end = mapGroupMembers_.end(); it != it_end; ++it)
		{
			(it->second).Write(writer);
		}
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		MutexLocker lock(&objectMutex_);

		groupName_ = reader->ReadString();
		int sz = reader->ReadInt32();

		for(int i = 0; i < sz; i++)
		{
			HostInformation info;
			info.Read(reader);
			
			mapGroupMembers_.insert(pair<std::string, HostInformation>(info.GetComponentName(), info));
		}	
	}

private:
	MapHostInformation		mapGroupMembers_;
	std::string				groupName_;
	mutable Mutex			objectMutex_;
};

} // namespace end

#endif //

	/*GroupInformation(const GroupInformation &other) : groupName_(other.GetGroupName()), mapGroupMembers_(other.GetGroupMembers())
	{
	}
	GroupInformation& operator=(const GroupInformation &other)
	{
		if(this != &other)
		{
			groupName_ = other.GetGroupName();
			mapGroupMembers_ = other.GetGroupMembers();
		}
		return *this;
	}*/


