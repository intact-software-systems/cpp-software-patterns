#ifndef MicroMiddleware_INetServerListener_h_Included
#define MicroMiddleware_INetServerListener_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/INet/INetInterfaces.h"

#include"MicroMiddleware/Export.h"
namespace MicroMiddleware
{
class ConnectionManagerInterface;

// TODO: Does it make sense to register stubs here?	- I guess stubs should be associated with a ServerListener?
// TODO: How do I make sure that a stub is added to all associated INetConnections?
//      -> Store all spawned INetConnections here?
class DLL_STATE INetServerListener : public ServerListenerInterface, public Thread
{
public:
	typedef std::map<INetInterfaceIdentifier, INetInterface*>	MapInterfaceStub;

public:
	INetServerListener(const InterfaceHandle &interfaceHandle
						, ConnectionManagerInterface *conn
						, bool autoStart = true);
	virtual ~INetServerListener();
	
	virtual bool Start(bool waitConnected = true);
	virtual void Stop(bool waitStopped = true);
	virtual int  socketDescriptor();
	
	virtual bool RegisterInterface(INetInterface*);
	virtual bool RemoveInterface(INetInterface*);
	
private:
	virtual void run();
	bool initINetServerListener();

protected:
	bool              				runThread_;
	mutable Mutex           		mutex_;
	WaitCondition					waitCondition_;

protected:
	int								socketDescriptor_;
	TcpServer						*tcpServer_;
	ConnectionManagerInterface		*connectionManager_;
	MapInterfaceStub				mapInterfaceStub_;

private:
	std::string						serverName_;
};

} // namespace MicroMiddleware

#endif // MicroMiddleware_INetServerListener_h_Included




