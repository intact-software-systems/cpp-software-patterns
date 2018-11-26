#ifndef NetworkInformationLib_IVivaldiGroupBase_h_IsIncluded
#define NetworkInformationLib_IVivaldiGroupBase_h_IsIncluded

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/VivaldiInformation.h"
#include "NetworkInformationLib/VivaldiGroupBase.h"
#include "NetworkInformationLib/Export.h"
namespace NetworkInformationLib
{

class DLL_STATE IVivaldiGroupProxy : virtual public VivaldiGroupInterface
{
public:
	IVivaldiGroupProxy(INetMulticastProxy *multicastProxy, const string &groupName) 
		: multicastProxy_(multicastProxy)
		, groupName_(groupName)
	{
		ASSERT(multicastProxy_);
		ASSERT(groupName_.empty() == false);
	}
	virtual ~IVivaldiGroupProxy() 
	{ }

	virtual void SetInfoUpdate(VivaldiInfo &obj)
	{
		RpcIdentifier rpc(VivaldiGroupInterface::RmiSetInfoUpdate, GetInterfaceName(), EndianConverter::ENDIAN_LITTLE, RpcIdentifier::SubscriberConnection);
		multicastProxy_->ExecuteRpc(&rpc, &obj, groupName_);
	}

	/*virtual void SetConnection(HostInformation &info) 
	{
		MutexLocker lock(&mutex_);

		ServiceConnectionManager *service = ServiceConnectionManager::GetOrCreate();
		if(info.GetOnlineStatus() == HostInformation::ONLINE)
		{
			service->RegisterService(info);
		}
		else
		{
			ASSERT(info.GetOnlineStatus() == HostInformation::OFFLINE);
			service->RemoveService(GetServiceName());
		}
	}*/

private:
	INetMulticastProxy  *multicastProxy_;
	string				groupName_;
	mutable Mutex		mutex_;
};

class DLL_STATE IVivaldiGroupStub : public INetStub
{
public:
	IVivaldiGroupStub(std::string serverName, VivaldiGroupInterface* serverInterface) 
		: INetStub(serverName, serverInterface->GetInterfaceName())
		, componentInterface(serverInterface)
	{ }
	virtual ~IVivaldiGroupStub() 
	{ }

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &reader)
	{
		cerr << COMPONENT_FUNCTION << rpc << endl;
		switch(rpc.GetMethodId())
		{
			case VivaldiGroupInterface::RmiSetInfoUpdate:
			{
				VivaldiInfo vivaldiInfo;
				ReceiveRpcObject(&vivaldiInfo, reader);
				EndUnmarshal(reader);

				componentInterface->SetInfoUpdate(vivaldiInfo);
				break;
			}
			default:
				IDEBUG() << " Not implemented ! " << rpc.GetMethodId();
				return false;
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
	VivaldiGroupInterface *componentInterface;
};



} // namespace end

#endif




