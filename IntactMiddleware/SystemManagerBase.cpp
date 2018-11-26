#include"IntactMiddleware/SystemManagerBase.h"

namespace IntactMiddleware
{



// -------------------------------------------------------
//	SystemManager connection used by ComponentFactory
// -------------------------------------------------------
/**
* Disconnects the host from the SystemManager.
*/
SystemManagerFactoryConnection::~SystemManagerFactoryConnection()
{
    myHostInformation.SetOnlineStatus(HostInformation::OFFLINE);
    systemManagerProxy->HostRegistration(myHostInformation);
    cerr << "Disconnected SystemManager (" << systemManagerHandle.GetHostName() << "," << systemManagerHandle.GetPortNumber() << ")" << endl;

    if(systemManagerProxy) delete systemManagerProxy;
    systemManagerProxy = NULL;
}

} // namespace IntactMiddleware
