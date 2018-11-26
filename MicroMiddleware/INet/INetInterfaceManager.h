#ifndef MicroMiddleware_INetInterfaceManager_h_IsIncluded
#define MicroMiddleware_INetInterfaceManager_h_IsIncluded

#include <map>
#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/INet/INetInterfaces.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{
/*-----------------------------------------------------------------------
            class INetInterfaceManager 
			- Check QoS  (See tick #93)

- Add middleware-brokered QoS contracts between publishers (who offer a maximum level for each QoS policy) 
  and subscribers (who request a minimum level for each QoS policy).
- QoS contracts provide the performance predictability and resource control required by real-time and 
  embedded systems while preserving the modularity, scalability and robustness inherent to the anonymous 
  publish/subscribe model.

- DDS standard QoS policies: DEADLINE, TIME_BASED_FILTER, CONTENT_BASED_FILTER, HISTORY, RELIABILITY, 
	USER_DATA, TOPIC_DATA, GROUP_DATA, DURABILITY, PRESENTATION, LATENCY_BUDGET, OWNERSHIP, 
	OWNERSHIP_STRENGTH, LIVELINESS, PARTITION, TRANSPORT_PRIORITY, LIFESPAN, DESTINATION_ORDER 
	and RESOURCE_LIMITS
------------------------------------------------------------------------- */
class DLL_STATE INetInterfaceManager : public Thread
{
public:
	typedef map<string, INetInterface*>  	MapIdInterface;

private:
	INetInterfaceManager(const std::string &name = std::string("MicroMiddleware.Manager.Interface"));
	~INetInterfaceManager();
	
private:
	virtual void run();

public:
	static INetInterfaceManager* GetOrCreate();

public:
	// called from outside
	bool Register(INetInterface*);
	bool Remove(INetInterface*);

//public:
	// Callbacks from Proxy/Stub
	// Handle incidents:
	// INetConnection is down! 
	// INetServer is down!

private:
	mutable Mutex	mutex_;
	WaitCondition   waitCondition_;
	bool			runThread_;

private:
	static INetInterfaceManager	*interfaceManager_;
};

}; // namespace MicroMiddleware

#endif


