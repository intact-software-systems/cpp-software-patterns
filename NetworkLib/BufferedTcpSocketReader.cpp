#include"NetworkLib/BufferedTcpSocketReader.h"
#include"NetworkLib/Socket/SocketStream.h"
#include"NetworkLib/Serialize/SocketReader.h"

using namespace std;

namespace NetworkLib
{

BufferedTcpSocketReader::BufferedTcpSocketReader(IOReader* stream)
	: BaseLib::Thread()
	, socketReader_(stream)
	, runThread_(true)
{
	start();
}

BufferedTcpSocketReader::~BufferedTcpSocketReader()
{
	Stop();
}

/* -----------------------------------------------------------------------------
SerializeReader?:
 - Read stream from Buffered SocketReader?
 - Demultiplex packets based on Message encapsulation scheme from RTPS
 - Read sequence numbers and match packets together
 - In case of seq. number missing -> Request resend
 - Verify correct format of packets
 - Upon successful verification enter into output buffer 

	TODO: How do I know the length of the next datagram?
		Proposal: Always send the message header in a seperate datagram? No!
		Proposal: Send a fixed size datagram. For example: 512 bytes.
	TODO: Check Endian and message-length
	TODO: Add feedback mechanism to application if message-reading fails!
----------------------------------------------------------------------------- */
void BufferedTcpSocketReader::run()
{
	try
	{
		IDEBUG() << "Started!";
		
		while(runThread_)
		{
			SerializeReader::Ptr reader;
			NetAddress address;
			
			bool success = unmarshalMessage(reader, address);
			if(runThread_ == false) break;

			// -- debug --
			ASSERT(success);
			ASSERT(reader);
			ASSERT(address.IsValid());
			// -- debug --

			BufferedInMessage::Ptr outMsg = BufferedInMessage::Ptr(new BufferedInMessage(address, reader));
			queueIncomingMessages_.producer_push_back(outMsg);
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caught" << ex.msg();
	}
}

bool BufferedTcpSocketReader::unmarshalMessage(SerializeReader::Ptr &reader, NetAddress &address)
{
	/*SocketReader socketReader(socketReader_);

	RTPS::DataEncapsulation dataEncapsulation;
	dataEncapsulation.Read(&socketReader);

	RTPS::MessageHeader messageHeader;
	messageHeader.Read(&socketReader);
	
	RTPS::SubmessageHeader subHeader;
	subHeader.Read(&socketReader);

	// TODO: print out all data
	//IDEBUG()  << dataEncapsulation << " " << messageHeader << " " << subHeader << endl;
	ASSERT(subHeader.GetSubmessageLength() > 0);

	// read submessage
	std::string hostAddress;
	int portNumber = 0;

	std::vector<char> inBuffer;
	inBuffer.resize(subHeader.GetSubmessageLength());

	int ret = socketReader_->ReadDatagram(&inBuffer[0], subHeader.GetSubmessageLength(), hostAddress, portNumber);

	// -- debug --
	if(ret != subHeader.GetSubmessageLength())
	{
		IWARNING() << "Failed to receive datagram on socket!" << ret << " != " << subHeader.GetSubmessageLength();
		// TODO: return false or throw exception?
	}
	IDEBUG() << "Received message from " << hostAddress << " " << portNumber;
	ASSERT(ret == subHeader.GetSubmessageLength());
	// -- debug --
	
	reader = SerializeReader::Ptr(new InByteArray(&inBuffer[0], subHeader.GetSubmessageLength()));
	address = NetAddress(hostAddress, portNumber);
*/
	return true;
}

BufferedInMessage::Ptr BufferedTcpSocketReader::PopMessage()
{
    BufferedInMessage::Ptr bufferPtr = queueIncomingMessages_.consumer_pop_front();
	if(!bufferPtr)
		ICRITICAL() << "BufferedTcpSocketReader::PopMessage(): returned NULL! Lost connection?";
	
	return bufferPtr;
}

void BufferedTcpSocketReader::Stop()
{
	runThread_ = false;
	if(socketReader_)
	{	
		socketReader_->Close();
		wait();
		socketReader_ = NULL;
	}
}

} // namespace NetworkLib

