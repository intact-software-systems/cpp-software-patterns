#ifndef MicroMiddleware_INetInterfaces_h_IsIncluded
#define MicroMiddleware_INetInterfaces_h_IsIncluded

#include<memory>
#include<list>

#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/NetObjects/InterfaceHandle.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/Export.h"

/*--------------------------------------------------------------
    forward declarations
--------------------------------------------------------------*/
namespace NetworkLib
{
    class SerializeReader;
    class SerializeWriter;
}

namespace MicroMiddleware
{
class ServerListenerInterface;
class ConnectionInterface;
class INetInterface;

/*--------------------------------------------------------------
    class INetInterfaceIdentifier
--------------------------------------------------------------*/
class DLL_STATE INetInterfaceIdentifier
{
public:
    INetInterfaceIdentifier()
        : connectionId_(RpcIdentifier::NoConnectionId)
        , domainName_("default.domain")
        {}
    INetInterfaceIdentifier(std::string name, RpcIdentifier::ConnectionId id, std::string domainName = string("default.domain"))
        : interfaceName_(name)
        , connectionId_(id)
        , domainName_(domainName)
        {}
    INetInterfaceIdentifier(const RpcIdentifier &id)
        : interfaceName_(id.GetInterfaceName())
        , connectionId_(id.GetConnectionId())
        , domainName_(id.GetDomainName())
        {}
    ~INetInterfaceIdentifier() {}

    bool IsValid() const
    {
        if(interfaceName_.empty()) return false;
        if(connectionId_ == RpcIdentifier::NoConnectionId) return false;
        if(domainName_.empty()) return false;

        return true;
    }

public:
    // -- inline set/get functions --
    inline std::string 					GetInterfaceName()	const { return interfaceName_; }
    inline RpcIdentifier::ConnectionId 	GetConnectionId()	const { return connectionId_; }
    inline std::string 					GetDomainName()		const { return domainName_; }

    void SetInterfaceName(const std::string &name)			{ interfaceName_ = name; }
    void SetConnectionId(RpcIdentifier::ConnectionId id)	{ connectionId_ = id; }
    void SetDomainName(const std::string &domain)			{ domainName_ = domain; }

public:
    // -- operators --
    bool operator==(const INetInterfaceIdentifier &other) const
    {
        if(	connectionId_ == other.GetConnectionId() &&
            interfaceName_ == other.GetInterfaceName() &&
            domainName_ == other.GetDomainName())
            return true;

        return false;
    }

    bool operator==(const RpcIdentifier &other) const
    {
        if(	connectionId_ == other.GetConnectionId() &&
            interfaceName_ == other.GetInterfaceName() &&
            domainName_ == other.GetDomainName())
            return true;

        return false;
    }

    bool operator<(const INetInterfaceIdentifier &other) const
    {
        return connectionId_< other.GetConnectionId() ||
               (connectionId_ == other.GetConnectionId() && interfaceName_.length() < other.GetInterfaceName().length()) ||
               (connectionId_ == other.GetConnectionId() && interfaceName_.length() == other.GetInterfaceName().length() && domainName_.length() < other.GetDomainName().length());
    }

    friend std::ostream& operator<<(std::ostream &ostr, const INetInterfaceIdentifier &other)
    {
        ostr << "(" << other.GetInterfaceName() << "," << other.GetConnectionId() << "," << other.GetDomainName() << ")";
        return ostr;
    }

private:
    std::string					interfaceName_;
    RpcIdentifier::ConnectionId	connectionId_;
    std::string					domainName_;
};

/*--------------------------------------------------------------
    class ConnectionManagerInterface
--------------------------------------------------------------*/

class ConnectionManagerInterface
{
public:
    ConnectionManagerInterface() {}
    virtual ~ConnectionManagerInterface() {}

    friend class ServerListenerInterface;
    friend class ConnectionInterface;

public:
    virtual ConnectionInterface* 	AddConnection(int socketDescriptor, ServerListenerInterface*) = 0;
    virtual void 					AddConnection(int socketDescriptor, ConnectionInterface*) = 0;

    virtual void 					RemoveConnection(int socketDescriptor, ConnectionInterface*) = 0;
    virtual void 					RemoveConnection(ConnectionInterface *conn) = 0;

    virtual void 					RemoveServerListener(int socketDescriptor, ServerListenerInterface*) = 0;
};

/*--------------------------------------------------------------
    class ServerListenerInterface
--------------------------------------------------------------*/

class ServerListenerInterface
{
public:
    ServerListenerInterface(const InterfaceHandle &handle)
        : interfaceHandle_(handle)
    {}
    virtual ~ServerListenerInterface()
    {}

public:
    virtual bool Start(bool waitConnected) = 0;
    virtual void Stop(bool waitStopped) = 0;
    virtual int  socketDescriptor() = 0;

