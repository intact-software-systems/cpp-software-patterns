#include "MicroMiddleware/INet/INetMulticast.h"
#include "MicroMiddleware/INet/INetConnection.h"
#include "MicroMiddleware/RmiObjectBase.h"
#include "MicroMiddleware/NetObjects/HostInformation.h"
#include "INetConnectionManager.h"

namespace MicroMiddleware
{
INetMulticast::INetMulticast(const InterfaceHandle &multicastServer)
		: multicastServerInterface_(multicastServer)
{ 
	ASSERT(multicastServerInterface_.IsValid());
}

INetMulticast::~INetMulticast()
{

}

void INetMulticast::run()
{
//	try
//	{
//		mutex_.lock();
//
//		while(true)
//		{
//			bool ret = waitCondition_.wait(&mutex_, 1000);
//
//			// Check for connection requests
//
//
//
//		}
//
//		mutex_.unlock();
//	}
//	catch(Exception ex)
//	{
//		mutex_.unlock();
//		iCritical() << PRETTY_FUNCTION << "Caught exception " << ex.what() << endl;
//	}
}


map<InterfaceHandle, INetConnection*>& INetMulticast::GetMulticastGroup(const string &groupName)
{
	MutexLocker lock(&mutex_);
	ASSERT(mapGroupConnection_.count(groupName) > 0);

	MapHandleConnection	&mapHandleConnection= mapGroupConnection_[groupName];
	return mapHandleConnection;
}

void INetMulticast::ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *rpcObject, const string &groupName)
{
	MutexLocker lock(&mutex_);
	
	if(mapGroupConnection_.count(groupName) <= 0) 
	{
		IDEBUG() << "WARNING! Not a member of group " << groupName ;
		return;
	}

	ASSERT(rpcObject != NULL);
	ASSERT(groupName.empty() == false);
	ASSERT(mapGroupConnection_.count(groupName) > 0);

	//char* buf = rpcObject->toRaw();
	//int length = rpcObject->GetLength();

	MapHandleConnection &mapHandleConnection= mapGroupConnection_[groupName];
	for(MapHandleConnection::iterator it = mapHandleConnection.begin(), it_end = mapHandleConnection.end(); it != it_end; ++it)
	{
		INetConnection *conn = it->second;
		ASSERT(conn);

		if(conn->IsConnected() == false) 
		{
			IWARNING()  << *rpc << " client on " << conn->GetInterfaceHandle() << " is disconnected " ;
			continue;
		}
		
		//IDEBUG() << *rpc << " sending to " << conn->GetInterfaceHandle() ;
		//conn->ExecuteRpc(rpc, rpcObject);
		ASSERT(0);
	}
}

void INetMulticast::RegisterOrRemoveConnections(const GroupInformation &groupInfo)
{
	ASSERT(groupInfo.IsValid());
	if(groupInfo.IsValid() == false) return;
	
	MutexLocker lock(&mutex_);
	
	IWARNING()  << " Adding clients from group name: " << groupInfo.GetGroupName() ;
	
	if(mapGroupConnection_.count(groupInfo.GetGroupName()) <= 0)
		mapGroupConnection_.insert(pair<string, MapHandleConnection>(groupInfo.GetGroupName(), MapHandleConnection()));

	MapHandleConnection	&mapHandleConnection = mapGroupConnection_[groupInfo.GetGroupName()];

	for(GroupInformation::MapHostInformation::iterator it = groupInfo.GetGroupMembers().begin(), it_end = groupInfo.GetGroupMembers().end(); it != it_end; ++it)
	{
		const HostInformation &hostInfo = it->second;
		IWARNING()  << hostInfo ;
		
		InterfaceHandle interfaceHandle(hostInfo.GetHostName(), hostInfo.GetPortNumber());

		if(hostInfo.GetOnlineStatus() == HostInformation::OFFLINE) // -> remove
		{
			if(mapHandleConnection.count(interfaceHandle) > 0)
				mapHandleConnection.erase(interfaceHandle);
			else
				IWARNING()  << "WARNING! Already removed " << hostInfo ;
		}
		else // ONLINE -> add
		{
			if(mapHandleConnection.count(interfaceHandle) <= 0)
			{
				INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
				ASSERT(connectionManager);

				INetConnection *conn = connectionManager->GetOrCreateConnection(interfaceHandle);
				ASSERT(conn);
				mapHandleConnection.insert(pair<InterfaceHandle, INetConnection*>(interfaceHandle, conn));
			}
			else
			{	
				GroupHandle handle(groupInfo.GetGroupName(), hostInfo.GetHostName(), hostInfo.GetPortNumber());
				IWARNING()  << " " << handle << " " << hostInfo.GetComponentName() << " is already a target for this " << multicastServerInterface_;
			}
		}
	}
}

void INetMulticast::RegisterConnection(const GroupHandle &groupHandle)
{
	MutexLocker lock(&mutex_);
	registerConnection(groupHandle);
}

void INetMulticast::RemoveConnection(const GroupHandle &groupHandle)
{
	MutexLocker lock(&mutex_);
	removeConnection(groupHandle);
}

void INetMulticast::registerConnection(const GroupHandle &groupHandle)
{
	ASSERT(groupHandle.GetGroupName().empty() == false);

	if(mapGroupConnection_.count(groupHandle.GetGroupName()) <= 0)
		mapGroupConnection_.insert(pair<string, MapHandleConnection>(groupHandle.GetGroupName(), MapHandleConnection()));

	MapHandleConnection	&mapHandleConnection = mapGroupConnection_[groupHandle.GetGroupName()];
	InterfaceHandle interfaceHandle(groupHandle.GetPortNumber(), groupHandle.GetHostName());
	
	if(mapHandleConnection.count(interfaceHandle) <= 0)
	{
		INetConnectionManager *connectionManager = INetConnectionManager::GetOrCreate();
		ASSERT(connectionManager);

		INetConnection *conn = connectionManager->GetOrCreateConnection(interfaceHandle);
		ASSERT(conn);
		mapHandleConnection.insert(pair<InterfaceHandle, INetConnection*>(interfaceHandle, conn));
	}
	else
	{
		IWARNING()  << "WARNING! Already inserted " << groupHandle ;
	}
}

void INetMulticast::removeConnection(const GroupHandle &groupHandle)
{
	ASSERT(groupHandle.GetGroupName().empty() == false);

	if(mapGroupConnection_.count(groupHandle.GetGroupName()) <= 0)
	{
		IWARNING()  << "WARNING! Already removed " << groupHandle ;
		return;
	}
	//mapGroupConnection_.insert(pair<string, MapHandleConnection>(groupHandle.GetGroupName(), MapHandleConnection()));

	MapHandleConnection	&mapHandleConnection = mapGroupConnection_[groupHandle.GetGroupName()];
	InterfaceHandle interfaceHandle(groupHandle.GetPortNumber(), groupHandle.GetHostName());
	
	if(mapHandleConnection.count(interfaceHandle) <= 0)
		IWARNING()  << "WARNING! Already removed " << groupHandle ;

	mapHandleConnection.erase(interfaceHandle);
}



}; // namespace end



