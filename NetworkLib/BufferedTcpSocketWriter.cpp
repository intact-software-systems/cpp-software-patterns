#include"NetworkLib/BufferedTcpSocketWriter.h"
#include"NetworkLib/Serialize/ByteArray.h"

namespace NetworkLib
{
//----------------------------------------------------------
// constructor/destructor
// TODO: Check that socketWriter is in fact a TCP connection!
//----------------------------------------------------------
BufferedTcpSocketWriter::BufferedTcpSocketWriter(IOWriter *writer) 
		: socketWriter_(writer)
		, runThread_(true)
{ 
	start();
}

BufferedTcpSocketWriter::~BufferedTcpSocketWriter() 
{
	Stop();
}

//----------------------------------------------------------
// main thread
// TODO: add support for multiplexing byteArray to several host/port 
// TODO: Add feedback mechanism to application if message-writing fails!
//----------------------------------------------------------
void BufferedTcpSocketWriter::run()
{
	try
	{
		IDEBUG() << "Started!";
		
		while(true)
		{
            BufferedOutMessage::Ptr message = queueOutMessages_.consumer_pop_front();
			
			const NetAddress &addr = message->GetNetAddress();
			SerializeWriter::Ptr writer = message->GetSerializeWriter();
			
			// -- debug --
			ASSERT(addr.IsValid());
			ASSERT(writer);
			// -- debug --

			bool ret = Flush(writer, addr.GetHostName(), addr.GetPortNumber());
			if(ret == false)
			{
				ICRITICAL() << "failed to flush out message!";
				// TODO: Add feedback mechanism to application if message-writing fails!
			}
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caught" << ex.msg();
	}
}

//----------------------------------------------------------
// Buffered messages are added to output queue
// TODO: Check for queue size, statistics, etc.
//----------------------------------------------------------
bool BufferedTcpSocketWriter::PushMessage(BufferedOutMessage::Ptr &message)
{
	// TODO: Check if socketWriter_ is connected! 
	// Return false if not connected

	queueOutMessages_.producer_push_back(message);
	return true;
}

//----------------------------------------------------------
// Encapsulate with RTPS
// write out
// TODO: add support for multiplexing byteArray to several host/port 
//----------------------------------------------------------
bool BufferedTcpSocketWriter::Flush(SerializeWriter::Ptr &writer, const std::string &hostAddress, const int &port)
{
	// Encapsulate packet based on Message encapsulation from RTPS
	ByteArray msgWrapper;
	
	// encapsulate packet with endian info.
	/*RTPS::DataEncapsulation dataEncapsulation(OMG_CDR_BE);
	dataEncapsulation.Write(&msgWrapper);

	// message header
	RTPS::ProtocolId::Type protocolId(RTPS::ProtocolId::IntactProtocol);
	RTPS::ProtocolVersion protocolVersion('2', '1');
	RTPS::VendorId vendorId;
	RTPS::GuidPrefix guidPrefix;
	char submessageFlags = 0x00;

	RTPS::MessageHeader messageHeader(protocolId, protocolVersion, vendorId, guidPrefix);
	messageHeader.Write(&msgWrapper);

    //char id = 'T';
	RTPS::SubmessageHeader subHeader(RTPS::SubmessageKind::DATA, writer->GetLength(), submessageFlags);
	subHeader.Write(&msgWrapper);

	bool ret = msgWrapper.Flush(socketWriter_, hostAddress, port);
	if(!ret) 
	{
		ICRITICAL() << "Could not write to " << hostAddress << " " << port;
		return false;
	}

	return writer->Flush(socketWriter_, hostAddress, port);*/

    return true;
}

void BufferedTcpSocketWriter::Stop()
{
	runThread_ = false;
	if(socketWriter_)
	{	
		socketWriter_->Close();
		wait();
		socketWriter_ = NULL;
	}
}

} // namespace NetworkLib


