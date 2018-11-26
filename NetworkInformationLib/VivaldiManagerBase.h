#ifndef NetworkInformationLib_VivaldiManagerBase_h_IsIncluded
#define NetworkInformationLib_VivaldiManagerBase_h_IsIncluded

#include <string>
#include"NetworkInformationLib/IncludeExtLibs.h"
#include"NetworkInformationLib/VivaldiInformation.h"
#include"NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{
// -------------------------------------------------------
//				VivaldiManagerInterface
// -------------------------------------------------------
class DLL_STATE VivaldiManagerInterface : public RmiInterface
{
public:
	VivaldiManagerInterface() { }
	~VivaldiManagerInterface() { }

	enum MethodID 
	{
		RmiSetVivaldiCoordinates = 11,
		RmiGetHostInformation = 12, 
		RmiGetNetworkInformation= 13
	};

	enum InterfaceId
	{
		VivaldiManagerInterfaceId = 57
	};

	virtual void SetVivaldiCoordinates(VivaldiInfo &) = 0;
	virtual HostInformation GetHostInformation(std::string componentName) = 0;
	virtual TreeStructure* GetNetworkInformation() = 0;

	virtual int GetInterfaceId() 			const { return VivaldiManagerInterface::VivaldiManagerInterfaceId; }
	virtual std::string GetInterfaceName() 	const { return RPC_FUNCTION_STRING; }
};

// -------------------------------------------------------
//				VivaldiManagerClient
// -------------------------------------------------------
class DLL_STATE VivaldiManagerClient : public RmiClient, public VivaldiManagerInterface
{
public:
	RMICLIENT_COMMON(VivaldiManagerClient)

	virtual void SetVivaldiCoordinates(VivaldiInfo &obj)
	{
		ExecuteOneWayRMI(VivaldiManagerInterface::RmiSetVivaldiCoordinates, &obj);
	}

	virtual HostInformation GetHostInformation(string componentName)
	{
		BeginMarshal(VivaldiManagerInterface::RmiGetHostInformation);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();
		
		BeginUnmarshal();
		HostInformation info;
		info.Read(GetSocketReader());
		EndUnmarshal();
		return info;
	}

	virtual TreeStructure* GetNetworkInformation()
	{
		BeginMarshal(VivaldiManagerInterface::RmiGetNetworkInformation);
		EndMarshal();
		
		BeginUnmarshal();
		TreeStructure *tree = new TreeStructure(); //info;
		//tree->Read(GetSocketReader());
		EndUnmarshal();
		return tree;
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

// -------------------------------------------------------
//				VivaldiManagerServer 
// -------------------------------------------------------
class DLL_STATE VivaldiManagerServer : public RmiServerListener
{
public:
	VivaldiManagerServer (const InterfaceHandle &handle, VivaldiManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(handle, autoStart, mode)
		, vivaldiManager_(serverInterface)
	{ }
	VivaldiManagerServer(const std::string &serviceName, const InterfaceHandle &handle, VivaldiManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(serviceName, handle, autoStart, mode)
		, vivaldiManager_(serverInterface)
	{ }

	virtual ~VivaldiManagerServer() {}

	virtual void RMInvocation(int methodId, RmiBase* rmiServer)
	{
		switch(methodId)
		{
			case VivaldiManagerInterface::RmiSetVivaldiCoordinates:
			{
				VivaldiInfo obj;
				rmiServer->ReceiveRMI(&obj);
				vivaldiManager_->SetVivaldiCoordinates(obj);
				break;
			}
			case VivaldiManagerInterface::RmiGetHostInformation:
			{
				string componentName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();

				HostInformation info = vivaldiManager_->GetHostInformation(componentName);
				rmiServer->ReplyRMI(&info);
				break;
			}
			case VivaldiManagerInterface::RmiGetNetworkInformation:
			{
				TreeStructure *tree = vivaldiManager_->GetNetworkInformation();
				ASSERT(0);
				//rmiServer->ReplyRMI(tree);
				
				//BeginMarshal(VivaldiManagerInterface::RmiGetNetworkInformation);
				//tree->Write(rmiServer->GetSocketReader());
				//EndMarshal();
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
	VivaldiManagerInterface *vivaldiManager_;
};

} // namespace end


#endif //





