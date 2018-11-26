#ifndef MicroMiddleware_RmiInterface_h_Included
#define MicroMiddleware_RmiInterface_h_Included

#include <string>

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class DLL_STATE RmiInterface
{
public:
	RmiInterface() { }
	virtual ~RmiInterface() { }

	enum RMIMethodId
	{
		ReturnValue = 1,
		ReturnException,
		ReleaseInterface,
		AddTarget,
		RemoveTarget,
		StubDisconnected,
		ProxyDisconnected,
		SubscriberDisconnected,
		PublisherDisconnected,
		KeepAlive
	};

	enum RMIInterfaceId
	{
		ConnectionStateUpdate = 1,
		MulticastGroupUpdate
	};

	virtual int GetInterfaceId() const = 0;

	virtual std::string GetInterfaceName() const;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_RmiInterface_h_Included


