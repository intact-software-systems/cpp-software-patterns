#if 0

#include"NetworkLib/BufferedUdpSocketWriter.h"
#include"NetworkLib/ByteArray.h"
#include"NetworkLib/EndianConverter.h"

namespace NetworkLib
{
//----------------------------------------------------------
// constructor/destructor
// TODO: Check that socketWriter is in fact a UDP connection!
//----------------------------------------------------------
BufferedUdpSocketWriter::BufferedUdpSocketWriter(IOWriter *writer, InternetProtocol::Type type)
	: socketWriter_(writer)
	, runThread_(true)
	, protocolType_(type)
{ 
	start();
}

BufferedUdpSocketWriter::BufferedUdpSocketWriter(IOWriter *writer,
												RTPS::WriterKind::Type writerType,
												const RTPS::GUID &guid,
												RTPS::TopicKindId::Type topicKind,
												RTPS::ReliabilityKindId::Type reliabilityKind)
	: socketWriter_(writer)
	, runThread_(true)
	, protocolType_(InternetProtocol::RTPS)
{
	initializeRtpsWriter(writerType, guid, topicKind, reliabilityKind);
}


BufferedUdpSocketWriter::~BufferedUdpSocketWriter() 
{
	Stop();
}

bool BufferedUdpSocketWriter::initializeRtpsWriter(RTPS::WriterKind::Type writerType, const RTPS::GUID &guid, RTPS::TopicKindId::Type topicKind, RTPS::ReliabilityKindId::Type reliabilityKind)
{
	RTPS::RtpsWriter *rtpsWriter = RTPS::RtpsFactory::NewDefaultRtpsWriter(writerType, guid, topicKind, reliabilityKind);
	ASSERT(rtpsWriter);

    vmRtpsWriter_ = RTPS::VirtualMachineWriter::Ptr( new RTPS::VirtualMachineWriter(rtpsWriter, socketWriter_));
	if(vmRtpsWriter_->isRunning() == false)	vmRtpsWriter_->start();

	return true;
}

//----------------------------------------------------------
// main thread
// TODO: add support for multiplexing byteArray to several host/port 
// TODO: Add feedback mechanism to application if message-writing fails!
//
// The RTPS Writer sends the contents of the CacheChange changes to the RTPS Reader using the Data Submessage
// and requests an acknowledgment by also sending a Heartbeat Submessage.
//----------------------------------------------------------
void BufferedUdpSocketWriter::run()
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

			bool ret = this->flush(writer, addr.GetHostName(), addr.GetPortNumber());
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
// TODO: Check if socketWriter_ is connected!
// Return false if not connected
//----------------------------------------------------------
bool BufferedUdpSocketWriter::PushMessage(BufferedOutMessage::Ptr &message)
{
	/*------------------------------------------------------------
	If using RTPS then add message.Data to RTPSWriter.dataCache:

	The transition performs the following logical actions in the virtual machine:

	the_rtps_writer := the_dds_writer.related_rtps_writer;
	a_change := the_rtps_writer.new_change(ALIVE, data, handle);
	the_rtps_writer.writer_cache.add_change(a_change);

	After the transition the following post-conditions hold:

	ASSERT(the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber)

	IF NOT using RTPS then
	--> other simpler implementations?
	------------------------------------------------------------*/

	switch(this->protocolType_)
	{
		case InternetProtocol::RTPS:
        {
            ASSERT(vmRtpsWriter_);
            ASSERT(vmRtpsWriter_->GetWriter());

			// -- initialize RtpsData --
			RTPS::RtpsData::Ptr rtpsData = RTPS::RtpsData::Ptr( new RTPS::RtpsData(RTPS::SerializedPayload(message->GetSerializeWriter())) );

            // -- add RtpsData to DataCache --
            InstanceHandle handle(0);
            ICRITICAL() << "Handle is 0!";

            RTPS::CacheChange::Ptr cacheChange = vmRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::ALIVE, rtpsData, handle);

			// 	After the transition the following post-conditions hold:
			ASSERT(vmRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());
			break;
		}
		case InternetProtocol::RTP:
			IWARNING() << "InternetProtocol::RTP is not implemented!";
		case InternetProtocol::RPC:
			// TODO: Use RTPS as example of how to implement RPC
			//      - Call an RPC writer that wraps multiple TCP sockets into one thread!
		default:
			queueOutMessages_.producer_push_back(message);
			break;
	}

