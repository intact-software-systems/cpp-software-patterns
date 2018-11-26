#include"NetworkLib/BufferedUdpSocketReader.h"
#include"NetworkLib/Socket/SocketStream.h"
#include"NetworkLib/Serialize/SocketReader.h"

using namespace std;

namespace NetworkLib
{

BufferedUdpSocketReader::BufferedUdpSocketReader(IOReader* stream, InternetProtocol::Type protocolType)
	: BaseLib::Thread()
	, socketReader_(stream)
	, protocolType_(protocolType)
	, runThread_(true)
{
	start();
}

BufferedUdpSocketReader::~BufferedUdpSocketReader()
{
	Stop();
}

bool BufferedUdpSocketReader::initializeReader(InternetProtocol::Type type)
{
	switch(type)
	{
		case InternetProtocol::RTPS:
		{
			// TODO: Initialize objects used for RTPS reader
			// TODO: use RtpsFactory!!!
			break;
		}
		case InternetProtocol::RPC:
		{
			// TODO: Nothing to do now?
			break;
		}
		case InternetProtocol::RTP:
			IWARNING() << "InternetProtocol::RTP is not implemented!";
		default:
			break;
	}

	return true;
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
	TODO: Add feedback mechanism to application if message-reading fails!
	TODO: Instead of using char buf[512] use std::vector<char> with size 512.

The RTPS Writer sends the contents of the CacheChange changes to the RTPS Reader using the Data Submessage
and requests an acknowledgment by also sending a Heartbeat Submessage.
----------------------------------------------------------------------------- */
void BufferedUdpSocketReader::run()
{
	try
	{
		IDEBUG() << "Started!";
		int datagramSize = 512;
		
		// call initialize first
		initializeReader(this->protocolType_);

		while(runThread_)
		{
			SerializeReader::Ptr reader;
			NetAddress address;

			bool success = unmarshalDatagram(reader, address, datagramSize);
			if(runThread_ == false) break;
			
			// -- debug --
			ASSERT(success);
			ASSERT(reader);
			ASSERT(address.IsValid());
			// -- debug --

			switch(this->protocolType_)
			{
				case InternetProtocol::RTPS:
				{
					// TODO: use RTPS virtual machine
					// rtpsMachine_->ReceiveDatagram(reader, address);
					break;
				}
				case InternetProtocol::RPC:
				{
					BufferedInMessage::Ptr outMsg = BufferedInMessage::Ptr(new BufferedInMessage(address, reader));
					queueIncomingMessages_.producer_push_back(outMsg);
					break;
				}
				case InternetProtocol::RTP:
					IWARNING() << "InternetProtocol::RTP is not implemented!";
				default:
				{
					BufferedInMessage::Ptr outMsg = BufferedInMessage::Ptr(new BufferedInMessage(address, reader));
					queueIncomingMessages_.producer_push_back(outMsg);
					break;
				}
			}
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caught" << ex.msg();
	}
}

//-------------------------------------------------------------------------------------------
// Do I have the address?
// Parse message and validate content
// Is it a fragment?
// If yes then store
// if no then pass to output-queue
// encapsulate packet with endian info.
//-------------------------------------------------------------------------------------------
bool BufferedUdpSocketReader::unmarshalDatagram(SerializeReader::Ptr &reader, NetAddress &address, const int &datagramSize)
{
	int 				portNumber = 0;
	std::string 		hostAddress;
	std::vector<char> 	inBuffer(datagramSize, 0x00);

	int bytesRead = socketReader_->ReadDatagram(&inBuffer[0], datagramSize, hostAddress, portNumber);

	// -- debug --
	if(bytesRead != datagramSize)
	{
		ICRITICAL() << "Failed to receive datagram on socket!" << bytesRead << " != " << datagramSize;
	}
	IDEBUG() << "Received message from " << hostAddress << " " << portNumber;
	ASSERT(bytesRead == datagramSize);
	// -- debug --

	reader = SerializeReader::Ptr(new InByteArray(&inBuffer[0], inBuffer.size()));

	address.SetHostName(hostAddress);
	address.SetPortNumber(portNumber);

	return true;
}

/*------------------------------------------------------------------------------------
This transition is triggered by the act of reading data from the DDS DataReader ‘the_dds_reader’ by means of the ‘take’
operation. Changes returned to the application are removed from the RTPS Reader’s HistoryCache such that subsequent
read or take operations do not find the same change.
The transition performs the following logical actions in the virtual machine:

the_rtps_reader := the_dds_reader.related_rtps_reader;
a_change_list := new();
FOREACH change IN the_rtps_reader.reader_cache.changes
{
	if DDS_FILTER(the_rtps_reader, change)
	{
		ADD change TO a_change_list;
	}
	the_rtps_reader.reader_cache.remove_change(a_change);
}
RETURN a_change_list;

The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on
CacheChange::kind, QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the
behavior and not necessarily the actual implementation of the protocol.

After the transition the following post-conditions hold:

FOREACH change IN a_change_list
	change BELONGS_TO the_rtps_reader.reader_cache.changes == FALSE
------------------------------------------------------------------------------------*/
BufferedInMessage::Ptr BufferedUdpSocketReader::PopMessage()	// == Take()
{
	// IF RTPS then take from rtpsReader.readerCache

    return queueIncomingMessages_.consumer_pop_front();
}

/*------------------------------------------------------------------------------------
This transition is triggered by the act of reading data from the DDS DataReader "the_dds_reader" by means of the "read"
operation. Changes returned to the application remain in the RTPS Reader’s HistoryCache such that subsequent read or
take operations can find them again.
The transition performs the following logical actions in the virtual machine:

the_rtps_reader := the_dds_reader.related_rtps_reader;
a_change_list := new();

FOREACH change IN the_rtps_reader.reader_cache.changes
{
	if DDS_FILTER(the_rtps_reader, change) ADD change TO a_change_list;
}

RETURN a_change_list;

The DDS_FILTER() operation reflects the capabilities of the DDS DataReader API to select a subset of changes based on
CacheChange::kind, QoS, content-filters and other mechanisms. Note that the logical actions above only reflect the
behavior and not necessarily the actual implementation of the protocol.
------------------------------------------------------------------------------------*/
BufferedInMessage::Ptr BufferedUdpSocketReader::Read()
{
	// IF RTPS then read from rtpsReader.readerCache

    return queueIncomingMessages_.consumer_pop_front();
}

/*------------------------------------------------------------------------------------
This transition is triggered by the destruction of a DDS DataReader "the_dds_reader".
The transition performs the following logical actions in the virtual machine:
delete the_dds_reader.related_rtps_reader;
------------------------------------------------------------------------------------*/
void BufferedUdpSocketReader::Stop()
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

