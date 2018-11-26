#ifndef MicroMiddleware_RpcIdentifier_h_Included
#define MicroMiddleware_RpcIdentifier_h_Included

#include <stdlib.h>
#include <ostream>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/RmiObjectBase.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE RpcIdentifier : public RmiObjectBase
{
public:
	enum ConnectionId
	{
		NoConnectionId = 0,
		StubConnection,
		ProxyConnection, 
		SubscriberConnection,
		PublisherConnection
	};

public:
	RpcIdentifier() : methodId_(0), interfaceId_(0), hostEndian_(0), rpcSize_(0), connectionId_(RpcIdentifier::NoConnectionId), domainName_("default.domain") 
		{}
	RpcIdentifier(const short &methodId, const short &interfaceId, const short &hostEndian) 
		: methodId_(methodId)
		, interfaceId_(interfaceId)
		, hostEndian_(hostEndian)
		, rpcSize_(0)
		, connectionId_(RpcIdentifier::NoConnectionId)
		, domainName_("default.domain")
		{}
	RpcIdentifier(const short &methodId, const short &interfaceId, const short &hostEndian, const ConnectionId &id) 
		: methodId_(methodId)
		, interfaceId_(interfaceId)
		, hostEndian_(hostEndian)
		, rpcSize_(0)
		, connectionId_(id)
		, domainName_("default.domain")
		{}	
	RpcIdentifier(const short &methodId, const short &interfaceId, const short &hostEndian, const int &rpcSize, const ConnectionId &id) 
		: methodId_(methodId)
		, interfaceId_(interfaceId)
		, hostEndian_(hostEndian)
		, rpcSize_(rpcSize)
		, connectionId_(id)
		, domainName_("default.domain")
		{}
	RpcIdentifier(const short &methodId, const std::string &interfaceName, const short &hostEndian, const ConnectionId &id) 
		: methodId_(methodId)
		, interfaceId_(-1)
		, interfaceName_(interfaceName)
		, hostEndian_(hostEndian)
		, rpcSize_(0)
		, connectionId_(id)
		, domainName_("default.domain")
		{}	
	RpcIdentifier(const short &methodId, const std::string &interfaceName, const short &hostEndian, const int &rpcSize, const ConnectionId &id) 
		: methodId_(methodId)
		, interfaceId_(-1)
		, interfaceName_(interfaceName)
		, hostEndian_(hostEndian)
		, rpcSize_(rpcSize)
		, connectionId_(id)
		, domainName_("default.domain")
		{}		

    virtual ~RpcIdentifier() {}

	inline bool IsValid() const 
	{ 	
		if(methodId_ < 0) 
			return false; 

		if(interfaceName_.empty() == true && interfaceId_ < 0)
			return false; 

		if(interfaceName_.empty() == false && connectionId_ == RpcIdentifier::NoConnectionId)
			return false;
		
		return true; 
	}

	inline short 		GetHostEndian()		const { return hostEndian_; }
	inline short 		GetInterfaceId()	const { return interfaceId_; }
	inline std::string	GetInterfaceName() 	const { return interfaceName_; }
	inline short 		GetMethodId()		const { return methodId_; }
	inline int			GetRpcSize()		const { return rpcSize_; }
	inline ConnectionId GetConnectionId()	const { return connectionId_; }
	inline std::string	GetDomainName()		const { return domainName_; }
	
	inline void 		SetHostEndian(short e)			{ hostEndian_ = e; }
	inline void 		SetInterfaceId(short i)			{ interfaceId_ = i; }
	inline void			SetInterfaceName(std::string s) { interfaceName_ = s; }
	inline void 		SetMethodId(short m)			{ methodId_ = m; }
	inline void			SetRpcSize(int s)				{ rpcSize_ = s; }
	inline void			SetConnectionId(ConnectionId c)	{ connectionId_ = c; } 
	inline void			SetDomainName(std::string dom)	{ domainName_ = dom; }

public:
	virtual void Write(NetworkLib::SerializeWriter *writer)
	{
		writer->WriteInt16(hostEndian_);
		writer->WriteInt16(interfaceId_);
		writer->WriteString(interfaceName_);
		writer->WriteInt16(methodId_);
		writer->WriteInt32(rpcSize_);
		writer->WriteInt16((short)connectionId_);
		writer->WriteString(domainName_);
	}

	virtual void Read(NetworkLib::SerializeReader *reader)
	{
		hostEndian_ 	= reader->ReadInt16();
		interfaceId_ 	= reader->ReadInt16();
		interfaceName_  = reader->ReadString();
		methodId_ 		= reader->ReadInt16();
		rpcSize_		= reader->ReadInt32();
		connectionId_ 	= (ConnectionId)reader->ReadInt16();
		domainName_		= reader->ReadString();
	}

	bool operator==(const RpcIdentifier &other)
	{
		if(	methodId_ == other.GetMethodId() &&
			interfaceId_ == other.GetInterfaceId() &&
			interfaceName_ == other.GetInterfaceName() &&
			hostEndian_ == other.GetHostEndian() &&
			rpcSize_ == other.GetRpcSize() &&
			connectionId_ == other.GetConnectionId() &&
			domainName_ == other.GetDomainName())
			return true;

		return false;
	}

private:
	short 			methodId_;
	short 			interfaceId_;
	std::string		interfaceName_;
	short 			hostEndian_;
	int				rpcSize_;
	ConnectionId 	connectionId_;
	std::string		domainName_;
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const RpcIdentifier::ConnectionId &id);
DLL_STATE std::ostream& operator<<(std::ostream &ostr, const RpcIdentifier &rpc);


}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcIdentifier_h_Included

