/*
 * RtpsMessageReceiver.cpp
 *
 *  Created on: Feb 29, 2012
 *      Author: knuthelv
 */

#include"RTPS/Machine/RtpsMessageReceiver.h"

namespace RTPS
{
/*--------------------------------------------------------------------------------------------
The following algorithm outlines the rules that a receiver of any Message must follow:

1. If the full Submessage header cannot be read, the rest of the Message is considered invalid.

2. The submessageLength field defines where the next Submessage starts or indicates that the Submessage extends to
the end of the Message, as explained in Section 8.3.3.2.3, “submessageLength,” on page 34. If this field is invalid,
the rest of the Message is invalid.

3. A Submessage with an unknown SubmessageId must be ignored and parsing must continue with the next
Submessage. Concretely: an implementation of RTPS 2.1 must ignore any Submessages with IDs that are outside of
the SubmessageKind set defined in version 2.1. SubmessageIds in the vendor-specific range coming from a
vendorId that is unknown must also be ignored and parsing must continue with the next Submessage.

4. Submessage flags. The receiver of a Submessage should ignore unknown flags. An implementation of RTPS 2.1
should skip all flags that are marked as “X” (unused) in the protocol.

5. A valid submessageLength field must always be used to find the next Submessage, even for Submessages with
known IDs.

6. A known but invalid Submessage invalidates the rest of the Message. Section 8.3.7 describes each known
Submessage and when it should be considered invalid.

--------------------------------------------------------------
Reception of a valid header and/or Submessage has two effects:

• It can change the state of the Receiver; this state influences how the following Submessages in the Message are
interpreted. Section 8.3.7 discusses how the state changes for each Submessage. In this version of the protocol, only
the Header and the Submessages InfoSource, InfoReply, InfoDestination, and InfoTimestamp
change the state of the Receiver.

• It can affect the behavior of the Endpoint to which the message is destined. This applies to the basic RTPS messages:
Data, DataFrag, HeartBeat, AckNack, Gap, HeartbeatFrag, NackFrag.
--------------------------------------------------------------------------------------------- */

RtpsMessageReceiver::RtpsMessageReceiver(const GuidPrefix &destGuidPrefix, bool verifyContent)
	: sourceVersion_(ProtocolVersion::PROTOCOLVERSION())	// default to PROTOCOLVERSION
	, sourceVendorId_(VendorId::UNKNOWN())					// default to VENDORID_UNKNOWN
	, sourceGuidPrefix_(GuidPrefix::UNKNOWN())				// default to GUIDPREFIX_UNKNOWN
	, destGuidPrefix_(destGuidPrefix)						// default to GUID of participant receiving msg
	, haveTimestamp_(false)
	, timeStamp_(Timestamp::INVALID())						// default to TIME_INVALID
	, verifyContent_(verifyContent)							// verify message content enabled or disabled
	, nextSubmessageHeader_(0)
{

}

RtpsMessageReceiver::~RtpsMessageReceiver()
{
}

/*--------------------------------------------------------------------------------------------
	entry point to the message receiver. It parses the entire datagram: header, submessages
	Keep list of submessages received
	TODO: The DataEncapsulation should be done before and the endian-type should be defined in NetworkLib::SerializeReader

	Parses the reader buffer, which at a minimum should contain exactly one MessageHeader.
--------------------------------------------------------------------------------------------- */
bool RtpsMessageReceiver::ParseDatagram(NetworkLib::SerializeReader::Ptr &reader, const GuidPrefix &destGuidPrefix)
{
	// -- debug --
	ASSERT(reader);
	ASSERT(reader->GetLength() > 0);
	// -- debug --

	try
	{
		// -------------------------------------
		// First, reset state of receiver
		// -------------------------------------
		resetState(destGuidPrefix);

		// -------------------------------------
		// Read RTPS message header and init receiver
		// -------------------------------------
		MessageHeader messageHeader = getMessageHeader(reader);
		ASSERT(messageHeader.IsValid());

		initializeReceiver(messageHeader);

		// -- fail-safe --
		int prevSubmessageHeader = nextSubmessageHeader_;
		// -- fail-safe --

		// -------------------------------------
		// parse all submessages in buffer
		// -------------------------------------
		while(nextSubmessageHeader_ < reader->GetLength())
		{
			// ---------------------------------
			// 1. Read SubmessageHeader
			// ---------------------------------
			SubmessageHeader submessageHeader;
			nextSubmessageHeader_ = getSubmessageHeader(submessageHeader, reader);

			// -- fail-safe --
			{
				if(prevSubmessageHeader == nextSubmessageHeader_)
				{
					IWARNING() << "Header start previous " << prevSubmessageHeader << " == " << nextSubmessageHeader_ << " next. Skipping rest of packet!";
					break;
				}
				prevSubmessageHeader = nextSubmessageHeader_;
			}
			// -- fail-safe --

			// ---------------------------------
			// 2. Read Submessage
			// ---------------------------------
			RtpsSubmessageBase* submessage = getSubmessage(submessageHeader, reader);
			if(submessage != NULL)
			{
				listSubmessages_.push_back(RtpsSubmessageBase::Ptr(submessage));
			}
		}

		return true;
	}
	catch(Exception ex)
	{
		IWARNING() << " Exception caught:" << ex.msg();
	}

	return false;
}


/*--------------------------------------------------------------------------------------------
	Reads the MessageHeader
--------------------------------------------------------------------------------------------- */
MessageHeader RtpsMessageReceiver::getMessageHeader(NetworkLib::SerializeReader::Ptr &reader)
{
	MessageHeader messageHeader;
	messageHeader.Read(reader.get());

	// -- start check --
	if(verifyContent_)
	{
		// TODO: Verify content and change the receiver state
		/*
			////////////////////////////
			//   CHECKING THE HEADER  //
			////////////////////////////
			RTPSHeader header = null;
			try {
					header = decodeRTPSHeader();
			} catch (MalformedSubmessageException e1) {
					return;
			}
			if (header.getVersion().compareTo(GlobalProperties.protocolVersion) > 0) {
					// cannot accept messages from a greater version
					return;
			}
			if (!header.getVendorId().equals(GlobalProperties.vendorId)) {
					// cannot accept messages from other vendors
					return;
			}
		*/
	}
	// -- end check --

	return messageHeader;
}

/*--------------------------------------------------------------------------------------------
	Reads the SubmessageHeader

	int submessageHeaderStart = packet.getCursorPosition();
	int submessageHeaderStop = submessageHeaderStart + Submessage.HEADER_SIZE;
	// ensuring that there is enough space of the submessage header
	if ( packet.getCursorPosition()+Submessage.HEADER_SIZE > packet.getLength() ) {
			return;
	}

	// reading the kind of the submessage
	_kind = packet.read_octet();
	// reading flags
	_flags = packet.read_octet();
	// setting packet endianess
	packet.setEndianess( ! BitUtility.getFlagAt(_flags,0) );
	// reading the SUBMESSAGE_LENGTH, otherwise called OCTETS_TO_NEXT_HEADER
	_submessageLength = packet.read_short();
	_nextSubmessageHeader = submessageHeaderStop + _submessageLength;

	// submessages are always aligned at 4, this check is done over submessageLength...
	if ( (_submessageLength % 4) != 0 ) {
			return;
	}
	if ( _submessageLength <= 0 ) {
			return;
	}
	// check if there is enough space for this submessage
	if ( _nextSubmessageHeader > packet.getLength() ) {
			return;
	}
--------------------------------------------------------------------------------------------- */
int RtpsMessageReceiver::getSubmessageHeader(SubmessageHeader &submessageHeader, NetworkLib::SerializeReader::Ptr &reader)
{
	// -- start pre-check --
	int64_t submessageHeaderStart = reader->GetBytesRead();
	// -- end pre-check --

	submessageHeader.Read(reader.get());

	// -- start check --
	int submessageHeaderStop = submessageHeaderStart + SubmessageHeader::SIZE();
	int nextSubmessageHeader = submessageHeaderStop + submessageHeader.GetSubmessageLength();

	if(verifyContent_)
	{
		if((submessageHeader.GetSubmessageLength() % 4) != 0)
		{
			throw Exception("Submessages should be aligned at 4 bytes!");
		}
		else if(submessageHeader.GetSubmessageLength() <= 0)
		{
			throw Exception("Submessage length <= 0!");
		}
		else if(nextSubmessageHeader > reader->GetLength())
		{
			throw Exception("Submessage length > datagram-size!");
		}
	}
	// -- end check --

	return nextSubmessageHeader;
}

/*---------------------------------------------------------------------------------------------
The RTPS protocol version 2.1 defines several kinds of Submessages. They are categorized into two groups: Entity-
Submessages and Interpreter-Submessages. Entity Submessages target an RTPS Entity. Interpreter Submessages modify
the RTPS Receiver state and provide context that helps process subsequent Entity Submessages.

Reception of a valid header and/or Submessage has two effects:

• It can change the state of the Receiver; this state influences how the following Submessages in the Message are
interpreted. Section 8.3.7 discusses how the state changes for each Submessage. In this version of the protocol, only
the Header and the Submessages InfoSource, InfoReply, InfoDestination, and InfoTimestamp
change the state of the Receiver.

• It can affect the behavior of the Endpoint to which the message is destined. This applies to the basic RTPS messages:
Data, DataFrag, HeartBeat, AckNack, Gap, HeartbeatFrag, NackFrag.
--------------------------------------------------------------------------------------------- */
RtpsSubmessageBase* RtpsMessageReceiver::getSubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader)
{
	switch(header.GetSubmessageKind())
	{
		case SubmessageKind::ACKNACK:
		case SubmessageKind::DATA:
		case SubmessageKind::DATA_FRAG:
		case SubmessageKind::GAP:
		case SubmessageKind::HEARTBEAT:
		case SubmessageKind::HEARTBEAT_FRAG:
		case SubmessageKind::NACK_FRAG:
		{
			return getEntitySubmessage(header, reader);
		}
		case SubmessageKind::INFO_DST:
		case SubmessageKind::INFO_REPLY:
		case SubmessageKind::INFO_REPLY_IP4:
		case SubmessageKind::INFO_SRC:
		case SubmessageKind::INFO_TS:
		case SubmessageKind::PAD:
		{
			return getInterpreterSubmessage(header, reader);
		}
		case SubmessageKind::UNKNOWN:
		{
			ICRITICAL() << "SubmessageKind was 0x00! " << (char)header.GetSubmessageKind();
		}
		default:
		{
			// Ignore un-identified submessage
			ICRITICAL() << "Unknown submessageKind: " << (char)header.GetSubmessageKind();
			break;
		}
	}

	return NULL;
}

