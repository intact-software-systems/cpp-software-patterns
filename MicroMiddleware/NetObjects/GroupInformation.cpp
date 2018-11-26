#include "MicroMiddleware/NetObjects/GroupInformation.h"

namespace MicroMiddleware
{

bool GroupInformation::RemoveHost(string componentName)	
{
	//MutexLocker lock(&objectMutex_);

	if(mapGroupMembers_.count(componentName) <= 0)
	{
		IWARNING()  << "WARNING! " << componentName << " is not a member of " << this->GetGroupName() ;
		return false;
	}

	mapGroupMembers_.erase(componentName);

	return true;
}

void GroupInformation::AddHost(const HostInformation &hostInfo)
{
	//MutexLocker lock(&objectMutex_);

	if(mapGroupMembers_.count(hostInfo.GetComponentName()) >= 1)
	{
		IWARNING()  << "WARNING! " << hostInfo.GetComponentName()<< " is already a member of " << this->GetGroupName() ;
		mapGroupMembers_[hostInfo.GetComponentName()] = hostInfo;
	}
	else
	{
		mapGroupMembers_.insert(pair<string, HostInformation>(hostInfo.GetComponentName(), hostInfo));
	}
}

bool GroupInformation::UpdateHost(const HostInformation &hostInfo)
{
	//MutexLocker lock(&objectMutex_);

	if(mapGroupMembers_.count(hostInfo.GetComponentName()) <= 0)
	{
		IWARNING()  << "ERROR! " << hostInfo.GetComponentName() << " is not a member of " << this->GetGroupName() ;
		return false;
	}
	
	mapGroupMembers_[hostInfo.GetComponentName()] = hostInfo;
	return true;
}

bool GroupInformation::UpdateHost(const HostInformation &hostInfo, const GroupHandle &groupHandle)
{
	//MutexLocker lock(&objectMutex_);
	if(mapGroupMembers_.count(hostInfo.GetComponentName()) <= 0)
	{
		IWARNING()  << "ERROR! " << hostInfo.GetComponentName() << " is not a member of " << this->GetGroupName() ;
		return false;
	}
	
	mapGroupMembers_[hostInfo.GetComponentName()] = hostInfo;
		
	HostInformation &groupHostInfo = mapGroupMembers_[hostInfo.GetComponentName()];
	groupHostInfo.SetHostName(groupHandle.GetHostName());
	groupHostInfo.SetPortNumber(groupHandle.GetPortNumber());
	return true;
}


bool GroupInformation::IsMember(string componentName, HostInformation::OnlineStatus status)
{
	//MutexLocker lock(&objectMutex_);

	if(mapGroupMembers_.count(componentName) <= 0)
		return false;

	HostInformation &hostInfo = mapGroupMembers_[componentName];
	if(status == HostInformation::NO_ONLINE_STATUS)
		return true;
	else if(hostInfo.GetOnlineStatus() == status)
		return true;

	return false;
}

HostInformation GroupInformation::GetHostInformation(string componentName)
{
	//MutexLocker lock(&objectMutex_);	

	if(mapGroupMembers_.count(componentName) <= 0)
		return HostInformation();
	
	return mapGroupMembers_[componentName];
}

bool GroupInformation::CompareTo(GroupInformation &info)
{
	if(info.GetNumberOfMembers()!= GetNumberOfMembers())
	{
		IWARNING()  << " Group informations are not equal!" ;
		return false;
	}

	for(MapHostInformation::iterator it = info.GetGroupMembers().begin(), it_end = info.GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation hostInfo = it->second;
		if(IsMember(hostInfo.GetComponentName(), HostInformation::NO_ONLINE_STATUS))
		{
			HostInformation myHostInfo = GetHostInformation(hostInfo.GetComponentName());
			ASSERT(myHostInfo.IsValid());

			if(hostInfo.CompareTo(myHostInfo) == false)
			{
				IWARNING()  << " Group informations are not equal!" ;
				return false;
			}
		}
		else 
		{
			IWARNING()  << " Group informations are not equal!" ;
			return false;
		}
	}
	
	for(MapHostInformation::iterator it = GetGroupMembers().begin(), it_end = GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation hostInfo = it->second;
		if(info.IsMember(hostInfo.GetComponentName(), HostInformation::NO_ONLINE_STATUS))
		{
			HostInformation hisHostInfo = info.GetHostInformation(hostInfo.GetComponentName());
			ASSERT(hisHostInfo.IsValid());

			if(hostInfo.CompareTo(hisHostInfo) == false)
			{
				IWARNING()  << " Group informations are not equal!" ;
				return false;
			}
		}
		else 
		{
			IWARNING()  << " Group informations are not equal!" ;
			return false;
		}
	}
	return true;
}


map<string, HostInformation> GroupInformation::GetMembersNotIn(GroupInformation &info)
{
	MapHostInformation membershipChange;

	for(MapHostInformation::iterator it = GetGroupMembers().begin(), it_end = GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation hostInfo = it->second;
		if(!info.IsMember(hostInfo.GetComponentName(), HostInformation::NO_ONLINE_STATUS))
		{
			//HostInformation hisHostInfo = info.GetHostInformation(hostInfo.GetComponentName());
			//ASSERT(hisHostInfo.IsValid());
			membershipChange.insert(pair<string, HostInformation>(hostInfo.GetComponentName(), hostInfo));
		}
	}

	return membershipChange;
}


map<string, HostInformation> GroupInformation::GetMembersChanged(GroupInformation &info)
{
	MapHostInformation membersChanged;

	for(MapHostInformation::iterator it = GetGroupMembers().begin(), it_end = GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation hostInfo = it->second;
		if(info.IsMember(hostInfo.GetComponentName(), HostInformation::NO_ONLINE_STATUS))
		{
			HostInformation hisHostInfo = info.GetHostInformation(hostInfo.GetComponentName());
			ASSERT(hisHostInfo.IsValid());

			if(hostInfo.CompareTo(hisHostInfo) == false)
				membersChanged.insert(pair<string, HostInformation>(hisHostInfo.GetComponentName(), hisHostInfo));
		}
	}

	return membersChanged;
}

std::list<NetworkLib::NetAddress> GroupInformation::GetNetAddresses() const
{
	std::list<NetworkLib::NetAddress> netAddresses;

	for(MapHostInformation::const_iterator it = mapGroupMembers_.begin(), it_end = mapGroupMembers_.end(); it != it_end; ++it)
	{
		const HostInformation &hostInfo = it->second;
		netAddresses.push_back(NetworkLib::NetAddress(hostInfo.GetHostIP(), hostInfo.GetPortNumber()));
	}
	return netAddresses;
}

void GroupInformation::Print(std::ostream &ostr)
{
	if(GetGroupMembers().empty() == true) return;

	ostr << GetGroupName() << ":" << endl;
	for(MapHostInformation::iterator it = GetGroupMembers().begin(), it_end = GetGroupMembers().end(); it != it_end; ++it)
	{
		HostInformation hostInfo = it->second;
		ostr << hostInfo << endl;
	}
}

}; // namespace MicroMiddleware

