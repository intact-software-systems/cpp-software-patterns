#ifndef MicroMiddleware_RmiClient_h_Included
#define MicroMiddleware_RmiClient_h_Included

#include <set>

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"

#define RMICLIENT_COMMON(RmiClientClass)																									\
    static RmiClientClass* GetRmiClient(const MicroMiddleware::InterfaceHandle& handle,			\
                                      MicroMiddleware::SetRmiClient* setRmiClient = 0)	\
	{																							\
		MicroMiddleware::RmiClient* rmiClient = FindProxy(handle, setRmiClient);				\
        if(!rmiClient)																			\
		{																						\
            rmiClient =  new RmiClientClass(handle);											\
            if(setRmiClient)																	\
                setRmiClient->insert(rmiClient);												\
        }																						\
		return dynamic_cast<RmiClientClass*>(rmiClient);										\
	}																							\
	RmiClientClass(const MicroMiddleware::InterfaceHandle &handle, bool autoStart = true)		\
			: RmiClient(handle, autoStart) 														\
	{} 																							\
	RmiClientClass(const std::string &clientName, const MicroMiddleware::InterfaceHandle &handle, bool autoStart = true) \
			: RmiClient(clientName, handle, autoStart) 											\
	{} 																							\
	RmiClientClass(const int &socketDescriptor, bool autoStart = true)							\
			: RmiClient(socketDescriptor, autoStart) 											\
	{} 																							\
	virtual ~RmiClientClass() {}																\


#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{

class RmiClient;
typedef set<RmiClient*>	SetRmiClient;

class DLL_STATE RmiClient : public RmiBase, protected Thread
{
public:
	RmiClient(const InterfaceHandle &interfaceHandle, bool autoStart = true);
	RmiClient(const std::string &clientName, const InterfaceHandle &interfaceHandle, bool autoStart = true);
	RmiClient(int socketDescriptor, bool autoStart = true);
	virtual ~RmiClient();

	static RmiClient* FindProxy(const InterfaceHandle &handle, const SetRmiClient *setRmiClient);

	void StopClient(bool waitForTermination = true);
	void StartClient();
	
	void StartOrRestartClient(const InterfaceHandle &handle);
	void StartOrRestartClient();
 
	bool ConnectToHost();
	bool WaitConnected();

	virtual void AddTarget(InterfaceHandle &handle);
	virtual void RemoveTarget(InterfaceHandle &handle);

public:
	InterfaceHandle 	GetInterfaceHandle()	const { MutexLocker lock(&mutex_); return interfaceHandle_; }
	HostInformation 	GetClientInformation()	const { MutexLocker lock(&mutex_); return clientInformation_; }
	string				GetClientName()			const { MutexLocker lock(&mutex_); return clientName_; }
	
	inline bool 			IsRunning()					{ return isRunning(); }
	inline SetRmiClient*	GetRmiClientCollection()	{ return &setRmiClient_; }
	
	virtual void Initialize()				{ }

protected:
	virtual void run();

private: //protected:
	// reimplement these in Stub and register with ServiceManager!!!
	virtual void SetConnection(HostInformation &hostInfo) 
	{ 
		IDEBUG() << hostInfo.GetComponentName() << " is " << hostInfo.GetOnlineStatus() ;
	}

private:
	bool connectToHost();
	bool initTcpSocket();

protected:
	bool              	runThread_;
	InterfaceHandle		interfaceHandle_;
	std::string			clientName_;
	HostInformation		clientInformation_;

	mutable Mutex       mutex_;
	TcpSocket::Ptr		tcpSocket_;
	int					socketDescriptor_;

	SetRmiClient		setRmiClient_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_RmiClient_h_Included