    virtual bool RegisterInterface(INetInterface*) = 0;
    virtual bool RemoveInterface(INetInterface*) = 0;

public:
    InterfaceHandle GetInterfaceHandle() const { return interfaceHandle_; }
    HostInformation GetServerInformation() const { return serverInformation_; }

protected:
    InterfaceHandle interfaceHandle_;
    HostInformation serverInformation_;
};

/*--------------------------------------------------------------
    class ConnectionInterface
--------------------------------------------------------------*/

class ConnectionInterface
{
public:
    enum ConnectionType
    {
        TcpServer = 0,
        TcpClient,
        UdpSender,
        UdpReceiver,
        MulticastSender,
        MulticastReceiver
    };

public:
    ConnectionInterface(const InterfaceHandle &handle, ConnectionType type)
        : interfaceHandle_(handle)
        , connectionType_(type)
    {}
    virtual ~ConnectionInterface()
    {}

    virtual void Stop(bool waitForTermination = true) = 0;

    virtual bool RegisterInterface(INetInterface*) = 0;
    virtual bool RemoveInterface(INetInterface*) = 0;

    virtual bool IsConnected() = 0;
    virtual bool KeepAlive() = 0;

    InterfaceHandle GetInterfaceHandle() 		const { return interfaceHandle_; }
    HostInformation GetConnectionInformation() 	const { return connectionInformation_; }
    ConnectionType	GetConnectionType()  		const { return connectionType_; }

protected:
    InterfaceHandle interfaceHandle_;
    HostInformation connectionInformation_;
    ConnectionType	connectionType_;
};

/*--------------------------------------------------------------
    class MessageParser
    - Use this as input to INetInterface objects
    - Marshals and Unmarshals objects
TODO: RpcIdentifier* should not be a pointer but rather RpcIdentifier &
TODO: RmiObjectBase* should not be a pointer but rather RmiObjectBase &
--------------------------------------------------------------*/
class INetMessageParser
{
public:
    typedef std::list<NetworkLib::NetAddress>			ListNetAddress;

public:
    INetMessageParser()
    {}
    virtual ~INetMessageParser()
    {}

    friend class INetConnection;

    CLASS_TRAITS(INetMessageParser)

    virtual void InitSocketObjects(AbstractSocket::Ptr) = 0;

public:
    virtual bool IsConnected() const = 0;
    virtual bool IsBound() const = 0;
    virtual bool IsBufferedRead() const = 0;
    virtual bool IsBufferedWrite() const = 0;

    virtual bool DoKeepAlive() = 0;

    virtual bool ExecuteRpc(RpcIdentifier *, RmiObjectBase *obj = NULL) = 0;
    virtual bool ReceiveRpcObject(RmiObjectBase *, SerializeReader::Ptr &) = 0;

    virtual bool BeginMarshal(RpcIdentifier *, SerializeWriter::Ptr&) = 0;
    virtual bool EndMarshal(SerializeWriter::Ptr &) = 0;
    virtual bool EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses) = 0;

    virtual void EndUnmarshal(SerializeReader::Ptr &) = 0;

protected:
    virtual bool BeginUnmarshal(RpcIdentifier *, SerializeReader::Ptr&) = 0;
    virtual void Close() = 0;

    virtual void ReadLock() {}
    virtual void ReadUnlock() {}
};

/*--------------------------------------------------------------
    class INetInterface
TODO: Is serializeType necessary? It should be oblivious of that, or?
TODO: RpcIdentifier* should not be a pointer but rather RpcIdentifier &
TODO: RmiObjectBase* should not be a pointer but rather RmiObjectBase &
--------------------------------------------------------------*/
class INetInterface
{
public:
    enum SerializeType
    {
        BufferedType = 0,
        UnbufferedType
    };

    typedef std::list<NetworkLib::NetAddress>			ListNetAddress;

public:
    INetInterface(const std::string &interfaceName, HostInformation::HostDescription interfaceType, SerializeType type = INetInterface::UnbufferedType)
        : interfaceName_(interfaceName)
        , interfaceType_(interfaceType)
        , serializeType_(type)
    {
        initInterfaceIdentifier(interfaceType);
    }
    virtual ~INetInterface()
    { }

