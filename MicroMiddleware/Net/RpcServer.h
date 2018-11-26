#ifndef MicroMiddleware_RpcServer_h_Included
#define MicroMiddleware_RpcServer_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware {

class RpcBase;

class DLL_STATE RpcServer
{
public:
    RpcServer(string serverName = string("")) : serverName_(serverName)
    {}

    virtual ~RpcServer()
    {}

    virtual bool RPCall(const RpcIdentifier& rpc, RpcBase* rpcBase) = 0;

    HostInformation GetServerInformation() const
    {
        MutexLocker lock(&mutex_);
        return serverInformation_;
    }

    void SetServerInformation(HostInformation serverInfo)
    {
        if(serverInformation_.IsValid())
        {
            IDEBUG() << "WARNING! Already valid server information " << serverInformation_;
        }
        serverInformation_ = serverInfo;

        if(!serverName_.empty())
        {
            serverInformation_.SetComponentName(serverName_);
        }
        else
        {
            serverInformation_.SetComponentName(serverInfo.GetComponentName());
        }
    }

    virtual void ServerStartedListening()
    {}

    virtual void ServerStoppedListening()
    {}

protected:
    mutable Mutex mutex_;
    HostInformation serverInformation_;
    string serverName_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RpcServer_h_Included



