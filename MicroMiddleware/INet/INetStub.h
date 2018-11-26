#ifndef MicroMiddleware_INetStub_h_Included
#define MicroMiddleware_INetStub_h_Included

#include"MicroMiddleware/IncludeExtLibs.h"
#include"MicroMiddleware/NetObjects/HostInformation.h"
#include"MicroMiddleware/NetObjects/RpcIdentifier.h"
#include"MicroMiddleware/INet/INetInterfaces.h"

//#include <deque>

#include"MicroMiddleware/Export.h"

namespace MicroMiddleware
{

class INetBase;
/*--------------------------------------------------------------
	class INetStub:
		- Does not create a TcpServer
		- Rather, it registers as a Stub in an existing INetConnection
		- An instance of INetStub registers to all connections from an INetServerListener
--------------------------------------------------------------*/
class DLL_STATE INetStub : public INetInterface 
						 , public BaseLib::Thread
{
public:
	typedef BaseLib::PCList<RpcCallWrapper>			PCListRpcCall;

public:
	INetStub(const std::string &serviceName
			, const std::string &interfaceName
			, HostInformation::HostDescription hostDescription
			, EndianConverter::Endian hostEndian = EndianConverter::ENDIAN_LITTLE
			, bool autoStart = true);
	INetStub(const std::string &serviceName, const std::string &interfaceName);

	virtual ~INetStub();

	virtual bool RPCall(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer) = 0; 
	
public:
	virtual void ReadCall(const RpcIdentifier &rpc, INetMessageParser::Ptr &base, SerializeReader::Ptr &buffer);
	
	virtual bool 		IsRunning() const;
	virtual void 		Stop();
	virtual void 		SetBase(INetMessageParser::Ptr &) { throw BaseLib::Exception("INetStub::SetBase(INetBase*): ERROR! Stub does not implement function!"); }
	virtual std::string GetServiceName() const;
	virtual std::string GetConnectedToName() const;
	
public:
	virtual bool 		ExecuteRpc(RpcIdentifier *rpc, RmiObjectBase *obj);

protected:
	virtual SerializeWriter::Ptr 	BeginMarshal(const short &methodId); 
	virtual bool 					EndMarshal(SerializeWriter::Ptr &);
	virtual bool 					EndMarshal(SerializeWriter::Ptr &, const ListNetAddress &listAddresses);
	
	virtual void					EndUnmarshal(SerializeReader::Ptr &);
		
	virtual void 					ReceiveRpcObject(RmiObjectBase *obj, SerializeReader::Ptr &);

protected:
	virtual void SetConnection(HostInformation &) { }

private:
	virtual SerializeReader::Ptr 	BeginUnmarshal()	{ throw BaseLib::Exception("INetStub::BeginUnmarshal(): BeginUnmarshal should not be called from INetStub!"); return SerializeReader::Ptr(); }

private:
	void ReadRpc(const RpcIdentifier &rpc, SerializeReader::Ptr &readBuffer); 

private:
	virtual void run();

protected:
	mutable Mutex 	mutex_;
	bool 			runThread_;

protected:
	PCListRpcCall			queueRpcCall_;
	INetMessageParser::Ptr  currentConnection_;		// Current connection

private:
	EndianConverter::Endian hostEndian_;
	std::string				serviceName_;
};

}; // namespace MicroMiddleware

#endif // MicroMiddleware_INetStub_h_Included



