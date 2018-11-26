#include"IntactMiddleware/MulticastManager.h"

namespace IntactMiddleware
{

MulticastManager::MulticastManager(const InterfaceHandle &handle, bool autorun)
	: multicastServerHandle_(handle)
{
	multicastClient_ = new RpcMulticastClient(multicastServerHandle_);
	multicastServer_ = new RpcMulticastServerListener(multicastServerHandle_, multicastClient_, true);

	if(autorun) start();
}

void MulticastManager::run()
{
	//while(true)
	//{
		multicastServer_->wait();
	//}
}

bool MulticastManager::RegisterInterface(int interfaceId, RpcServer *rpcServer)
{
	return multicastServer_->RegisterInterface(interfaceId, rpcServer);
}

void MulticastManager::AddOrRemoveMulticastClients(GroupInformation groupInfo)
{
	multicastClient_->AddOrRemoveMulticastClients(groupInfo);
}

void MulticastManager::RemoveMulticastClient(GroupHandle &handle)
{
	multicastClient_->RemoveMulticastClient(handle);
}

void MulticastManager::AddMulticastClient(GroupHandle &handle)
{
	multicastClient_->AddMulticastClient(handle);
}

} // namespace IntactMiddleware
