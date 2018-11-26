#ifndef MicroMiddleware_INetProxy_h_Included
#define MicroMiddleware_INetProxy_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/INet/INetInterfaces.h"

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{
class INetBase;
class RmiObjectBase;

/*--------------------------------------------------------------
	class INetProxy:
		- Does not connect to a remote host
		- Rather, it uses an INetConnection/INetBase to serialize an RPC call
		- Currently, an instance of INetProxy can use only ONE INetConnection
		- Hence, an instance of INetProxy must be created for each INetConnection 

	TODO:
		- Consider Thread. Thread can automatically detect if INetConnection is down.
		- QosPolicy -> SetQosPolicy()
		- QosPolicy negotiation between stub and proxy?
		- I think QoSPolicy is more applicable in a publisher/subscriber scenario.
		- QosPolicy includes all DDS related policies
		- How should I include QoS related to groups? 
			- Diameter, eccentricity, degree, dynamicity, bandwidth, group-size, etc

	TODO:
		- Implement BUFFERED/UNBUFFERED options in INetConnection
		- INetProxy should probably be unaware of BUFFERED/UNBUFFERED!
--------------------------------------------------------------*/

class DLL_STATE INetProxy : public INetInterface
							, public BaseLib::Thread
{
public:
	typedef BaseLib::PCList<RpcCallWrapper>			PCListRpcCall;

public:
	INetProxy(const std::string &serviceName
			, const std::string &connectedToName
			, const std::string &interfaceName
			, HostInformation::HostDescription hostDescription
			, EndianConverter::Endian hostEndian = EndianConverter::ENDIAN_LITTLE);
	
	INetProxy(const std::string &serviceName, const std::string &connectedToName, const std::string &interfaceName);
	
	virtual ~INetProxy();

public:
	bool				IsConnected() const;
	
public:
	virtual bool 		IsRunning() const;

    virtual void 		ReadCall(const RpcIdentifier &rpc, INetMessageParser::Ptr &base, SerializeReader::Ptr &bufferReader);
	
	virtual void 		Stop();
	virtual void 		SetBase(INetMessageParser::Ptr &base);	// SetBase is called when Proxy registers in INetConnection

	virtual std::string GetServiceName() const;
	virtual std::string GetConnectedToName() const;

public:
	virtual bool 		ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj);

protected:
	virtual SerializeWriter::Ptr 	BeginMarshal(const short &methodId); 
	virtual bool 					EndMarshal(SerializeWriter::Ptr &);
	virtual bool 					EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses);
	
	virtual SerializeReader::Ptr 	BeginUnmarshal();
	virtual void					EndUnmarshal(SerializeReader::Ptr &);
		
	virtual void 					ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &);

protected:
	virtual void SetConnection(HostInformation &) { }

private:
	virtual void run();

protected:
	mutable Mutex 		mutex_;
	bool				runThread_;
	
protected:	
	PCListRpcCall		queueRpcCall_;
	PCListRpcCall		queueFilteredRpcCalls_;
	bool 				waitRpc_;
	
protected:
	INetMessageParser::Ptr 	inetConn_;

private:
	EndianConverter::Endian hostEndian_;
	std::string				serviceName_;
	std::string				connectedToName_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetProxy_h_Included




