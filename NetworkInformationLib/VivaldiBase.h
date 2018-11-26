#ifndef NetworkInformationLib_VivaldiBase_h_IsIncluded
#define NetworkInformationLib_VivaldiBase_h_IsIncluded

#include "NetworkInformationLib/IncludeExtLibs.h"
#include "NetworkInformationLib/Export.h"

namespace NetworkInformationLib
{

class DLL_STATE VivaldiGroupInterface : public RmiInterface
{
public:
	VivaldiGroupInterface() { }
	~VivaldiGroupInterface() { }

	enum RMIMethodId {
		RMI_SetRotationMatrix = 21
	};

	enum InterfaceId
	{
		VivaldiGroupInterfaceId = 30
	};

	virtual void SetRotationMatrix(ViewState&) = 0;

	virtual int GetInterfaceId() { return VivaldiGroupInterface::VivaldiGroupInterfaceId; }
};

class DLL_STATE VivaldiGroupClient : virtual public VivaldiGroupInterface
{
public:
	VivaldiGroupClient(RpcMulticastClient *rpcMulticast, const string &groupName) 
		: rpcMulticast_(rpcMulticast) 
		, groupName_(groupName)
		{}
	virtual ~VivaldiGroupClient() {}

	virtual void SetRotationMatrix(ViewState &obj)
	{
		RpcIdentifier rpc(VivaldiGroupInterface::RMI_SetRotationMatrix, VivaldiGroupInterface::VivaldiGroupInterfaceId, 1);
		rpcMulticast_->ExecuteRpc(&rpc, &obj, groupName_);
	}

private:
	RpcMulticastClient *rpcMulticast_;
	string				groupName_;
};

class DLL_STATE VivaldiGroupServer : public RpcServer
{
public:
	VivaldiGroupServer(VivaldiGroupInterface* serverInterface) : componentInterface(serverInterface)
	{ }
	virtual ~VivaldiGroupServer() 
	{ }

	virtual bool RPCall(const RpcIdentifier &rpc, RpcBase *rmiServer)
	{
		//IDEBUG() << " Received method request " << methodId << endl;
		switch(rpc.GetMethodId())
		{
			case VivaldiGroupInterface::RMI_SetRotationMatrix:
			{
				ViewState viewState;
				rmiServer->ReceiveRpcObject(&viewState);
				componentInterface->SetRotationMatrix(viewState);
				break;
			}
			default:
				IDEBUG() << " Not implemented ! " << rpc.GetMethodId() << endl;
				return false;
		}
		return true;
	}
private:
	VivaldiGroupInterface *componentInterface;
};

} // end namespace

#endif