    CLASS_TRAITS(INetInterface)

private:
    void initInterfaceIdentifier(HostInformation::HostDescription interfaceType)
    {
        RpcIdentifier::ConnectionId id = RpcIdentifier::NoConnectionId;
        switch(interfaceType)
        {
            case HostInformation::PROXY:
            {
                id = RpcIdentifier::ProxyConnection;
                break;
            }
            case HostInformation::STUB:
            {
                id = RpcIdentifier::StubConnection;
                break;
            }
            case HostInformation::PUBLISHER:
            {
                id = RpcIdentifier::PublisherConnection;
                break;
            }
            case HostInformation::SUBSCRIBER:
            {
                id = RpcIdentifier::SubscriberConnection;
                break;
            }
            default:
                iFatal() << PRETTY_FUNCTION << "Could not recognize interfaceType!";
                break;
        }

        interfaceIdentifier_ = INetInterfaceIdentifier(interfaceName_, id);
        ASSERT(interfaceIdentifier_.IsValid());
    }

public:
    virtual void ReadCall(const RpcIdentifier &rpc, INetMessageParser::Ptr &base, SerializeReader::Ptr &buffer) = 0;

    virtual void SetBase(INetMessageParser::Ptr &base) = 0;
    virtual void Stop() = 0;
    virtual void SetConnection(HostInformation &serverInfo) = 0;

    virtual bool IsRunning() const = 0;

    virtual std::string	GetServiceName() const = 0;
    virtual std::string GetConnectedToName() const = 0;

public:
    bool IsValid() const
    {
        return interfaceIdentifier_.IsValid();
    }

public:
    inline std::string 						GetInterfaceName()			const 	{ return interfaceName_; }
    inline SerializeType					GetSerializeType()			const	{ return serializeType_; }
    inline HostInformation::HostDescription	GetInterfaceType()  		const 	{ return interfaceType_; }
    inline INetInterfaceIdentifier			GetInterfaceIdentifier()	const	{ return interfaceIdentifier_; }

public:
    virtual bool 					ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj) = 0;

protected:
    virtual SerializeWriter::Ptr 	BeginMarshal(const short &methodId) = 0;
    virtual bool 					EndMarshal(SerializeWriter::Ptr &) = 0;
    virtual bool 					EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses) = 0;

    virtual SerializeReader::Ptr 	BeginUnmarshal() = 0;
    virtual void					EndUnmarshal(SerializeReader::Ptr&) = 0;

    virtual void 					ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr&) = 0;

private:
    std::string 						interfaceName_;
    HostInformation::HostDescription	interfaceType_;
    SerializeType 						serializeType_;
    INetInterfaceIdentifier				interfaceIdentifier_;
};

/*--------------------------------------------------------------
    class INetMarshal
--------------------------------------------------------------*/
/*class INetMarshal
{
private:
    INetMarshal()
    {}
    virtual ~INetMarshal()
    {}

public:
    virtual bool 				ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj) = 0;

protected:
    virtual SerializeWriter* 	BeginMarshal(const short &methodId) = 0;
    virtual bool 				EndMarshal() = 0;

    virtual SerializeWriter* 	GetSocketWriter() const = 0;
};*/

/*--------------------------------------------------------------
    class INetUnmarshal
--------------------------------------------------------------*/
/*class INetUnmarshal
{
private:
    INetUnmarshal()
    {}
    virtual ~INetUnmarshal()
    {}

protected:
    virtual SerializeReader* 	BeginUnmarshal() = 0;
    virtual void				EndUnmarshal() = 0;

    virtual void 				ReceiveRpcObject(RmiObjectBase *obj) = 0;

    virtual SerializeReader* 	GetSocketReader() const = 0;
};*/


/*--------------------------------------------------------------
    class RpcCallWrapper
--------------------------------------------------------------*/
class RpcCallWrapper
{
public:
    RpcCallWrapper(const RpcIdentifier &rpc, INetMessageParser::Ptr &conn, SerializeReader::Ptr &buf)
        : rpcId_(rpc)
        , inetConnection_(conn)
        , readBuffer_(buf)
    {}
    RpcCallWrapper(const RpcIdentifier &rpc, INetMessageParser::Ptr &conn)
        : rpcId_(rpc)
        , inetConnection_(conn)
    {}
    RpcCallWrapper()
    {}
    virtual ~RpcCallWrapper()
    {}

    INetMessageParser::Ptr	GetConnection() const { return inetConnection_; }
    SerializeReader::Ptr	GetReadBuffer() const { return readBuffer_; }
    RpcIdentifier			GetRpcId()		const { return rpcId_; }

private:
    RpcIdentifier 			rpcId_;
    INetMessageParser::Ptr	inetConnection_;
    SerializeReader::Ptr 	readBuffer_;
};


} // namespace MicroMiddleware

#endif

