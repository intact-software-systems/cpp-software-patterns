#include "MicroMiddleware/Net/RpcMulticastClient.h"
#include "MicroMiddleware/Net/RpcClient.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"

namespace MicroMiddleware
{
RpcMulticastClient::RpcMulticastClient(const InterfaceHandle &multicastServer)
		: multicastServerInterface_(multicastServer)
{ 
	ASSERT(multicastServerInterface_.IsValid());
}

RpcMulticastClient::~RpcMulticastClient()
{

}


map<ClientTriplet, RpcClient*>& RpcMulticastClient::GetRpcGroup(const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(mapGroupNameRpcClient_.count(groupName) > 0);

	MapClientTripletRpcClient &mapRpcClient = mapGroupNameRpcClient_[groupName];
	return mapRpcClient;
}

void RpcMulticastClient::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	
	if(mapGroupNameRpcClient_.count(groupName) <= 0) 
	{
		IDEBUG() << "WARNING! Not a member of group " << groupName ;
		return;
	}

	ASSERT(rpcObject != NULL);
	ASSERT(!groupName.empty());
	ASSERT(mapGroupNameRpcClient_.count(groupName) > 0);

	MapClientTripletRpcClient &mapRpcClient = mapGroupNameRpcClient_[groupName];
	for(MapClientTripletRpcClient::iterator it = mapRpcClient.begin(), it_end = mapRpcClient.end(); it != it_end; ++it)
	{
		RpcClient *rpcClient = it->second;
		ASSERT(rpcClient);

		if(!rpcClient->IsConnected())
		{
			IWARNING()  << *rpc << " client on " << rpcClient->GetInterfaceHandle() << " is disconnected " ;
			continue;
		}
		
		//IDEBUG() << *rpc << " sending to rpcClient " ;
		rpcClient->ExecuteRpc(rpc, rpcObject);
	}
}

void RpcMulticastClient::AddOrRemoveMulticastClients(GroupInformation &groupInfo)
{
	ASSERT(groupInfo.IsValid());
	if(!groupInfo.IsValid()) return;
	
	MutexLocker lock(&clientMutex_);
	
	IWARNING()  << " Adding clients from group name: " << groupInfo.GetGroupName() ;
	
	if(mapGroupNameRpcClient_.count(groupInfo.GetGroupName()) <= 0)
		mapGroupNameRpcClient_.insert(pair<string, MapClientTripletRpcClient>(groupInfo.GetGroupName(), MapClientTripletRpcClient()));

	MapClientTripletRpcClient &mapRpcClient = mapGroupNameRpcClient_[groupInfo.GetGroupName()];
	
	for(GroupInformation::MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		const HostInformation &hostInfo = it->second;
		IWARNING()  << hostInfo ;
		
								// socket, port, host
		ClientTriplet clientTriplet(0, hostInfo.GetPortNumber(), hostInfo.GetHostName());
	
		if(hostInfo.GetOnlineStatus() == HostInformation::OFFLINE) // -> remove
		{
			if(mapRpcClient.count(clientTriplet) > 0)
			{
				RpcClient *rpcClient  = mapRpcClient[clientTriplet];
				ASSERT(rpcClient != NULL);
				rpcClient->StopClient();

				delete rpcClient;
				mapRpcClient.erase(clientTriplet);
			}
			else
			{
				IWARNING()  << "Already removed " << hostInfo ;
			}
		}
		else // ONLINE -> add
		{
			if(mapRpcClient.count(clientTriplet) <= 0)
			{
				InterfaceHandle handle(hostInfo.GetHostName(), hostInfo.GetPortNumber());

				RpcClient* rpcClient = new RpcClient(handle);
				rpcClient->StartClient();
				rpcClient->WaitConnected();
			
				IWARNING()  << " Adding as target " << multicastServerInterface_ ;
				ASSERT(rpcClient->IsConnected());
				GroupHandle groupServerHandle(groupInfo.GetGroupName(), multicastServerInterface_);

				rpcClient->AddTarget(groupServerHandle);

				mapRpcClient.insert(pair<ClientTriplet, RpcClient*>(clientTriplet, rpcClient));
			}
			else
			{	
				GroupHandle handle(groupInfo.GetGroupName(), hostInfo.GetHostName(), hostInfo.GetPortNumber());
				IWARNING()  << " " << handle << " " << hostInfo.GetComponentName() << " is already a target for this " << multicastServerInterface_;
			}
		}
	}
}

void RpcMulticastClient::AddMulticastClient(const GroupHandle &groupHandle)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(!groupHandle.GetGroupName().empty());

	if(mapGroupNameRpcClient_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupNameRpcClient_.insert(pair<string, MapClientTripletRpcClient>(groupHandle.GetGroupName(), MapClientTripletRpcClient()));

	MapClientTripletRpcClient &mapRpcClient = mapGroupNameRpcClient_[groupHandle.GetGroupName()];
	
	ClientTriplet clientTriplet(0, groupHandle.GetPortNumber(),groupHandle.GetHostName());

	if(mapRpcClient.count(clientTriplet) <= 0)
	{
		IWARNING()  << " Adding client " << groupHandle ;
		
		RpcClient* rpcClient = new RpcClient(
			InterfaceHandle(groupHandle.GetHostName(), groupHandle.GetPortNumber()));
		rpcClient->StartClient();
		//rpcClient->WaitConnected();
		//ASSERT(rpcClient->IsConnected());

		mapRpcClient.insert(pair<ClientTriplet, RpcClient*>(clientTriplet, rpcClient));
	}
	else
	{
		IWARNING()  << "WARNING! Already a target " << groupHandle ;
	}
}

void RpcMulticastClient::RemoveMulticastClient(const GroupHandle &groupHandle)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(!groupHandle.GetGroupName().empty());

	if(mapGroupNameRpcClient_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupNameRpcClient_.insert(pair<string, MapClientTripletRpcClient>(groupHandle.GetGroupName(), MapClientTripletRpcClient()));

	MapClientTripletRpcClient &mapRpcClient = mapGroupNameRpcClient_[groupHandle.GetGroupName()];
	
	ClientTriplet clientTriplet(0, groupHandle.GetPortNumber(), groupHandle.GetHostName());

	if(mapRpcClient.count(clientTriplet) > 0)
	{
		RpcClient *rpcClient  = mapRpcClient[clientTriplet];
		ASSERT(rpcClient != NULL);
		rpcClient->StopClient();

		delete rpcClient;
		mapRpcClient.erase(clientTriplet);
	}
	else
	{
		IWARNING()  << "WARNING! Already removed " << groupHandle ;
	}
}

}; // namespace end


