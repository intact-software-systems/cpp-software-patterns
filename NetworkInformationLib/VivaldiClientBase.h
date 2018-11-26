#ifndef NetworkInformationLib_VivaldiClientBase_h_IsIncluded
#define NetworkInformationLib_VivaldiClientBase_h_IsIncluded

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/VivaldiInformation.h"
#include "NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{

class DLL_STATE VivaldiClientInterface : public RmiInterface
{
public:
	VivaldiClientInterface() { }
	~VivaldiClientInterface() { }

	enum RMIMethodId 
	{
		RmiGroupUpdate = 21			// VivaldiSpace::FROM_SERVER
	};

	enum InterfaceId
	{
		VivaldiClientInterfaceId = 32
	};

	//virtual void SetGroupUpdate(GroupInformation &groupInfo) = 0;
	virtual void SetGroupUpdate(string groupName) = 0;

	virtual int GetInterfaceId() const { return VivaldiClientInterface::VivaldiClientInterfaceId; }
	virtual std::string GetInterfaceName() 	const { return RPC_FUNCTION_STRING; }
};

// -------------------------------------------------------
//				VivaldiClientProxy
// -------------------------------------------------------
class DLL_STATE VivaldiClientProxy : public RmiClient, public VivaldiClientInterface
{
public:
	RMICLIENT_COMMON(VivaldiClientProxy)

	//virtual void SetGroupUpdate(GroupInformation &groupInfo)
	//{
	//	ExecuteOneWayRMI(VivaldiClientInterface::RmiGroupUpdate, &groupInfo);
	//}

	virtual void SetGroupUpdate(string groupName)
	{
		BeginMarshal(VivaldiClientInterface::RmiGroupUpdate);
		GetSocketWriter()->WriteString(groupName);
		EndMarshal();
	}
};

// -------------------------------------------------------
//				VivaldiClientServer 
// -------------------------------------------------------
class DLL_STATE VivaldiClientStub : public RmiServerListener
{
public:
	VivaldiClientStub(const InterfaceHandle &handle, VivaldiClientInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(handle, autoStart, mode)
		, vivaldiManager_(serverInterface)
	{ }
	VivaldiClientStub(const string &serviceName, const InterfaceHandle &handle, VivaldiClientInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(serviceName, handle, autoStart, mode)
		, vivaldiManager_(serverInterface)
	{ }
	
	virtual ~VivaldiClientStub() {}

	virtual void RMInvocation(int methodId, RmiBase* rmiServer)
	{
		switch(methodId)
		{
			//case VivaldiClientInterface::RmiGroupUpdate:
			//{
			//	GroupInformation groupInfo;
			//	rmiServer->ReceiveRMI(&groupInfo);
			//	vivaldiManager_->SetGroupUpdate(groupInfo);
			//	break;
			//}
			case VivaldiClientInterface::RmiGroupUpdate:
			{
				string groupName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();

				vivaldiManager_->SetGroupUpdate(groupName);
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
	VivaldiClientInterface *vivaldiManager_;
};

} // namespace end

#endif


