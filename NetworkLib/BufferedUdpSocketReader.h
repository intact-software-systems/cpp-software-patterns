#ifndef NetworkLib_BufferedUdpSocketReader_IsIncluded
#define NetworkLib_BufferedUdpSocketReader_IsIncluded

#include"NetworkLib/IncludeExtLibs.h"
#include"NetworkLib/CommonDefines.h"
#include"NetworkLib/BufferedSocketReader.h"
#include"NetworkLib/NetAddress.h"
#include"NetworkLib/Serialize/InByteArray.h"
#include"NetworkLib/Serialize/SerializeBase.h"

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
// TODO: How to keep state and store partially received messages?
----------------------------------------------------------------------------- */
class DLL_STATE BufferedUdpSocketReader : public BufferedSocketReader
										, public BaseLib::Thread
{
public:
    typedef BaseLib::PCList<BufferedInMessage::Ptr>		PCListMessages;

public:
	BufferedUdpSocketReader(IOReader* stream, InternetProtocol::Type type = InternetProtocol::RTPS);
	virtual ~BufferedUdpSocketReader();

    CLASS_TRAITS(BufferedUdpSocketReader)

public:

    // ----------------------------------------------------------------------
	// DDS Reader communicates with RTPS/Cache using these functions
    // TODO: Return list of all messages available in Cache!
    // ----------------------------------------------------------------------

	virtual BufferedInMessage::Ptr PopMessage();		// == Take();
	virtual BufferedInMessage::Ptr Read();				// only reads does not pop/take

	virtual void Stop();								// == Delete();

private:
	virtual void run();

private:
	bool unmarshalDatagram(SerializeReader::Ptr &reader, NetAddress &address, const int &datagramSize);

	bool initializeReader(InternetProtocol::Type protocolType);

private:
	IOReader				*socketReader_;
	PCListMessages 			queueIncomingMessages_;
	InternetProtocol::Type	protocolType_;

private:
    BaseLib::Templates::ProtectedBool runThread_;
};

} // namespace NetworkLib

#endif // NetworkLib_BufferedUdpSocketReader_IsIncluded


