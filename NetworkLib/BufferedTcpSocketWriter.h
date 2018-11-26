#ifndef NetworkLib_BufferedTcpSocketWriter_h_IsIncluded
#define NetworkLib_BufferedTcpSocketWriter_h_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/Socket/IOBase.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/BufferedSocketWriter.h"
#include"NetworkLib/Export.h"
namespace NetworkLib
{
// One BufferedTcpSocketWriter can be used to multiplex byteArray to several host/port 
class DLL_STATE BufferedTcpSocketWriter : public BufferedSocketWriter
										, public BaseLib::Thread
{
public:
	BufferedTcpSocketWriter(IOWriter *writer);
	virtual ~BufferedTcpSocketWriter();
	
	virtual bool PushMessage(BufferedOutMessage::Ptr&);
	virtual void Stop();

	typedef BaseLib::PCList<BufferedOutMessage::Ptr>	PCListMessages;

private:
	virtual void run();

	bool Flush(SerializeWriter::Ptr &writer, const std::string &hostAddress, const int &port);
	// TODO: virtual bool Flush(IOWriter *writer, listOfHostsAndPorts);

private:
	IOWriter 		*socketWriter_;
	PCListMessages  queueOutMessages_;

private:
    BaseLib::Templates::ProtectedBool	runThread_;
};

} // namespace NetworkLib

#endif //


