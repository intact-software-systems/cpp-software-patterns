#ifndef NetworkLib_BufferedUdpSocketWriter_h_IsIncluded
#define NetworkLib_BufferedUdpSocketWriter_h_IsIncluded

#if 0
#include <vector>
#include <cstdarg>
#include <iostream>

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/IOBase.h"
#include"NetworkLib/SerializeBase.h"
#include"NetworkLib/BufferedSocketWriter.h"
#include"NetworkLib/Export.h"

namespace NetworkLib
{
// TODO: ByteArray that supports BIG endianness
// One BufferedUdpSocketWriter can be used to multiplex byteArray to several host/port 
class DLL_STATE BufferedUdpSocketWriter : public BufferedSocketWriter
										, public BaseLib::Thread
{
public:
	typedef BaseLib::PCList<BufferedOutMessage::Ptr>	PCListMessages;

public:
	BufferedUdpSocketWriter(IOWriter *writer, InternetProtocol::Type type = InternetProtocol::RPC);
	BufferedUdpSocketWriter(IOWriter *writer, RTPS::WriterKind::Type writerType, const RTPS::GUID &guid, RTPS::TopicKindId::Type topicKind, RTPS::ReliabilityKindId::Type reliabilityKind);
	virtual ~BufferedUdpSocketWriter();
	
	virtual void Stop();

public:
	// The DDS writer communicates with RTPS using these functions
	virtual bool PushMessage(BufferedOutMessage::Ptr&);
	virtual bool Dispose(BufferedOutMessage::Ptr&);
	virtual bool Unregister(BufferedOutMessage::Ptr&);

public:
	virtual bool AddSubscriber(const NetAddress &address);
	virtual bool RemoveSubscriber(const NetAddress &address);

private:
	virtual void run();

	bool flush(SerializeWriter::Ptr &writer, const std::string &hostAddress, const int &port);
	// TODO: 
	// virtual bool Flush(IOWriter *writer, listOfHostsAndPorts);

	bool initializeRtpsWriter(RTPS::WriterKind::Type writerType, const RTPS::GUID &guid, RTPS::TopicKindId::Type topicKind, RTPS::ReliabilityKindId::Type reliabilityKind);

private:
	IOWriter 		*socketWriter_;
	PCListMessages  queueOutMessages_;

private:
	RTPS::VirtualMachineWriter::Ptr	vmRtpsWriter_;

private:
    BaseLib::Templates::ProtectedBool	runThread_;
	InternetProtocol::Type 	protocolType_;
};

} // namespace NetworkLib

#endif

#endif //


