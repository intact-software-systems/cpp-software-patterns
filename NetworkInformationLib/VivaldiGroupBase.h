#ifndef NetworkInformationLib_VivaldiGroupBase_h_IsIncluded
#define NetworkInformationLib_VivaldiGroupBase_h_IsIncluded

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/VivaldiInformation.h"
#include "NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{

class DLL_STATE VivaldiGroupInterface : public RmiInterface
{
public:
	VivaldiGroupInterface() { }
	~VivaldiGroupInterface() { }

	enum RMIMethodId 
	{
		RmiSetInfoUpdate = 20		// VivaldiSpace::NEIGHBOR
	};

	enum InterfaceId
	{
		VivaldiGroupInterfaceId = 32
	};

	virtual void SetInfoUpdate(VivaldiInfo &) = 0;

	virtual int GetInterfaceId() const { return VivaldiGroupInterface::VivaldiGroupInterfaceId; }
	virtual std::string GetInterfaceName() const { return RPC_FUNCTION_STRING; }
};

class DLL_STATE VivaldiGroupClient : virtual public VivaldiGroupInterface
{
public:
	VivaldiGroupClient(RpcMulticastClient *rpcMulticast, const string &groupName) 
		: rpcMulticast_(rpcMulticast) 
		, groupName_(groupName)
		{}
	virtual ~VivaldiGroupClient() {}

	virtual void SetInfoUpdate(VivaldiInfo &obj)
	{
		RpcIdentifier rpc(VivaldiGroupInterface::RmiSetInfoUpdate, VivaldiGroupInterface::VivaldiGroupInterfaceId, 1);
		rpcMulticast_->ExecuteRpc(&rpc, &obj, groupName_);
	}

	inline RpcMulticastClient *GetRpcMulticastClient()		const { return rpcMulticast_; }

private:
	RpcMulticastClient *rpcMulticast_;
	string				groupName_;
};

/*class VivaldGroupProxy : public RmiClient, public VivaldiGroupInterface
{
public:
	RMICLIENT_COMMON(VivaldiClientProxy);

	virtual void SetInfoUpdate(VivaldiInfo &obj)
	{
		ExecuteOneWayRMI(VivaldiClientInterface::RmiSetInfoUpdate, &obj);
	}
};*/


class DLL_STATE VivaldiGroupServer : public RpcServer
{
public:
	VivaldiGroupServer(string serverName, VivaldiGroupInterface* serverInterface) 
		: RpcServer(serverName)
		, componentInterface(serverInterface)
	{ }
	virtual ~VivaldiGroupServer() 
	{ }

	virtual bool RPCall(const RpcIdentifier &rpc, RpcBase *rmiServer)
	{
		//IDEBUG() << " Received method request " << methodId << endl;
		switch(rpc.GetMethodId())
		{
			case VivaldiGroupInterface::RmiSetInfoUpdate:
			{
				VivaldiInfo vivaldiInfo;
				rmiServer->ReceiveRpcObject(&vivaldiInfo);
				componentInterface->SetInfoUpdate(vivaldiInfo);
				break;
			}
			default:
				IDEBUG() << " Not implemented ! " << rpc.GetMethodId();
				return false;
		}
		return true;
	}

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
	VivaldiGroupInterface *componentInterface;
};



} // namespace end

#endif




