#pragma once

#ifdef SSL_SUPPORT

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
#define RMISSLCLIENT_COMMON(RmiSslClientClass)																									\
    static RmiSslClientClass* GetRmiSslClient(const MicroMiddleware::InterfaceHandle& handle,			\
                                      MicroMiddleware::SetRmiSslClient* setRmiSslClient = 0)	\
	{																							\
		MicroMiddleware::RmiSslClient* rmiClient = FindProxy(handle, setRmiSslClient);				\
        if(!rmiClient)																			\
		{																						\
            rmiClient =  new RmiSslClientClass(handle);											\
            if(setRmiSslClient)																	\
                setRmiSslClient->insert(rmiClient);												\
        }																						\
		return dynamic_cast<RmiSslClientClass*>(rmiClient);										\
	}																							\
	RmiSslClientClass(const MicroMiddleware::InterfaceHandle &handle, bool autoStart = false)		\
			: RmiSslClient(handle, autoStart) 														\
	{} 																							\
	RmiSslClientClass(const int &socketDescriptor, bool autoStart = false)							\
			: RmiSslClient(socketDescriptor, autoStart) 											\
	{} 																							\
	virtual ~RmiSslClientClass() {}																\


class RmiSslClient;
typedef set<RmiSslClient*>	SetRmiSslClient;

class DLL_STATE RmiSslClient : public RmiBase, protected Thread
{
public:
	RmiSslClient(const InterfaceHandle &interfaceHandle, bool autoStart = true);
	RmiSslClient(int socketDescriptor, bool autoStart = true);
	virtual ~RmiSslClient()
	{}

	static RmiSslClient* FindProxy(const InterfaceHandle &handle, const SetRmiSslClient *setRmiSslClient);

	void StopClient(bool waitForTermination = true);
	void StartClient();
	
	void StartOrRestartClient(const InterfaceHandle &handle);
	void StartOrRestartClient();
 
	bool ConnectToHost();

	bool WaitConnected()
	{
		try
		{
			lock();
			while(IsConnected() == false)
			{
				//IDEBUG() << " Client is still not connected!" << endl;
				rmiMonitor_.wait(&clientMutex_, 1000);
			}
		}
		catch(...)
		{
			IDEBUG() << " ERROR while waiting for connection on " << interfaceHandle_ << endl;
		}
		
		unlock();
		return IsConnected();
	}

	virtual void AddTarget(InterfaceHandle &handle);
	virtual void RemoveTarget(InterfaceHandle &handle);

public:
	inline InterfaceHandle 	GetInterfaceHandle()		{ return interfaceHandle_; }
	inline bool 			IsRunning()					{ return isRunning(); }
	inline SetRmiSslClient*	GetRmiSslClientCollection()	{ return &setRmiSslClient_; }
	
	inline void lock()				{ clientMutex_.lock(); }
	inline void unlock()			{ clientMutex_.unlock(); }

	virtual void Initialize()				{ }

protected:
	virtual void run();

private:
	bool connectToHost();
	void initSslSocket();

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	Mutex             	clientMutex_;
	SslSocket::Ptr		sslSocket_;
	int					socketDescriptor_;

	SetRmiSslClient		setRmiSslClient_;
};

}

#endif


