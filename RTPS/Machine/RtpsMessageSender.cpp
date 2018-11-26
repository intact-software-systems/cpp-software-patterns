/*
 * RtpsMessageSender.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: knuthelv
 */

#include"RTPS/Machine/RtpsMessageSender.h"
#include"RTPS/Stateless/StatelessWriter.h"
#include"RTPS/Stateless/StatelessReader.h"
#include"RTPS/Stateful/StatefulWriter.h"
#include"RTPS/Stateful/StatefulReader.h"

namespace RTPS
{
/*--------------------------------------------------------------------------------------------
						class RtpsMessageSender

- Sends RTPS messages using SocketWriter
- TODO: Add specialized send functions for best-effort, reliable, etc, use templates?
--------------------------------------------------------------------------------------------- */
RtpsMessageSender::RtpsMessageSender(const GuidPrefix &writerGuidPrefix, IOWriter::Ptr writer, unsigned int datagramSize)
	: writerVersion_(ProtocolVersion::PROTOCOLVERSION())
	, writerVendorId_(VendorId::UNKNOWN())
	, senderGuidPrefix_(writerGuidPrefix)
	//, timeStamp_(Timestamp::INVALID())
	, datagramSize_(datagramSize)
	, socketWriter_(writer)
{

}

RtpsMessageSender::~RtpsMessageSender()
{

}


/*--------------------------------------------------------------------------------------------
a_change := the_reader_locator.next_requested_change();
IF a_change IN the_writer.writer_cache.changes
{
	DATA = new DATA(a_change);
	IF (the_reader_locator.expectsInlineQos)
	{
		DATA.inlineQos := the_writer.related_dds_writer.qos;
	}
	DATA.readerId := ENTITYID_UNKNOWN;
	sendto the_reader_locator.locator, DATA;
}
ELSE
{
	GAP = new GAP(a_change.sequenceNumber);
	GAP.readerId := ENTITYID_UNKNOWN;
	sendto the_reader_locator.locator, GAP;
}

Sends RTPS messages using SocketWriter
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr readerLocator, const SequenceSet &sequenceSet)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		DataCache           &writerCache = statelessWriter->GetDataCache();     // The Cache with all available DATA
		ListSubmessages     submessages;                                        // populate with data to be sent in DATA
		SequenceSet         missingCache;                                       // populate with missing caches to be added to GAP
		const Locator       &locator = readerLocator->GetLocator();

		// ---------------------------------------------
		// Iterate through all cache data to be sent
		// ---------------------------------------------
		for(SequenceSet::iterator it = sequenceSet.begin(), it_end = sequenceSet.end(); it != it_end; ++it)
		{
			const SequenceNumber &sequenceNumber = *it;

			CacheChange::Ptr cacheChange = writerCache.GetChange(sequenceNumber);

			// ---------------------------------------------
			// Note that it is possible that the requested change had
			// already been removed from the HistoryCache by the DDS
			// DataWriter. In that case, the StatelessWriter sends a GAP Message.
			// --------------------------------------------
			if(!cacheChange) // GAP
			{
				std::pair<SequenceSet::iterator, bool> inserted = missingCache.insert(sequenceNumber);

				// -- debug --
				ASSERT(inserted.second);
				// -- debug --
			}
			else // DATA
			{
				returnCode = addDataMessages(cacheChange->GetData(), submessages, statelessWriter->GetGUID().GetEntityId(), locator, readerLocator->ExpectsInlineQos(), true);
			}
		}

		// ---------------------------------------------------
		// Initialize GAP messages until MissingCache is empty
		// ---------------------------------------------------
		if(!missingCache.empty())
		{
			returnCode = addGapMessages(submessages, missingCache, statelessWriter->GetGUID().GetEntityId());
		}

		// ----------------------------------------------------
		// If Data or GAP is added then send it as RTPS Message
		// ----------------------------------------------------
		if(!submessages.empty())
		{
			returnCode = writeMessage(submessages, locator);
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return returnCode;
}

DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr readerLocator, const SequenceRange &seqNumRange)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		SequenceNumber          sequenceNumber  = seqNumRange.first;            // first unsent
		const SequenceNumber&   endSeq          = seqNumRange.second;           // last unsent

		DataCache               &writerCache = statelessWriter->GetDataCache(); // The Cache with all available DATA
		ListSubmessages         submessages;                                    // populate with data to be sent in DATA
		SequenceSet             missingCache;                                   // populate with missing caches to be added to GAP
		const Locator           &locator = readerLocator->GetLocator();

		// ---------------------------------------------
		// Iterate through all cache data to be sent
		// ---------------------------------------------
		for( ; sequenceNumber <= endSeq; sequenceNumber++)
		{
			CacheChange::Ptr cacheChange = writerCache.GetChange(sequenceNumber);

			// ---------------------------------------------
			// Note that it is possible that the requested change had
			// already been removed from the HistoryCache by the DDS
			// DataWriter. In that case, the StatelessWriter sends a GAP Message.
			// --------------------------------------------
			if(!cacheChange) // GAP
			{
				std::pair<SequenceSet::iterator, bool> inserted = missingCache.insert(sequenceNumber);

				// -- debug --
				ASSERT(inserted.second);
				// -- debug --
			}
			else // DATA
			{
				returnCode = addDataMessages(cacheChange->GetData(), submessages,statelessWriter->GetGUID().GetEntityId(), locator, readerLocator->ExpectsInlineQos(), true);
			}
		}

		// ---------------------------------------------------
		// Initialize GAP messages until MissingCache is empty
		// ---------------------------------------------------
		if(!missingCache.empty())
		{
			returnCode = addGapMessages(submessages, missingCache, statelessWriter->GetGUID().GetEntityId());
		}

		// ----------------------------------------------------
		// If Data or GAP is added then send it as RTPS Message
		// ----------------------------------------------------
		if(!submessages.empty())
		{
			returnCode = writeMessage(submessages, locator);
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
        return DCPS::ReturnCode::Error;
	}

	return returnCode;
}


/*--------------------------------------------------------------------------------------------
This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeatPeriod.
The transition performs the following logical actions in the virtual machine for the Writer ‘the_rtps_writer’ and
ReaderLocator ‘the_reader_locator.’

seq_num_min := the_rtps_writer.writer_cache.get_seq_num_min();
seq_num_max := the_rtps_writer.writer_cache.get_seq_num_max();
HEARTBEAT := new HEARTBEAT(the_rtps_writer.writerGuid, seq_num_min, seq_num_max);
HEARTBEAT.FinalFlag := SET;
HEARTBEAT.readerId := ENTITYID_UNKNOWN;
sendto the_reader_locator, HEARTBEAT;
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatelessWriter* statelessWriter, ReaderLocator::Ptr readerLocator, RtpsHeartbeat::Ptr heartbeat)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		ListSubmessages submessages;
		submessages.push_back( RtpsSubmessageBase::Ptr(heartbeat) );

		returnCode = this->writeMessage(submessages, readerLocator->GetLocator());
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return returnCode;
}


/*--------------------------------------------------------------------------------------------
	a_change := the_reader_proxy.next_unsent_change();
	a_change.status := UNDERWAY;
	if(a_change.is_relevant)
	{
		DATA = new DATA(a_change);
		IF (the_reader_proxy.expectsInlineQos) {
			DATA.inlineQos := the_rtps_writer.related_dds_writer.qos;
		}
		DATA.readerId := ENTITYID_UNKNOWN;
		send DATA;
	}
	// -- For Reliable ReaderProxy only --
	else
	{
		GAP = new GAP(a_change.sequenceNumber);
		GAP.readerId := ENTITYID_UNKNOWN;
		send GAP;
	}
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatefulWriter* statefulWriter, ReaderProxy::Ptr readerProxy, ReaderProxy::MapChangeForReader &mapChangeForReader)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		DataCache           &writerCache = statefulWriter->GetDataCache();      // The Cache with all available DATA
		ListSubmessages     submessages;                                        // populate with data to be sent in DATA
		SequenceSet         missingCache;                                       // populate with missing caches to be added to GAP
		Locator             locator;

		{
			// TODO: use entire list of locators!
			LocatorList locatorList = readerProxy->GetUnicastLocatorList();

			// -- debug --
			ASSERT(!locatorList.GetLocatorVector().empty());
			ASSERT(locatorList.GetLocatorVector().size() == 1);
			// -- debug --

			locator = *(locatorList.GetLocatorVector().begin());
		}

		// ---------------------------------------------
		// Iterate through all cache data to be sent
		// ---------------------------------------------
		for(ReaderProxy::MapChangeForReader::iterator it = mapChangeForReader.begin(), it_end = mapChangeForReader.end(); it != it_end; ++ it)
		{
			const SequenceNumber &sequenceNumber = it->first;

			ChangeForReader::Ptr changeForReader = it->second;

			// -- debug --
			ASSERT(changeForReader);
			// -- debug --

			changeForReader->SetStatus(ChangeForReaderStatusKind::UNDERWAY);

			CacheChange::Ptr cacheChange = writerCache.GetChange(sequenceNumber);

			// ---------------------------------------------
			// Note that it is possible that the requested change had
			// already been removed from the HistoryCache by the DDS
			// DataWriter. In that case, the StatefulWriter sends a GAP Message.
			// --------------------------------------------
			if(!cacheChange || !changeForReader->IsRelevant()) // GAP
			{
				missingCache.insert(sequenceNumber);
			}
			else // DATA
			{
				returnCode = addDataMessages(cacheChange->GetData(), submessages, statefulWriter->GetGUID().GetEntityId(), locator, readerProxy->ExpectsInlineQos(), false);
			}
		}

		// ---------------------------------------------------
		// Initialize GAP messages until MissingCache is empty
		// ---------------------------------------------------
		if(!missingCache.empty())
		{
			returnCode = addGapMessages(submessages, missingCache, statefulWriter->GetGUID().GetEntityId());
		}

		// ----------------------------------------------------
		// If Data or GAP is added then send it as RTPS Message
		// ----------------------------------------------------
		if(!submessages.empty())
		{
			returnCode = writeMessage(submessages, locator);    // TODO: readerProxy->GetLocator());
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return returnCode;
}
/*--------------------------------------------------------------------------------------------
This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeatPeriod.
The transition performs the following logical actions for the StatefulWriter ‘the_rtps_writer’ in the virtual machine:

seq_num_min := the_rtps_writer.writer_cache.get_seq_num_min();
seq_num_max := the_rtps_writer.writer_cache.get_seq_num_max();
HEARTBEAT := new HEARTBEAT(the_rtps_writer.writerGuid, seq_num_min, seq_num_max);
HEARTBEAT.FinalFlag := NOT_SET;
HEARTBEAT.readerId := ENTITYID_UNKNOWN;
send HEARTBEAT;
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatefulWriter* statefulWriter, ReaderProxy::Ptr readerProxy, RtpsHeartbeat::Ptr heartbeat)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		ListSubmessages submessages;
		submessages.push_back( RtpsSubmessageBase::Ptr(heartbeat) );

		Locator locator;

		{
			// TODO: use entire list of locators!
			LocatorList locatorList = readerProxy->GetUnicastLocatorList();

			// -- debug --
			ASSERT(!locatorList.GetLocatorVector().empty());
			ASSERT(locatorList.GetLocatorVector().size() == 1);
			// -- debug --

			locator = *(locatorList.GetLocatorVector().begin());
		}

		returnCode = this->writeMessage(submessages, locator);
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return returnCode;
}

/*--------------------------------------------------------------------------------------------
This transition is triggered by the firing of a timer indicating that the duration of R::heartbeatResponseDelay has elapsed
since the state must_send_ack was entered.

The transition performs the following logical actions for the WriterProxy ‘the_writer_proxy’ in the virtual machine:

missing_seq_num_set.base := the_writer_proxy.available_changes_max() + 1;
missing_seq_num_set.set := <empty>;

FOREACH change IN the_writer_proxy.missing_changes() DO
	ADD change.sequenceNumber TO missing_seq_num_set.set;

send ACKNACK(missing_seq_num_set);

The above logical action does not express the fact that the PSM mapping of the ACKNACK message will be limited in
its capacity to contain sequence numbers. In the case where the ACKNACK message cannot accommodate the complete
list of missing sequence numbers it should be constructed such that it contains the subset with smaller value of the
sequence number.
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatefulReader *statefulReader, WriterProxy &writerProxy, ListSubmessages &submessages)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		LocatorList locatorList = writerProxy.GetUnicastLocatorList();

		// -- debug --
		ASSERT(!locatorList.GetLocatorVector().empty());
		ASSERT(locatorList.GetLocatorVector().size() == 1);
		// -- debug --

		Locator locator = *(locatorList.GetLocatorVector().begin());

		returnCode = this->writeMessage(submessages, locator);
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return returnCode;
}


/*--------------------------------------------------------------------------------------------
  Encapsulates Submessages in an RTPS Message and writes to UDP socket
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::writeMessage(const ListSubmessages &submessages, const Locator &locator)
{
	try
	{
		// -- pre checks --
		if(submessages.empty())	return DCPS::ReturnCode::NODATA;
		// -- end pre checks --

		// --------------------------------------------------------------
		// - How to send as one consecutive memory block?
		//      Allocate a ByteArray with given datagram size and continuously write to it without deleting memory
		//      works like a function stack, start from memory position 0 every time.
		//      ByteArray uses SetPosition(0) to reset it.
		// --------------------------------------------------------------

		// add DataEncapsulation
		DataEncapsulation encapsulation;

		// Initialize MessageHeader
		MessageHeader messageHeader = MessageHeader::DEFAULT();
		messageHeader.SetGuidPrefix(this->senderGuidPrefix_);

		ByteArray msgWrapper(datagramSize_);
		for(ListSubmessages::const_iterator it = submessages.begin(), it_end = submessages.end(); it != it_end; ++it)
		{
			RtpsSubmessageBase::Ptr submessage = *it;

			// -- debug --
			ASSERT(submessage);
			// -- debug --

			// -----------------------------------------------------------------
			// Check if datagram size is being violated if adding submessage
			// if yes then write out current message and build another datagram
			// containing one RTPS MessageHeader
			// -----------------------------------------------------------------
			if(msgWrapper.GetLength() + submessage->GetSubmessageSize() > datagramSize_)
			{
				DCPS::ReturnCode::Type ret = this->writeOutDatagram(msgWrapper, locator);

				// -- debug --
				ASSERT(ret == DCPS::ReturnCode::Ok);
				// -- debug --

				// Reset position without erasing data!
				msgWrapper.SetPosition(0);
			}

			// ---------------------------
			// first submessage added
			// ---------------------------
			if(msgWrapper.GetLength() == 0)
			{
				encapsulation.Write(&msgWrapper);
				messageHeader.Write(&msgWrapper);
			}

			// -----------------------------
			// Add submessage to byte array
			// -----------------------------
			bool success = submessage->WriteWithHeader(&msgWrapper);
			ASSERT(success);
		}

		// -----------------------------------------------------------------
		// Send out last Message
		// -----------------------------------------------------------------
		if(msgWrapper.GetLength() > 0)
		{
			ASSERT(msgWrapper.GetLength() < datagramSize_);

			this->writeOutDatagram(msgWrapper, locator);
		}
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caught : " << ex.msg();
		return DCPS::ReturnCode::Error;
	}

	return DCPS::ReturnCode::Ok;
}

DCPS::ReturnCode::Type RtpsMessageSender::writeOutDatagram(NetworkLib::SerializeWriter &msgWrapper, const Locator &locator)
{
	// -------------------------------------------------
	// Add RtpsPad to fill out size of datagram
	// Size of RtpsPad is minimum submessage size == 4
	// -------------------------------------------------
	int paddingSize = datagramSize_ - (msgWrapper.GetLength() + SubmessageHeader::SIZE());
	if(paddingSize > 4)
	{
		IDEBUG() << "Added RTPS Pad with size " << paddingSize;
		RtpsPad rtpsPad(paddingSize);

		rtpsPad.WriteWithHeader(&msgWrapper);
	}

	// -------------------------------------------------
	//  Write out RTPS Message to socket
	// -------------------------------------------------
	bool success = msgWrapper.Flush(socketWriter_.get(), locator.GetAddress(), locator.GetPort());

	// -- debug --
	ASSERT(success);
	if(!success)
	{
		ICRITICAL() << "Could not send datagram!";
		return DCPS::ReturnCode::Error;
	}
	// -- debug --

	return DCPS::ReturnCode::Ok;
}

/*--------------------------------------------------------------------------------------------
	Initialize GAP submessages based on missing SequenceNumbers and insert into list
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::addGapMessages(ListSubmessages &submessages, SequenceSet &missingCache, const EntityId &entityId)
{
	// ---------------------------------------------------
	// Initialize GAP messages until MissingCache is empty
	// ---------------------------------------------------
	while(!missingCache.empty())
	{
		RtpsGap::Ptr gap = RtpsGap::Ptr( new RtpsGap() );
		gap->SetReaderId(EntityId::UNKNOWN());
		gap->SetWriterId(entityId);

		// ---------------------------------------------
		// Populate SequenceNumberSet with missing changes
		// -------------------------------------------
		SequenceNumberSet   seqList = SequenceNumberSet::UNKNOWN();
		SequenceNumber      prevSeq = SequenceNumber::UNKNOWN();

		SequenceSet::iterator it, it_end;
		for(it = missingCache.begin(), it_end = missingCache.end(); it != it_end; ++it)
		{
			const SequenceNumber &seqNum = *it;

			if(prevSeq == SequenceNumber::UNKNOWN())
			// -> if first, set GAP start as first element
			{
				gap->SetGapStart(seqNum);
			}
			else if(prevSeq.GetSequenceNumber() != (seqNum.GetSequenceNumber() - 1))
			// -> if non continuous then initialize SequenceNumberSet
			{
				if(seqList.IsEmpty())
				// -> if first, set bit-map base as previous SequenceNumber
				{
					seqList.SetBitmapBase(prevSeq);
				}

				bool ret = seqList.AddSeqNumber(seqNum, true);
				if(!ret)
				{
					break;
				}
			}
			else if(!seqList.IsEmpty())
			// -> even if continuous now, previously it was non-continuous, so continue adding to SequenceNumberset
			{
				bool ret = seqList.AddSeqNumber(seqNum, true);
				if(!ret)
				{
					break;
				}
			}

			prevSeq = seqNum;
		}

		missingCache.erase(missingCache.begin(), it);

		submessages.push_back(RtpsSubmessageBase::Ptr(gap));
	}

	return DCPS::ReturnCode::Ok;
}
/*--------------------------------------------------------------------------------------------
	Initialize DATA submessages based on missing SequenceNumbers and insert into list
--------------------------------------------------------------------------------------------- */
DCPS::ReturnCode::Type RtpsMessageSender::addDataMessages(RtpsData::Ptr data, ListSubmessages &submessages, const EntityId &entityId, const Locator &locator, bool expectsInlineQos, bool isStateless)
{
	// -----------------------------------------------------------------
	// Check if sender (writer/reader) is Stateful or Stateless
	// -----------------------------------------------------------------

	// if Stateless then add RtpsInfoReply
	if(isStateless)
	{
		RtpsInfoReply::Ptr infoReply = RtpsInfoReply::Ptr( new RtpsInfoReply(locator) );
		submessages.push_back( RtpsSubmessageBase::Ptr(infoReply) );
	}

	// -----------------------------------------------------------------
	// Add DATA message to list
	// -----------------------------------------------------------------

	// -- debug --
	ASSERT(data);
	ASSERT(data->GetWriterId() == entityId);
	// -- debug --

	if(expectsInlineQos)
	{
		// TODO: Init StatelessWriter QoS from DDS
		//data->SetInlineQos(statelessWriter->GetRelatedDDSWriterQos());
	}

	data->SetReaderId(EntityId::UNKNOWN());     // then needs RtpsInfoReply

	submessages.push_back(RtpsSubmessageBase::Ptr(data));                 // insert data to be sent

	return DCPS::ReturnCode::Ok;
}

/*--------------------------------------------------------------------------------------------
This transition is triggered by the guard condition [RL::can_send() == true] indicating that the RTPS Writer ‘the_writer’
has the resources needed to send a change to the RTPS ReaderLocator ‘the_reader_locator.’ The transition performs the
following logical actions in the virtual machine.

	a_change := the_reader_locator.next_requested_change();
	IF a_change IN the_writer.writer_cache.changes
	{
		DATA = new DATA(a_change);
		IF (the_reader_locator.expectsInlineQos)
		{
			DATA.inlineQos := the_writer.related_dds_writer.qos;
		}
		DATA.readerId := ENTITYID_UNKNOWN;
		sendto the_reader_locator.locator, DATA;
	}
	ELSE
	{
		GAP = new GAP(a_change.sequenceNumber);
		GAP.readerId := ENTITYID_UNKNOWN;
		sendto the_reader_locator.locator, GAP;
	}

After the transition the following post-conditions hold:

( a_change BELONGS-TO the_reader_locator.requested_changes() ) == FALSE

Note that it is possible that the requested change had already been removed from the HistoryCache by the DDS
DataWriter. In that case, the StatelessWriter sends a GAP Message.


Send out DATA as specified in seqNumSet -> called from -> bool StatelessWriter::sendRequestedChanges();
--------------------------------------------------------------------------------------------- */
/*DCPS::ReturnCode::Type RtpsMessageSender::SendTo(StatelessWriter* statelessWriter, ReaderLocator *locator, const SequenceNumberSet &seqNumSet)
{
	ASSERT(socketWriter_);

	DCPS::ReturnCode::Type returnCode = DCPS::ReturnCode::Ok;

	try
	{
		SequenceNumber relevantSeq = seqNumSet.GetBitmapBase();

		SequenceSet seqSet;
		seqSet.insert(relevantSeq);

		for(int bitPos = 0; bitPos < seqNumSet.GetNumBits(); bitPos++)
		{
			relevantSeq++;
			if(gapList.TestBitAtPosition(bitPos))
			{
				seqSet.insert(relevantSeq);
			}
		}

		returnCode = this->SendTo(statelessWriter, locator, seqSet);
	}
	catch(BaseLib::Exception ex)
	{
		IWARNING() << "Exception caugh:" << ex.msg();
		return DCPS::ReturnCode::Error;
	}
	return returnCode;
}*/

} // namespace RTPS
