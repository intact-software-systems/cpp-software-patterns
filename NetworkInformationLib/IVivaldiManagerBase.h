#ifndef NetworkInformationLib_IVivaldiManagerBase_h_IsIncluded
#define NetworkInformationLib_IVivaldiManagerBase_h_IsIncluded

#include <string>
#include"NetworkInformationLib/IncludeExtLibs.h"
#include"NetworkInformationLib/VivaldiInformation.h"
#include"NetworkInformationLib/VivaldiManagerBase.h"
#include"NetworkInformationLib/Export.h"
using namespace std;

namespace NetworkInformationLib
{
// -------------------------------------------------------
//				VivaldiManagerProxy
// TODO: Use InterfaceNames (string) instead of InterfaceId (int)
// -------------------------------------------------------
class DLL_STATE IVivaldiManagerProxy : public INetProxy, public VivaldiManagerInterface
{
public:
	IVivaldiManagerProxy(const string &serviceName, const std::string &connectedToName) : INetProxy(serviceName, connectedToName, VivaldiManagerInterface::GetInterfaceName()) 
	{}
	~IVivaldiManagerProxy()
	{}

	virtual void SetVivaldiCoordinates(VivaldiInfo &obj)
	{
		RpcIdentifier rpc(VivaldiManagerInterface::RmiSetVivaldiCoordinates, INetProxy::GetInterfaceName(), EndianConverter::ENDIAN_LITTLE, RpcIdentifier::StubConnection);
		ExecuteRpc(&rpc, &obj);
	}

	virtual HostInformation GetHostInformation(string componentName)
	{
		SerializeWriter::Ptr writer = BeginMarshal(VivaldiManagerInterface::RmiGetHostInformation);
		writer->WriteString(componentName);
		EndMarshal(writer);
		
		SerializeReader::Ptr reader = BeginUnmarshal();
		HostInformation info;
		info.Read(reader.get());
		EndUnmarshal(reader);
		return info;
	}

	virtual TreeStructure* GetNetworkInformation()
	{
		SerializeWriter::Ptr writer = BeginMarshal(VivaldiManagerInterface::RmiGetNetworkInformation);
		EndMarshal(writer);
		
		SerializeReader::Ptr reader = BeginUnmarshal();
		TreeStructure *tree = new TreeStructure(); //info;
		//tree->Read(GetSocketReader());
		EndUnmarshal(reader);
		return tree;
	}

	/*virtual void SetConnection(HostInformation &info) 
	{
		//MutexLocker lock(&mutex_);

		cout << COMPONENT_FUNCTION << GetServiceName() << endl;

		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			info.SetComponentName(GetServiceName());
			service->RegisterService(info);
		}
		else
		{
			ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
			service->RemoveService(GetServiceName());
		}
	}*/
};

// -------------------------------------------------------
//				IVivaldiManagerStub
// -------------------------------------------------------
class DLL_STATE IVivaldiManagerStub : public INetStub
{
public:
	IVivaldiManagerStub(const std::string &serviceName, VivaldiManagerInterface* serverInterface)
		: INetStub(serviceName, serverInterface->GetInterfaceName())
		, vivaldiManager_(serverInterface)
	{ }
	virtual ~IVivaldiManagerStub() {}

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &reader)
	{
		switch(rpc.GetMethodId())
		{
			case VivaldiManagerInterface::RmiSetVivaldiCoordinates:
			{
				VivaldiInfo obj;
				ReceiveRpcObject(&obj, reader);
				EndUnmarshal(reader);
				
				vivaldiManager_->SetVivaldiCoordinates(obj);
				break;
			}

			case VivaldiManagerInterface::RmiGetHostInformation:
			{
				string componentName = reader->ReadString();
				EndUnmarshal(reader);
				
				HostInformation info = vivaldiManager_->GetHostInformation(componentName);
				
				RpcIdentifier replyRpc = rpc;
				ExecuteRpc(&replyRpc, &info);
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
				IWARNING()  << " RMI not implemented " << rpc.GetMethodId();
				return false;
				break;
		}

		return true;
	}


	/*virtual void SetConnection(HostInformation &info) 
	{
		//MutexLocker lock(&mutex_);

		cout << COMPONENT_FUNCTION << GetServiceName() << endl;
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			info.SetComponentName(GetServiceName());
			service->RegisterService(info);
		}
		else
		{
			ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
			service->RemoveService(GetServiceName());
		}
	}*/

private:
	VivaldiManagerInterface *vivaldiManager_;
};

} // namespace ---

#endif //