	return true;
}
/*------------------------------------------------------------
This transition is triggered by the act of disposing a data-object previously written with the DDS DataWriter
"the_dds_writer". The DataWriter::dispose() operation takes as parameter the InstanceHandle_t "handle" used to
differentiate among different data-objects.

This operation has no effect if the topicKind==NO_KEY.
The transition performs the following logical actions in the virtual machine:

the_rtps_writer := the_dds_writer.related_rtps_writer;
if (the_rtps_writer.topicKind == WITH_KEY) {
	a_change := the_rtps_writer.new_change(NOT_ALIVE_DISPOSED, <nil>, handle);
	the_rtps_writer.writer_cache.add_change(a_change);
}

After the transition the following post-conditions hold:
if (the_rtps_writer.topicKind == WITH_KEY) then
	the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber
------------------------------------------------------------*/
bool BufferedUdpSocketWriter::Dispose(BufferedOutMessage::Ptr&)
{
	switch(this->protocolType_)
	{
		case InternetProtocol::RTPS:
        {
            ASSERT(vmRtpsWriter_);
            ASSERT(vmRtpsWriter_->GetWriter());

			if(vmRtpsWriter_->GetWriter()->GetTopicKind() == RTPS::TopicKindId::WithKey)
			{
                InstanceHandle handle(0);
                ICRITICAL() << "Handle is 0!";

				RTPS::CacheChange::Ptr cacheChange = vmRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::NOT_ALIVE_DISPOSED, RTPS::RtpsData::Ptr(), handle);

				// 	After the transition the following post-conditions hold:
				ASSERT(vmRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());
			}
			break;
		}
		case InternetProtocol::RTP:
			IWARNING() << "InternetProtocol::RTP is not implemented!";
		case InternetProtocol::RPC:
		default:
			break;
	}

	return false;
}

/*------------------------------------------------------------
This transition is triggered by the act of unregistering a data-object previously written with the DDS DataWriter
"the_dds_writer". The DataWriter::unregister() operation takes as arguments the InstanceHandle_t "handle" used to
differentiate among different data-objects.
This operation has no effect if the topicKind==NO_KEY.
The transition performs the following logical actions in the virtual machine:

the_rtps_writer := the_dds_writer.related_rtps_writer;

if (the_rtps_writer.topicKind == WITH_KEY) {
	a_change := the_rtps_writer.new_change(NOT_ALIVE_UNREGISTERED, <nil>, handle);
	the_rtps_writer.writer_cache.add_change(a_change);
}

After the transition the following post-conditions hold:

if (the_rtps_writer.topicKind == WITH_KEY) then
	the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber
------------------------------------------------------------*/
bool BufferedUdpSocketWriter::Unregister(BufferedOutMessage::Ptr&)
{
	switch(this->protocolType_)
	{
		case InternetProtocol::RTPS:
		{
            ASSERT(vmRtpsWriter_);
            ASSERT(vmRtpsWriter_->GetWriter());

			if(vmRtpsWriter_->GetWriter()->GetTopicKind() == RTPS::TopicKindId::WithKey)
			{
                InstanceHandle handle(0);
                ICRITICAL() << "Handle is 0!";

				RTPS::CacheChange::Ptr cacheChange = vmRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::NOT_ALIVE_UNREGISTERED, RTPS::RtpsData::Ptr(), handle);

				// 	After the transition the following post-conditions hold:
				ASSERT(vmRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());
			}
			break;
		}
		case InternetProtocol::RTP:
			IWARNING() << "InternetProtocol::RTP is not implemented!";
		case InternetProtocol::RPC:
		default:
			break;
	}

	return false;
}

//----------------------------------------------------------
// Encapsulate with RTPS
// write out
// TODO: add support for multiplexing byteArray to several host/port 
// TODO: Pattern is marshalDatagram then call function Flush possibly many times with different hostAddress/port
//----------------------------------------------------------
bool BufferedUdpSocketWriter::flush(SerializeWriter::Ptr &writer, const std::string &hostAddress, const int &port)
{
	// Encapsulate packet based on Message encapsulation from RTPS
	ByteArray msgWrapper(512);
	
	// encapsulate packet with endian info.
	RTPS::DataEncapsulation dataEncapsulation(OMG_CDR_BE);
	dataEncapsulation.Write(&msgWrapper);

	// message header
	RTPS::ProtocolId::Type protocolId(RTPS::ProtocolId::IntactProtocol);
	RTPS::ProtocolVersion protocolVersion('2', '1');
	RTPS::VendorId vendorId;
	RTPS::GuidPrefix guidPrefix;
	//char submessageFlags = 0x00;

	RTPS::MessageHeader messageHeader(protocolId, protocolVersion, vendorId, guidPrefix);
	messageHeader.Write(&msgWrapper);

	// TODO: prepend msgWrapper to byteArray
	ASSERT(writer->IsBuffered());
	msgWrapper.Write(writer->GetData(), writer->GetLength());
	//msgWrapper += writer;
	
	// TODO: fragmentation is done here!
	// if size > 512 (max: 65527) then fragment
	
	return msgWrapper.Flush(socketWriter_, hostAddress, port);
}

//----------------------------------------------------------
// Add subscriber
//----------------------------------------------------------
bool BufferedUdpSocketWriter::AddSubscriber(const NetAddress &address)
{
	// TODO: Implement

	// Different behavior for RTPS and RPC
	return true;
}

//----------------------------------------------------------
// Remove subscriber
//----------------------------------------------------------
bool BufferedUdpSocketWriter::RemoveSubscriber(const NetAddress &address)
{
	// TODO: Implement

	// Different behavior for RTPS and RPC
	return true;
}

//----------------------------------------------------------
// Stop thread and TODO: cleanup
//----------------------------------------------------------
void BufferedUdpSocketWriter::Stop()
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

#endif
