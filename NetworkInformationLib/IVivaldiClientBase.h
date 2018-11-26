#ifndef NetworkInformationLib_IVivaldiClientBase_h_IsIncluded
#define NetworkInformationLib_IVivaldiClientBase_h_IsIncluded

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/VivaldiInformation.h"
#include "NetworkInformationLib/Export.h"
namespace NetworkInformationLib
{

// -------------------------------------------------------
//				VivaldiClientProxy
// -------------------------------------------------------
class DLL_STATE IVivaldiClientProxy : virtual public INetProxy, public VivaldiClientInterface
{
public:
	IVivaldiClientProxy(const string &serviceName, const std::string &connectedToName) 
		: INetProxy(serviceName, connectedToName, VivaldiClientInterface::GetInterfaceName())
	{}
	virtual ~IVivaldiClientProxy()
	{}

	virtual void SetGroupUpdate(string groupName)
	{
		SerializeWriter::Ptr writer = BeginMarshal(VivaldiClientInterface::RmiGroupUpdate);
		writer->WriteString(groupName);
		EndMarshal(writer);
	}

	/*virtual void SetConnection(HostInformation &info) 
	{
		//MutexLocker lock(&mutex_);

		cout << COMPONENT_FUNCTION << GetServiceName() << endl;
		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			info.SetComponentName(GetServiceName());
			service->RegisterService(this);
		}
		else
		{
			ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
			service->RemoveService(GetServiceName());
		}
	}*/
};

// -------------------------------------------------------
//				VivaldiClientServer 
// -------------------------------------------------------
class DLL_STATE IVivaldiClientStub : public INetStub
{
public:
	IVivaldiClientStub(const std::string &serviceName, VivaldiClientInterface* serverInterface)
		: INetStub(serviceName, serverInterface->GetInterfaceName())
		, vivaldiClient_(serverInterface)
	{ }
	virtual ~IVivaldiClientStub() {}

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &reader)
	{
		switch(rpc.GetMethodId())
		{
			case VivaldiClientInterface::RmiGroupUpdate:
			{
				string groupName = reader->ReadString();
				EndUnmarshal(reader);

				vivaldiClient_->SetGroupUpdate(groupName);
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
	VivaldiClientInterface *vivaldiClient_;
};

} // namespace end

#endif



