#ifndef IntactMiddleware_IntactManagerBase_h_IsIncluded
#define IntactMiddleware_IntactManagerBase_h_IsIncluded

#include <string>

#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/ServiceConnectionManager.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{
// -------------------------------------------------------
//				IntactManager Interfaces
// 	TODO: 
// 		- IntactManagerGUIInterface - specialized for IntactManagerGUI
// -------------------------------------------------------
/**
* RMI functions to communicate with the IntactManager.
*/
class DLL_STATE IntactManagerInterface : public RmiInterface
{
public:
	IntactManagerInterface() { }
	~IntactManagerInterface() { }

	enum MethodId
	{
		Method_AliveChecker = 10,
		Method_GetGroupUpdate
	};

	enum InterfaceId
	{
		IntactManagerInterfaceId = 200
	};

	virtual int AliveChecker(string componentName) = 0;
	virtual void GetGroupUpdate(GroupInformation &groupInfo) = 0;

	virtual int GetInterfaceId() const { return IntactManagerInterface::IntactManagerInterfaceId; }
};


class DLL_STATE IntactManagerClient : public RmiClient, public IntactManagerInterface
{
public:
	RMICLIENT_COMMON(IntactManagerClient);

	virtual int AliveChecker(string componentName)
	{
		BeginMarshal(IntactManagerInterface::Method_AliveChecker);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}

	virtual void GetGroupUpdate(GroupInformation &groupInfo)
	{
		ExecuteTwoWayRMI(IntactManagerInterface::Method_GetGroupUpdate, &groupInfo);
	}

protected:
	virtual void ClientConnected() 
	{ 
		MutexLocker lock(&mutex_);

		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RegisterService(clientInformation_);
	}

	virtual void ClientDisconnected() 
	{ 
		MutexLocker lock(&mutex_);
		
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		service->RemoveService(clientInformation_.GetComponentName());
	}
};

#ifdef SSL_SUPPORT
class DLL_STATE IntactManagerSslClient : public RmiSslClient, public IntactManagerInterface
{
public:
	RMISSLCLIENT_COMMON(IntactManagerSslClient);

	virtual int AliveChecker(string componentName)
	{
		BeginMarshal(IntactManagerInterface::Method_AliveChecker);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}

	virtual void GetGroupUpdate(GroupInformation &groupInfo)
	{
		ExecuteTwoWayRMI(IntactManagerInterface::Method_GetGroupUpdate, &groupInfo);
	}
};
#endif

// -------------------------------------------------------
//				IntactManager Stub
//			- Receiver/consumer class
// -------------------------------------------------------
class DLL_STATE IntactManagerServer : public RmiServerListener
{
public:
	IntactManagerServer(const InterfaceHandle &handle, IntactManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(handle, autoStart, mode)
		, intactManager_(serverInterface)
	{ }
	IntactManagerServer(const string &serviceName, const InterfaceHandle &handle, IntactManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(serviceName, handle, autoStart, mode)
		, intactManager_(serverInterface)
	{ }

	virtual ~IntactManagerServer() {}

	virtual void RMInvocation(int methodId, RmiBase* rmiServer)
	{
		switch(methodId)
		{
			case IntactManagerInterface::Method_AliveChecker:
			{
				string componentName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();
				
				int ret = intactManager_->AliveChecker(componentName);

				rmiServer->BeginMarshal(IntactManagerInterface::Method_AliveChecker);
				rmiServer->GetSocketWriter()->WriteInt32(ret);
				rmiServer->EndMarshal();
				break;
			}
			case IntactManagerInterface::Method_GetGroupUpdate:
			{
				GroupInformation groupInfo;
				rmiServer->ReceiveRMI(&groupInfo);
				
				intactManager_->GetGroupUpdate(groupInfo);

				rmiServer->ReplyRMI(&groupInfo);
				break;
			}
			default:
				IWARNING()  << " RMI not implemented " << methodId;
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
	IntactManagerInterface *intactManager_;
};

} // namespace end


#endif //




