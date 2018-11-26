#include "MicroMiddleware/Net/AlmMulticastPublisher.h"
#include "MicroMiddleware/Net/AlmPublisher.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"

namespace MicroMiddleware
{

map<ClientTriplet, AlmPublisher*>& AlmMulticastPublisher::GetRpcGroup(const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(mapGroupNameAlmPublisher_.count(groupName) > 0);

	MapClientTripletAlmPublisher &mapAlmPublisher = mapGroupNameAlmPublisher_[groupName];
	return mapAlmPublisher;
}

void AlmMulticastPublisher::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	
	ASSERT(rpcObject != NULL);
	ASSERT(!groupName.empty());
	ASSERT(mapGroupNameAlmPublisher_.count(groupName) > 0);

	//char* buf = rpcObject->toRaw();
	//int length = rpcObject->GetLength();

	MapClientTripletAlmPublisher &mapAlmPublisher = mapGroupNameAlmPublisher_[groupName];
	for(MapClientTripletAlmPublisher::iterator it = mapAlmPublisher.begin(), it_end = mapAlmPublisher.end(); it != it_end; ++it)
	{
		AlmPublisher *almPublisher = it->second;
		ASSERT(almPublisher);

		if(!almPublisher->IsConnected())
		{
			IWARNING()  << *rpc << " client on " << almPublisher->GetInterfaceHandle() << " is disconnected " ;
			continue;
		}
		
		IDEBUG() << *rpc << " sending to almPublisher " << almPublisher->GetInterfaceHandle() ;
		almPublisher->ExecuteRpc(rpc, rpcObject);
	}
}

void AlmMulticastPublisher::AddOrRemoveMulticastClients(GroupInformation &groupInfo)
{
	MutexLocker lock(&clientMutex_);
	IWARNING()  << " Adding clients from group name: " << groupInfo.GetGroupName() ;
	
	if(mapGroupNameAlmPublisher_.count(groupInfo.GetGroupName()) <= 0)
		mapGroupNameAlmPublisher_.insert(pair<string, MapClientTripletAlmPublisher>(groupInfo.GetGroupName(), MapClientTripletAlmPublisher()));

	MapClientTripletAlmPublisher &mapAlmPublisher = mapGroupNameAlmPublisher_[groupInfo.GetGroupName()];
	
	for(GroupInformation::MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		const HostInformation &hostInfo = it->second;
		IWARNING()  << hostInfo ;
		
								// socket, port, host
		ClientTriplet clientTriplet(0, hostInfo.GetPortNumber(), hostInfo.GetHostName());
	
		if(hostInfo.GetOnlineStatus() == HostInformation::OFFLINE) // -> remove
		{
			if(mapAlmPublisher.count(clientTriplet) > 0)
			{
				AlmPublisher *almPublisher  = mapAlmPublisher[clientTriplet];
				ASSERT(almPublisher != NULL);
				almPublisher->StopClient();

				delete almPublisher;
				mapAlmPublisher.erase(clientTriplet);
			}
			else
			{
				IWARNING()  << "Already removed " << hostInfo ;
			}
		}
		else // ONLINE -> add
		{
			if(mapAlmPublisher.count(clientTriplet) <= 0)
			{
				InterfaceHandle handle(hostInfo.GetHostName(), hostInfo.GetPortNumber());

				AlmPublisher* almPublisher = new AlmPublisher(handle);
				almPublisher->StartClient();
				almPublisher->WaitForUdpSocket();
			
				IWARNING()  << " Adding as target " << multicastServerInterface_ ;
				GroupHandle groupServerHandle(groupInfo.GetGroupName(), multicastServerInterface_);

				almPublisher->AddTarget(groupServerHandle);

				mapAlmPublisher.insert(pair<ClientTriplet, AlmPublisher*>(clientTriplet, almPublisher));
			}
			else
			{
				IWARNING()  << " " << groupInfo.GetGroupName() << " Already a target " << multicastServerInterface_;
			}
		}
	}
}

void AlmMulticastPublisher::AddMulticastClient(const GroupHandle &groupHandle)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(!groupHandle.GetGroupName().empty());

	if(mapGroupNameAlmPublisher_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupNameAlmPublisher_.insert(pair<string, MapClientTripletAlmPublisher>(groupHandle.GetGroupName(), MapClientTripletAlmPublisher()));

	MapClientTripletAlmPublisher &mapAlmPublisher = mapGroupNameAlmPublisher_[groupHandle.GetGroupName()];
	
	ClientTriplet clientTriplet(0, groupHandle.GetPortNumber(),groupHandle.GetHostName());

	if(mapAlmPublisher.count(clientTriplet) <= 0)
	{
		IWARNING()  << " Adding client " << groupHandle ;
		
		AlmPublisher* almPublisher = new AlmPublisher(
			InterfaceHandle(groupHandle.GetHostName(), groupHandle.GetPortNumber()));
		almPublisher->StartClient();
		almPublisher->WaitConnected();
		ASSERT(almPublisher->IsConnected());

		mapAlmPublisher.insert(pair<ClientTriplet, AlmPublisher*>(clientTriplet, almPublisher));
	}
	else
	{
		IWARNING()  << "WARNING! Already a target " << groupHandle ;
	}
}

void AlmMulticastPublisher::RemoveMulticastClient(const GroupHandle &groupHandle)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(!groupHandle.GetGroupName().empty());

	if(mapGroupNameAlmPublisher_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupNameAlmPublisher_.insert(pair<string, MapClientTripletAlmPublisher>(groupHandle.GetGroupName(), MapClientTripletAlmPublisher()));

	MapClientTripletAlmPublisher &mapAlmPublisher = mapGroupNameAlmPublisher_[groupHandle.GetGroupName()];
	
	ClientTriplet clientTriplet(0, groupHandle.GetPortNumber(), groupHandle.GetHostName());

	if(mapAlmPublisher.count(clientTriplet) > 0)
	{
		AlmPublisher *almPublisher  = mapAlmPublisher[clientTriplet];
		ASSERT(almPublisher != NULL);
		almPublisher->StopClient();

		delete almPublisher;
		mapAlmPublisher.erase(clientTriplet);
	}
	else
	{
		IWARNING()  << "WARNING! Already removed " << groupHandle ;
	}
}

}; // namespace end