/*---------------------------------------------------------------------------------------------
The Interpreter Submessages are:

• InfoSource: Provides information about the source from which subsequent Entity Submessages originated. This
Submessage is primarily used for relaying RTPS Submessages. This is not discussed in the current specification.

• InfoDestination: Provides information about the final destination of subsequent Entity Submessages. This Submessage
is primarily used for relaying RTPS Submessages. This is not discussed in the current specification.

• InfoReply: Provides information about where to reply to the entities that appear in subsequent Submessages.

• InfoTimestamp: Provides a source timestamp for subsequent Entity Submessages.

• Pad: Used to add padding to a Message if needed for memory alignment.
---------------------------------------------------------------------------------------------*/
RtpsSubmessageBase* RtpsMessageReceiver::getInterpreterSubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader)
{
	switch(header.GetSubmessageKind())
	{
		case SubmessageKind::INFO_DST:
		{
			// TODO: Use when relaying/multicasting
			RtpsInfoDestination rtpsInfoDestination(header);
			rtpsInfoDestination.Read(reader.get());

			ASSERT(rtpsInfoDestination.IsValid());

			if(rtpsInfoDestination.GetGuidPrefix() != GuidPrefix::UNKNOWN())
			{
				this->destGuidPrefix_ = rtpsInfoDestination.GetGuidPrefix();
			}
			// TODO:
			//else
			//{
			//	this->destGuidPrefix_ = <GuidPrefix_t of the Participant receiving the message>
			//}
			break;
		}
		case SubmessageKind::INFO_SRC:
		{
			/* ---------------------------------------------------------
			TODO: Change in state of this:
			TODO: Use when relaying/multicasting

			Receiver.sourceGuidPrefix = InfoSource.guidPrefix
			Receiver.sourceVersion = InfoSource.protocolVersion
			Receiver.sourceVendorId = InfoSource.vendorId
			Receiver.unicastReplyLocatorList = { LOCATOR_INVALID }
			Receiver.multicastReplyLocatorList = { LOCATOR_INVALID }
			haveTimestamp = false
			-----------------------------------------------------------*/
			RtpsInfoSource rtpsInfo(header);
			rtpsInfo.Read(reader.get());

			ASSERT(rtpsInfo.IsValid());

			this->sourceGuidPrefix_             = rtpsInfo.GetSourceGuidPrefix();
			this->sourceVersion_                = rtpsInfo.GetProtocolVersion();;
			this->sourceVendorId_               = rtpsInfo.GetVendorId();
			this->unicastReplyLocatorList_      = LocatorList::INVALID();
			this->multicastReplyLocatorList_    = LocatorList::INVALID();
			this->haveTimestamp_                = false;

			break;
		}
		case SubmessageKind::INFO_REPLY:
		{
			RtpsInfoReply rtpsInfo(header);
			rtpsInfo.Read(reader.get());

			ASSERT(rtpsInfo.IsValid());

			/* -----------------------------------------------------------------
			Receiver.unicastReplyLocatorList = InfoReply.unicastLocatorList
			if ( MulticastFlag )
			{
				Receiver.multicastReplyLocatorList = InfoReply.multicastLocatorList
			}
			else
			{
				Receiver.multicastReplyLocatorList = <empty>
			}
			------------------------------------------------------------------*/

			this->unicastReplyLocatorList_ = rtpsInfo.GetUnicastLocatorList();
			if(rtpsInfo.HasMulticastFlag())
			{
				this->multicastReplyLocatorList_ = rtpsInfo.GetMulticastLocatorList();
			}
			break;
		}
		case SubmessageKind::INFO_REPLY_IP4:
		{
			// TODO: Implement Submessage
			break;
		}
		case SubmessageKind::INFO_TS:
		{
			RtpsInfoTimestamp infoTimestamp(header);
			infoTimestamp.Read(reader.get());

			ASSERT(infoTimestamp.IsValid());

			/*---------------------------------------------------
			if(!InfoTimestamp.InvalidateFlag)
			{
				Receiver.haveTimestamp = true
				Receiver.timestamp = InfoTimestamp.timestamp
			}
			else
			{
				Receiver.haveTimestamp = false
			}
			---------------------------------------------------*/

			if(!infoTimestamp.HasInvalidateFlag())
			{
				this->haveTimestamp_ = true;
				this->timeStamp_ = infoTimestamp.GetTimestamp();
				ASSERT(timeStamp_ != Timestamp::INVALID());
			}
			else
			{
				this->haveTimestamp_ = false;
			}
			break;
		}
		case SubmessageKind::PAD:
		{
			RtpsPad pad(header);
			pad.Read(reader.get());

			ASSERT(pad.IsValid());

			break;
		}
		default:
			ICRITICAL() << "Unknown interpreter SubmessagKind: " << (char)header.GetSubmessageKind();
			break;
	}
	return NULL;
}

