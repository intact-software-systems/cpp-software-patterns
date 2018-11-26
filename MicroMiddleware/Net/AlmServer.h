#ifndef MicroMiddleware_AlmServer_h_Included
#define MicroMiddleware_AlmServer_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class RpcBase;

class DLL_STATE AlmServer 
{
public:
	AlmServer(std::string groupName = string("")) : groupName_(groupName) { }
	virtual ~AlmServer() { }

	virtual RmiObjectBase* RPCall(const RpcIdentifier &rpc, RpcBase *rpcBase) = 0;

	std::string GetGroupName()		{ return groupName_; }

protected:
	mutable Mutex rpcMutex_;
	std::string	groupName_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_AlmServer_h_Included


