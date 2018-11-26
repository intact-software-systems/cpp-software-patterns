#include"MicroMiddleware/Net/RmiMulticast.h"

namespace MicroMiddleware
{

void RmiMulticast::StopAll(bool waitForTermination)
{
	MutexLocker lock(&clientMutex_);
	for(ListRmiClient::iterator it = GetRmiClients().begin(), it_end = GetRmiClients().end(); it != it_end; ++it)
	{
		RmiClient *rmiClient = *it;
		rmiClient->StopClient(waitForTermination);
	}
}
void RmiMulticast::StartAll()
{
	MutexLocker lock(&clientMutex_);
	for(ListRmiClient::iterator it = GetRmiClients().begin(), it_end = GetRmiClients().end(); it != it_end; ++it)
	{
		RmiClient *rmiClient = *it;
		if(rmiClient->IsRunning() == false)
			rmiClient->StartClient();
	}
}

void RmiMulticast::RestartAll()
{
	MutexLocker lock(&clientMutex_);
	for(ListRmiClient::iterator it = GetRmiClients().begin(), it_end = GetRmiClients().end(); it != it_end; ++it)
	{
		RmiClient *rmiClient = *it;
		rmiClient->StartOrRestartClient();
	}
}

RmiClient* RmiMulticast::StartAndAddClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	RmiClient *rmiClient = findRmiClient(handle);
	if(rmiClient == NULL)
	{
		rmiClient = newRmiClient(handle);
		listRmiClients_.push_back(rmiClient);
	}
	
	if(rmiClient->IsRunning() == false)
		rmiClient->StartClient();

	return rmiClient;
}

RmiClient* RmiMulticast::AddClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	RmiClient *rmiClient = findRmiClient(handle);
	if(rmiClient == NULL)
	{
		rmiClient = newRmiClient(handle);
		listRmiClients_.push_back(rmiClient);
	}

	return rmiClient;
}

RmiClient* RmiMulticast::StopAndRemoveClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	RmiClient *rmiClient = findRmiClient(handle);
	if(rmiClient == NULL) return NULL;

	if(rmiClient->IsRunning() == true)
		rmiClient->StopClient();
	
	listRmiClients_.remove(rmiClient);

	return rmiClient;
}

RmiClient* RmiMulticast::RemoveClient(const InterfaceHandle &handle)
{
	MutexLocker lock(&clientMutex_);
	RmiClient *rmiClient = findRmiClient(handle);
	if(rmiClient == NULL) return NULL;

	listRmiClients_.remove(rmiClient);

	return rmiClient;
}

RmiClient* RmiMulticast::findRmiClient(const InterfaceHandle &interfaceHandle)
{
	for(ListRmiClient::iterator it = GetRmiClients().begin(), it_end = GetRmiClients().end(); it != it_end; ++it)
	{
		RmiClient *rmiClient = *it;
		if(rmiClient->GetInterfaceHandle() == interfaceHandle)
			return rmiClient;
	}
	return NULL;
}

}; // namespace MicroMiddleware