/*---------------------------------------------------------------------------------------------
The Entity Submessages are:
• Data: Contains information regarding the value of an application Date-object. Data Submessages are sent by Writers
(NO_KEY Writer or WITH_KEY Writer) to Readers (NO_KEY Reader or WITH_KEY Reader).

• DataFrag: Equivalent to Data, but only contains a part of the new value (one or more fragments). Allows data to be
transmitted as multiple fragments to overcome transport message size limitations.

• Heartbeat: Describes the information that is available in a Writer. Heartbeat messages are sent by a Writer
(NO_KEY Writer or WITH_KEY Writer) to one or more Readers (NO_KEY Reader or WITH_KEY Reader).

• HeartbeatFrag: For fragmented data, describes what fragments are available in a Writer. HeartbeatFrag messages
are sent by a Writer (NO_KEY Writer or WITH_KEY Writer) to one or more Readers (NO_KEY Reader or
WITH_KEY Reader).

• Gap: Describes the information that is no longer relevant to Readers. Gap messages are sent by a Writer to one or
more Readers.

• AckNack: Provides information on the state of a Reader to a Writer. AckNack messages are sent by a Reader to one
or more Writers.

• NackFrag: Provides information on the state of a Reader to a Writer, more specifically what fragments the Reader is
still missing. NackFrag messages are sent by a Reader to one or more Writers.
---------------------------------------------------------------------------------------------*/
RtpsSubmessageBase* RtpsMessageReceiver::getEntitySubmessage(SubmessageHeader &header, NetworkLib::SerializeReader::Ptr &reader)
{
	RtpsSubmessageBase* submessage = NULL;

	// -- 1. Initialize submessage object
	switch(header.GetSubmessageKind())
	{
		case SubmessageKind::ACKNACK:
		{
			submessage = new RtpsAckNack(header);
			break;
		}
		case SubmessageKind::DATA:
		{
			submessage = new RtpsData(header);
			break;
		}
		case SubmessageKind::DATA_FRAG:
		{
			submessage = new RtpsDataFrag(header);
			break;
		}
		case SubmessageKind::GAP:
		{
			submessage = new RtpsGap(header);
			break;
		}
		case SubmessageKind::HEARTBEAT:
		{
			submessage = new RtpsHeartbeat(header);
			break;
		}
		case SubmessageKind::HEARTBEAT_FRAG:
		{
			submessage = new RtpsHeartbeatFrag(header);
			break;
		}
		case SubmessageKind::NACK_FRAG:
		{
			submessage = new RtpsNackFrag(header);
			break;
		}
		default:
			ICRITICAL() << "Unknown entity SubmessageKind: " << (char)header.GetSubmessageKind();
			break;
	}

	// -- 2. Read message
	if(submessage != NULL)
	{
		submessage->Read(reader.get());
		submessage->SetSrcGuidPrefix(this->sourceGuidPrefix_);

		if(this->timeStamp_ != Timestamp::INVALID())
		{
			submessage->SetTimestamp(this->timeStamp_);
		}

		// -- debug --
		ASSERT(submessage->IsValid());
		// -- debug --
	}

	return submessage;
}

