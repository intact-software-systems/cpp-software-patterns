#pragma once

#ifdef SSL_SUPPORT

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/Net/RmiBase.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{
class RmiServerListener;

class DLL_STATE RmiSslServer : public RmiBase, public Thread 
{
public:
	RmiSslServer(RmiServerListener *rmiSL, int socketDescriptor, bool autoStart = true);
	virtual ~RmiSslServer();

	virtual void run();

	virtual void InvokeRMI(int methodId);
	
	void StartOrRestartServer(const InterfaceHandle &);
	void StopServer(bool waitForTermination = true);
	
private:
	void startOrRestartServer();
	void initRmiSslServer();
	
	inline void lock()				{ serverMutex_.lock(); }
	inline void unlock()			{ serverMutex_.unlock(); }

protected:
	bool            		runThread_;
	InterfaceHandle 		interfaceHandle_;
	Mutex          			serverMutex_;
	SslSocket::Ptr			sslSocket_;
	int						socketDescriptor_;
	RmiServerListener       *rmiListener_;
};

}

#endif

