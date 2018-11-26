#ifndef NetworkLib_BufferedTcpSocketReader_IsIncluded
#define NetworkLib_BufferedTcpSocketReader_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/BufferedSocketReader.h"
#include"NetworkLib/Serialize/SerializeBase.h"
#include"NetworkLib/NetAddress.h"
#include"NetworkLib/Serialize/InByteArray.h"

#include"NetworkLib/Export.h"
namespace NetworkLib 
{
class IOReader;

/* -----------------------------------------------------------------------------
SerializeReader?:
 - Read stream from Buffered SocketReader?
 - Demultiplex packets based on Message encapsulation scheme from RTPS
 - Read sequence numbers and match packets together
 - In case of seq. number missing -> Request resend
 - Verify correct format of packets
 - Upon successful verification enter into output buffer 
----------------------------------------------------------------------------- */
class DLL_STATE BufferedTcpSocketReader : public BufferedSocketReader
										, public BaseLib::Thread
{
public:
	BufferedTcpSocketReader(IOReader* stream);
	virtual ~BufferedTcpSocketReader();

	typedef BaseLib::PCList<BufferedInMessage::Ptr>		PCListMessages;
	
	// TODO: How to keep state and store partially received messages?
	virtual BufferedInMessage::Ptr PopMessage();
	virtual void Stop();

private:
	virtual void run();

private:
	bool unmarshalMessage(SerializeReader::Ptr &reader, NetAddress &address);

private:
	IOReader		*socketReader_;
	PCListMessages  queueIncomingMessages_;

private:
    BaseLib::Templates::ProtectedBool runThread_;
};

} // namespace NetworkLib

#endif // NetworkLib_BufferedTcpSocketReader_IsIncluded