/*--------------------------------------------------------------------------------------------
	The initial state of the Receiver is described in Section 8.3.4.
--------------------------------------------------------------------------------------------- */
void RtpsMessageReceiver::resetState(const GuidPrefix &destGuidPrefix)
{
	sourceVersion_ 		= ProtocolVersion::PROTOCOLVERSION();	// default to PROTOCOLVERSION
	sourceVendorId_ 	= VendorId::UNKNOWN();					// default to VENDORID_UNKNOWN
	sourceGuidPrefix_ 	= GuidPrefix::UNKNOWN();				// default to GUIDPREFIX_UNKNOWN
	destGuidPrefix_ 	= destGuidPrefix;						// default to GUID of participant receiving msg
	haveTimestamp_ 		= false;								// default to false
	timeStamp_ 			= Timestamp::INVALID();					// default to TIME_INVALID

    listSubmessages_.clear();                                 // clear all previously parsed submessages
}

/*--------------------------------------------------------------------------------------------
	The initial state of the Receiver is described in Section 8.3.4. This section describes
	how the Header of a new Message affects the state of the Receiver.

	Receiver.sourceGuidPrefix = Header.guidPrefix
	Receiver.sourceVersion = Header.version
	Receiver.sourceVendorId = Header.vendorId
	Receiver.haveTimestamp = false
--------------------------------------------------------------------------------------------- */
void RtpsMessageReceiver::initializeReceiver(const MessageHeader &messageHeader)
{
	sourceVersion_ 		= messageHeader.GetProtocolVersion();
	sourceVendorId_ 	= messageHeader.GetVendorId();
	sourceGuidPrefix_ 	= messageHeader.GetGuidPrefix();
	haveTimestamp_		= false;
}

} // namespace RTPS

