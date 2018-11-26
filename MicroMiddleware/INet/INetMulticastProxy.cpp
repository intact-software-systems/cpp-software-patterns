#include "MicroMiddleware/INet/INetMulticastProxy.h"
#include "MicroMiddleware/INet/INetConnectionManager.h"
#include "MicroMiddleware/INet/INetProxy.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"

namespace MicroMiddleware
{

map<InterfaceHandle, INetProxy*>& INetMulticastProxy::GetGroup(const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(mapGroupProxies_.count(groupName) > 0);

	MapHandleProxy &mapINetProxy = mapGroupProxies_[groupName];
	return mapINetProxy;
}

void INetMulticastProxy::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
{
	MutexLocker lock(&clientMutex_);
	
	if(mapGroupProxies_.count(groupName) <= 0) 
	{
		IDEBUG() << "WARNING! No members in group " << groupName;
		return;
	}

	ASSERT(rpcObject != NULL);
	ASSERT(groupName.empty() == false);
	ASSERT(mapGroupProxies_.count(groupName) > 0);
	
	MapHandleProxy &mapINetProxy = mapGroupProxies_[groupName];
	for(MapHandleProxy::iterator it = mapINetProxy.begin(), it_end = mapINetProxy.end(); it != it_end; ++it)
	{
		INetProxy *proxy = it->second;
		ASSERT(proxy);

		if(proxy->IsConnected() == false) 
		{
			IDEBUG()  << *rpc << " client disconnected " << it->first;
			continue;
		}
		
		//IDEBUG() << *rpc << " sending to remote stub" ;
		proxy->ExecuteRpc(rpc, rpcObject);
	}
}

/*---------------------------------------------------------------------------------
 TODO:
 	- Goal: Make each INetProxy visible as a Proxy in ServiceConnectionManager
	- How to achieve when there is no interface?:
		- Is it possible to have a ComponentBase* INetProxy::GetParent()?
		- RegisterOrRemoveProxies returns a list of current proxies, these are added to the list in the implementation of the Interface?
----------------------------------------------------------------------------------*/
void INetMulticastProxy::RegisterOrRemoveProxies(GroupInformation &groupInfo, const std::string &interfaceName)
{
	MutexLocker lock(&clientMutex_);
	IWARNING()  << " Adding clients from group name: " << groupInfo.GetGroupName() ;
	
	if(mapGroupProxies_.count(groupInfo.GetGroupName()) <= 0)
		mapGroupProxies_.insert(pair<string, MapHandleProxy>(groupInfo.GetGroupName(), MapHandleProxy()));

	MapHandleProxy &mapINetProxy = mapGroupProxies_[groupInfo.GetGroupName()];
	
	INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
	ASSERT(connectionManager);

	for(GroupInformation::MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		const HostInformation &hostInfo = it->second;
		IWARNING()  << hostInfo ;
		
		InterfaceHandle handle(hostInfo.GetPortNumber(), hostInfo.GetHostName());
	
		if(hostInfo.GetOnlineStatus() == HostInformation::OFFLINE) // -> remove
		{
			if(mapINetProxy.count(handle) > 0)
			{
				INetProxy *proxy  = mapINetProxy[handle];
				ASSERT(proxy);
				ASSERT(proxy->IsValid());

				//if(proxy->GetName().empty() == false)
				{
					bool ret = connectionManager->RemoveInterface(handle, proxy);
					ASSERT(ret);
				}

				delete proxy;
				mapINetProxy.erase(handle);
			}
			else
			{
				IWARNING()  << "Already removed " << hostInfo ;
			}
		}
		else // ONLINE -> add
		{
			if(mapINetProxy.count(handle) <= 0)
			{
				// -- debug --
				IDEBUG() << " New Proxy for " << groupInfo.GetGroupName() << " connected to " << hostInfo.GetConnectedToName();
				ASSERT(hostInfo.GetConnectedToName().empty() == false);
				ASSERT(groupInfo.GetGroupName() != interfaceName);
				// -- debug --

				INetProxy* proxy = new INetProxy(hostInfo.GetComponentName(), hostInfo.GetConnectedToName(), interfaceName);

				bool ret = connectionManager->RegisterInterface(handle, proxy);
				ASSERT(ret);

				mapINetProxy.insert(pair<InterfaceHandle, INetProxy*>(handle, proxy));
			}
			else
			{
				GroupHandle handle(groupInfo.GetGroupName(), hostInfo.GetHostName(), hostInfo.GetPortNumber());
				cerr << TSPRETTY_FUNCTION << " " << handle << " " << hostInfo.GetComponentName() << " is already added" << endl;
			}
		}
	}
}

/*void INetMulticastProxy::RegisterProxy(const GroupHandle &groupHandle, INetProxy *proxy)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(groupHandle.GetGroupName().empty() == false);
	ASSERT(proxy);

	if(mapGroupProxies_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupProxies_.insert(pair<string, MapHandleProxy>(groupHandle.GetGroupName(), MapHandleProxy()));

	MapHandleProxy &mapINetProxy = mapGroupProxies_[groupHandle.GetGroupName()];
	InterfaceHandle handle(groupHandle.GetPortNumber(), groupHandle.GetHostName());

	if(mapINetProxy.count(handle) > 0)
		IWARNING()  << "WARNING! Already a target " << groupHandle ;
	
	INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
	ASSERT(connectionManager);

	INetConnection *conn = connectionManager->GetOrCreateConnection(handle);
	ASSERT(conn);
	
	bool ret = connectionManager->RegisterInterface(handle, proxy);
	IWARNING()  << " Adding client " << groupHandle ;
	ASSERT(ret);
	
	mapINetProxy.insert(pair<InterfaceHandle, INetProxy*>(handle, proxy));
}

void INetMulticastProxy::RemoveProxy(const GroupHandle &groupHandle)
{
	MutexLocker lock(&clientMutex_);
	ASSERT(groupHandle.GetGroupName().empty() == false);

	if(mapGroupProxies_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupProxies_.insert(pair<string, MapHandleProxy>(groupHandle.GetGroupName(), MapHandleProxy()));

	MapHandleProxy &mapINetProxy = mapGroupProxies_[groupHandle.GetGroupName()];
	
	InterfaceHandle handle(groupHandle.GetPortNumber(), groupHandle.GetHostName());
	
	if(mapINetProxy.count(handle) > 0)
	{
		INetProxy *proxy = mapINetProxy[handle];
		ASSERT(proxy != NULL);
		
		INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
		ASSERT(connectionManager);
		bool ret = connectionManager->RemoveInterface(handle, proxy);
		ASSERT(ret);

		delete proxy;
		mapINetProxy.erase(handle);
	}
	else
	{
		IWARNING()  << "WARNING! Already removed " << groupHandle ;
	}
}*/

}; // namespace end



