#ifndef MicroMiddleware_RmiMulticast_h_Included
#define MicroMiddleware_RmiMulticast_h_Included

#include <list>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiClient.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class DLL_STATE RmiMulticast
{
public:
	RmiMulticast() 
	{ }
	virtual ~RmiMulticast()
	{ }

	typedef list<RmiClient*> ListRmiClient;
	
	void StopAll(bool waitForTermination = true);
	void StartAll();
	void RestartAll();
	
	RmiClient* AddClient(const InterfaceHandle &handle);
	RmiClient* StartAndAddClient(const InterfaceHandle &handle);
	
	RmiClient* RemoveClient(const InterfaceHandle &handle);
	RmiClient* StopAndRemoveClient(const InterfaceHandle &handle);

	ListRmiClient& GetRmiClients() 	{ return listRmiClients_;}
	
	virtual RmiClient* newRmiClient(const InterfaceHandle &) = 0;

public:	
	inline void lock()				{ clientMutex_.lock(); }
	inline void unlock()			{ clientMutex_.unlock(); }

	virtual void Initialize()				{ }

protected:
	RmiClient* 	findRmiClient(const InterfaceHandle &interfaceHandle);

protected:
	Mutex             	clientMutex_;
	ListRmiClient		listRmiClients_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_RmiMulticast_h_Included



