#ifndef IntactMiddleware_MembershipManagerBase_h_IsIncluded
#define IntactMiddleware_MembershipManagerBase_h_IsIncluded

#include <string>
#include"IntactMiddleware/IncludeExtLibs.h"
#include"IntactMiddleware/ServiceConnectionManager.h"
#include"IntactMiddleware/Export.h"

namespace IntactMiddleware
{
// -------------------------------------------------------
//				MembershipManager Interfaces
// -------------------------------------------------------
/**
* RMI functions to communicate with the MembershipManager.
*/
class DLL_STATE MembershipManagerInterface : public RmiInterface
{
public:
	MembershipManagerInterface() { }
	~MembershipManagerInterface() { }

	enum MethodId
	{
		Method_Login = 100,
		Method_Logout,
		Method_AliveChecker,
		Method_MembershipMap
	};

	enum InterfaceId
	{
		MembershipManagerInterfaceId = 100
	};

	virtual int Login(HostInformation &info) = 0;
	virtual int Logout(string componentName) = 0;
	virtual int AliveChecker(string componentName) = 0;
	virtual map<string, HostInformation> GetMembershipMap() = 0;

	virtual int GetInterfaceId() const { return MembershipManagerInterface::MembershipManagerInterfaceId; }
};


class DLL_STATE MembershipManagerClient : public RmiClient, public MembershipManagerInterface
{
public:
	RMICLIENT_COMMON(MembershipManagerClient);

	virtual int Login(HostInformation &info)
	{
		BeginMarshal(MembershipManagerInterface::Method_Login);
		info.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}
	
	virtual int Logout(string componentName)
	{
		BeginMarshal(MembershipManagerInterface::Method_Logout);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}

	virtual int AliveChecker(string componentName)
	{
		BeginMarshal(MembershipManagerInterface::Method_AliveChecker);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}


	virtual map<string, HostInformation> GetMembershipMap()
	{
		BeginMarshal(MembershipManagerInterface::Method_MembershipMap);
		EndMarshal();

		BeginUnmarshal();
			map<string, HostInformation> mapMembership;
			int size = GetSocketReader()->ReadInt32();
			for(int i = 0; i < size; i++)
			{
				string componentName = GetSocketReader()->ReadString();
				HostInformation info;
				info.Read(GetSocketReader());
				mapMembership[componentName] = info;
			}
		EndUnmarshal();

		return mapMembership;
	}

/*protected:
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
	}*/
};

#ifdef SSL_SUPPORT
class DLL_STATE MembershipManagerSslClient : public RmiSslClient, public MembershipManagerInterface
{
public:
	RMISSLCLIENT_COMMON(MembershipManagerSslClient);

	virtual int Login(HostInformation &info)
	{
		BeginMarshal(MembershipManagerInterface::Method_Login);
		info.Write(GetSocketWriter());
		EndMarshal();

		BeginUnmarshal();
		info.Read(GetSocketReader());
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}
	
	virtual int Logout(string componentName)
	{
		BeginMarshal(MembershipManagerInterface::Method_Logout);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}

	virtual int AliveChecker(string componentName)
	{
		BeginMarshal(MembershipManagerInterface::Method_AliveChecker);
		GetSocketWriter()->WriteString(componentName);
		EndMarshal();

		BeginUnmarshal();
		int ret = GetSocketReader()->ReadInt32();
		EndUnmarshal();
		return ret;
	}

	virtual map<string, HostInformation> GetMembershipMap()
	{
		BeginMarshal(MembershipManagerInterface::Method_MembershipMap);
		EndMarshal();

		BeginUnmarshal();
			map<string, HostInformation> mapMembership;
			int size = GetSocketReader()->ReadInt32();
			for(int i = 0; i < size; i++)
			{
				string componentName = GetSocketReader()->ReadString();
				HostInformation info;
				info.Read(GetSocketReader());
				mapMembership[componentName] = info;
			}
		EndUnmarshal();

		return mapMembership;
	}
};
#endif

// -------------------------------------------------------
//				MembershipManager Stub
//			- Receiver/consumer class
// -------------------------------------------------------
class DLL_STATE MembershipManagerServer : public RmiServerListener
{
public:
	MembershipManagerServer(const InterfaceHandle &handle, MembershipManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(handle, autoStart, mode)
		, membershipManager_(serverInterface)
	{ }
	MembershipManagerServer(const string &serviceName, const InterfaceHandle &handle, MembershipManagerInterface* serverInterface, bool autoStart, RmiBase::CommMode mode = RmiBase::UnencryptedMode)
		: RmiServerListener(serviceName, handle, autoStart, mode)
		, membershipManager_(serverInterface)
	{ }
	virtual ~MembershipManagerServer() {}

	virtual void RMInvocation(int methodId, RmiBase* rmiServer)
	{
		switch(methodId)
		{
			case MembershipManagerInterface::Method_Login:
			{
				HostInformation info;
				info.Read(rmiServer->GetSocketReader());
				rmiServer->EndUnmarshal();
				
				int ret = membershipManager_->Login(info);

				rmiServer->BeginMarshal(RmiInterface::ReturnValue);
				info.Write(rmiServer->GetSocketWriter());
				rmiServer->GetSocketWriter()->WriteInt32(ret);
				rmiServer->EndMarshal();
				break;
			}
			case MembershipManagerInterface::Method_Logout:
			{
				string componentName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();
				
				int ret = membershipManager_->Logout(componentName);

				rmiServer->BeginMarshal(RmiInterface::ReturnValue);
				rmiServer->GetSocketWriter()->WriteInt32(ret);
				rmiServer->EndMarshal();
				break;
			}			
			case MembershipManagerInterface::Method_AliveChecker:
			{
				string componentName = rmiServer->GetSocketReader()->ReadString();
				rmiServer->EndUnmarshal();
				
				int ret = membershipManager_->AliveChecker(componentName);

				rmiServer->BeginMarshal(RmiInterface::ReturnValue);
				rmiServer->GetSocketWriter()->WriteInt32(ret);
				rmiServer->EndMarshal();
				break;
			}
			case MembershipManagerInterface::Method_MembershipMap:
			{
				rmiServer->EndUnmarshal();

				map<string, HostInformation> mapMembers = membershipManager_->GetMembershipMap();

				rmiServer->BeginMarshal(MembershipManagerInterface::Method_MembershipMap);
					rmiServer->GetSocketWriter()->WriteInt32(mapMembers.size());
					for(map<string, HostInformation>::iterator it = mapMembers.begin(), it_end = mapMembers.end(); it != it_end; ++it)
					{
						rmiServer->GetSocketWriter()->WriteString(it->first);

						HostInformation &info = it->second;
						info.Write(rmiServer->GetSocketWriter());
					}
				rmiServer->EndMarshal();
				break;
			}
			default:
				IWARNING() << " RMI not implemented " << methodId;
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
	MembershipManagerInterface *membershipManager_;
};

} // namespace end


#endif //



