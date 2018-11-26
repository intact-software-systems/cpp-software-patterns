/*
 * StatefulReader.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include"RTPS/Stateful/StatefulReader.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"
#include"RTPS/Submessages/IncludeLibs.h"

namespace RTPS
{
/*---------------------------------------------------------
Specialization of RTPS Reader. The RTPS StatefulReader keeps
state on each matched RTPS Writer. The state kept on
each writer is encapsulated in the RTPS WriterProxy class.
-----------------------------------------------------------*/
StatefulReader::StatefulReader()
    : RtpsReader()
    , ackNackResponseTimer_()
{
}

StatefulReader::StatefulReader(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
    : RtpsReader(guid, topicKind, reliabilityKind)
    , ackNackResponseTimer_()
{

}

StatefulReader::~StatefulReader()
{
}

// ----------------------------------------------------------------------------------
// called from VirtualMachine
// ----------------------------------------------------------------------------------
DCPS::ReturnCode::Type StatefulReader::WaitDispatch(int64 milliseconds)
{
    switch(this->GetReliabilityKind())
    {
        case ReliabilityKindId::BestEffort:
        {
            bool state = waitSet_.WaitDispatch(milliseconds);

            return DCPS::ReturnCode::Ok;
        }
        case ReliabilityKindId::Reliable:
        {
            int64 sleepTime = milliseconds;

            // Function to calculate sleepTime!
            {
                sleepTime = std::min(sleepTime, ackNackResponseTimer_.Remaining().InMillis());
            }

            bool state = waitSet_.WaitDispatch(sleepTime);

            // --------------------------------
            // Send out AckNack when GuardCondition for WriterProxy is true
            // Should this be a condition setting off every n milliseconds?
            // --------------------------------
            if(ackNackResponseTimer_.HasExpired())
            {
                bool sentAckNack = sendAckNack();            // Action: send out AckNack with all missing changes

                // -- debug --
                if(!sentAckNack)
                    IWARNING() << "AckNack response timer expired but no AckNack sent!";
                // -- debug --
            }

            return DCPS::ReturnCode::Ok;
        }
        default:
            return DCPS::ReturnCode::BAD_PARAMETER;
    }
}

/*-------------------------------------------------------------------------
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
--------------------------------------------------------------------------*/
bool StatefulReader::sendAckNack()
{
    // -- debug --
    ASSERT(messageSender_);
    // -- debug --

    if(!messageSender_) return false;

    bool sentOutAckNack = false;

    for(MapGUIDWriters::iterator it = this->mapMatchedWriters_.begin(), it_end = this->mapMatchedWriters_.end(); it != it_end; ++it)
    {
        WriterProxy::Ptr writerProxy = it->second;
        ASSERT(writerProxy);

        if(!writerProxy->HasMissingChanges()) continue;

        // ----------------------------------------------
        // WriterProxy has missing changes: Send out AckNack
        //
        // Fill SequenceNumberSet and send AckNack for this WriterProxy
        // Send out multiple AckNack in case we cannot fit every missing changes
        // ----------------------------------------------

        SequenceSet seqMissingChanges = writerProxy->MissingChanges();

        // -- debug --
        ASSERT(!seqMissingChanges.empty());
        // -- debug --

        RtpsMessageSender::ListSubmessages listOfSubmessages;

        while(!seqMissingChanges.empty())
        {
            SequenceNumber seqNumBase = *(seqMissingChanges.begin());

            // -- debug --
            ASSERT(seqNumBase != SequenceNumber::UNKNOWN());
            // -- debug --

            // To be sent in RtpsAckNack
            SequenceNumberSet seqNumberMissing;
            seqNumberMissing.SetBitmapBase(seqNumBase);

            {   // find sequence numbers to be added in RtpsAckNack

                SequenceSet::iterator it_in, it_in_end;
                for(it_in = seqMissingChanges.begin(), it_in_end = seqMissingChanges.end(); it_in != it_in_end; ++it_in)
                {
                    SequenceNumber nextMissingSeq = *it_in;

                    int64 currentSeqPosition = nextMissingSeq.GetSequenceNumber() - seqNumBase.GetSequenceNumber();

                    // -- debug --
                    ASSERT(currentSeqPosition >= 0);
                    // -- debug --

                    bool setBit = seqNumberMissing.SetBitAtPosition(currentSeqPosition, true);
                    if(!setBit) break;          // filled up capacity
                }

                // Update sequence number set
                seqMissingChanges.erase(seqMissingChanges.begin(), it_in);
            }

            // ------------------------------------------------
            // Initialize RtpsAckNack to be sent to writer and add to list
            // ------------------------------------------------

            RtpsAckNack::Ptr ackNack = RtpsAckNack::Ptr( new RtpsAckNack() );
            ackNack->SetWriterId(this->GetGUID().GetEntityId());
            ackNack->SetReaderId(this->GetGUID().GetEntityId());
            ackNack->SetReaderSNState(seqNumberMissing);
            // TODO: Set count to avoid receiver processing duplicate packets

            listOfSubmessages.push_back(RtpsSubmessageBase::Ptr( ackNack ));
        }

        // -- debug --
        ASSERT(!listOfSubmessages.empty());
        // -- debug --

        DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, *writerProxy.get(), listOfSubmessages);

        // -- debug --
        if(retType != DCPS::ReturnCode::Ok)
            ICRITICAL() << "Could not send AckNacks " << typeid(listOfSubmessages).name() << " to " << typeid(writerProxy).name();
        // -- debug --

        sentOutAckNack = true;
    }

    return sentOutAckNack;
}

/*-------------------------------------------------------------------------
case ReliabilityKindId::BestEffort:

This transition is triggered by the reception of a DATA message by the RTPS Reader ‘the_rtps_reader.’ The DATA
message encapsulates the change ‘a_change.’ The encapsulation is described in Section 8.3.7.2.
The Best-Effort reader checks that the sequence number associated with the change is strictly greater than the highest
sequence number of all changes received in the past from this RTPS Writer (WP::available_changes_max()). If this check
fails, the RTPS Reader discards the change. This ensures that there are no duplicate changes and no out-of-order changes.
The transition performs the following logical actions in the virtual machine:

a_change := new CacheChange(DATA);
writer_guid := {Receiver.SourceGuidPrefix, DATA.writerId};
writer_proxy := the_rtps_reader.matched_writer_lookup(writer_guid);
expected_seq_num := writer_proxy.available_changes_max() + 1;

if ( a_change.sequenceNumber >= expected_seq_num )
{
    the_rtps_reader.reader_cache.add_change(a_change);
    writer_proxy.received_change_set(a_change.sequenceNumber);
    if ( a_change.sequenceNumber > expected_seq_num )
    {
        writer_proxy.lost_changes_update(a_change.sequenceNumber);
    }
}

After the transition the following post-conditions hold:
writer_proxy.available_changes_max() >= a_change.sequenceNumber

-------------------------------------------
case ReliabilityKindId::Reliable:

This transition is triggered by the reception of a DATA message destined to the RTPS StatefulReader ‘the_reader’
originating from the RTPS Writer represented by the WriterProxy ‘the_writer_proxy.’
The transition performs the following logical actions in the virtual machine:

a_change := new CacheChange(DATA);
the_reader.reader_cache.add_change(a_change);
the_writer_proxy.received_change_set(a_change.sequenceNumber);

Any filtering is done when accessing the data using the DDS DataReader read or take operations, as described in Section
8.2.9.
--------------------------------------------------------------------------*/
bool StatefulReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsData *data)
{
    GUID writerGuid(messageReceiver->GetSourceGuidPrefix(), data->GetWriterId());

    WriterProxy::Ptr writerProxy = this->MatchedWriterLookup(writerGuid);
    if(!writerProxy)
    {
        IDEBUG() << "Could not find writer!";
        return false;
    }

    switch(this->GetReliabilityKind())
    {
        case ReliabilityKindId::BestEffort:
        {
            SequenceNumber expectedSeqNum(this->readerCache_.GetSequenceNumberMax().GetSequenceNumber() + 1);

            if(data->GetWriterSN() >= expectedSeqNum)
            {
                RtpsData::Ptr dataPtr(data);
                CacheChange::Ptr cacheChange = CacheChange::Ptr( new CacheChange(ChangeKindId::ALIVE, writerGuid, data->GetWriterSN(), dataPtr));

                readerCache_.AddChange(cacheChange);

                if(data->GetWriterSN() > expectedSeqNum)
                {
                    writerProxy->LostChangesUpdate(data->GetWriterSN());
                }
            }

            ASSERT(writerProxy->AvailableChangesMax() >= data->GetWriterSN());
            return true;
        }
        case ReliabilityKindId::Reliable:
        {
            RtpsData::Ptr dataPtr(data);
            CacheChange::Ptr cacheChange = CacheChange::Ptr( new CacheChange(ChangeKindId::ALIVE, writerGuid, data->GetWriterSN(), dataPtr));

            readerCache_.AddChange(cacheChange);

            writerProxy->ReceivedChangeSet(data->GetWriterSN());
            return true;
        }
        default:
            return false;
    }
}

/*-----------------------------------------------------------
This transition is triggered by the reception of a GAP message destined to the RTPS StatefulReader ‘the_reader’
originating from the RTPS Writer represented by the WriterProxy ‘the_writer_proxy.’

The transition performs the following logical actions in the virtual machine:

FOREACH seq_num IN [GAP.gapStart, GAP.gapList.base-1] DO
{
    the_writer_proxy.irrelevant_change_set(seq_num);
}

FOREACH seq_num IN GAP.gapList DO
{
    the_writer_proxy.irrelevant_change_set(seq_num);
}
-----------------------------------------------------------*/
bool StatefulReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsGap *gap)
{
    // -- debug --
    ASSERT(messageReceiver);
    ASSERT(gap);
    // -- debug --

    GUID writerGuid(messageReceiver->GetSourceGuidPrefix(), gap->GetWriterId());

    WriterProxy::Ptr writerProxy = this->MatchedWriterLookup(writerGuid);
    if(!writerProxy)
    {
        IDEBUG() << "Could not find writer!";
        return false;
    }

    const SequenceNumberSet &gapList    = gap->GetGapList();
    const SequenceNumber &gapListBase   = gapList.GetBitmapBase();

    // set sequential irrelevant changes
    {
        for(SequenceNumber gapIrellevant = gap->GetGapStart(); gapIrellevant < gapListBase; gapIrellevant++)
        {
            writerProxy->IrrelevantChangeSet(gapIrellevant);
        }
    }

    // set non-sequential irrelevant changes
    {
        SequenceNumber gapIrrelevant = gapList.GetBitmapBase();
        writerProxy->IrrelevantChangeSet(gapIrrelevant);

        for(unsigned int bitPos = 0; bitPos < gapList.GetNumBits(); bitPos++)
        {
            gapIrrelevant++;

            if(gapList.TestBitAtPosition(bitPos))
            {
                writerProxy->IrrelevantChangeSet(gapIrrelevant);
            }
        }
    }

    return true;
}

/*-----------------------------------------------------------
This transition is triggered by the reception of a HEARTBEAT message destined to the RTPS StatefulReader ‘the_reader’
originating from the RTPS Writer represented by the WriterProxy ‘the_writer_proxy.’

The transition performs the following logical actions in the virtual machine:

the_writer_proxy.missing_changes_update(HEARTBEAT.lastSN);
the_writer_proxy.lost_changes_update(HEARTBEAT.firstSN);

if (HB.FinalFlag == NOT_SET)
    then must_send_ack
else if(HB.LivelinessFlag == NOT_SET)
    then may_send_ack
else waiting
-----------------------------------------------------------*/
bool StatefulReader::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsHeartbeat *heartbeat)
{
    switch(this->GetReliabilityKind())
    {
        case ReliabilityKindId::BestEffort:
        {
            return false;
        }
        case ReliabilityKindId::Reliable:
        {
            GUID writerGuid(messageReceiver->GetSourceGuidPrefix(), heartbeat->GetWriterId());

            WriterProxy::Ptr writerProxy = this->MatchedWriterLookup(writerGuid);
            if(!writerProxy)
            {
                IDEBUG() << "Could not find writer!";
                return false;
            }

            writerProxy->MissingChangesUpdate(heartbeat->GetLastSN());
            writerProxy->LostChangesUpdate(heartbeat->GetFirstSN());

            /*-----------------------------------------------------------
            if (HB.FinalFlag == NOT_SET)
                then must_send_ack
            else if(HB.LivelinessFlag == NOT_SET)
                then may_send_ack
            else waiting
            -----------------------------------------------------------*/
            if(!heartbeat->HasFinalFlag())
            {
                // --------------------------------------------------------
                // Must send AckNack so start the ackNack response timer
                // Response timer is only started if it has not previously been started!
                //
                // Logical sleep-time:
                // int64 responseTime = std::min(this->heartBeatResponseDelay_, ackNackResponseTimer_.Remaining());
                // --------------------------------------------------------

                if(this->heartBeatResponseDelay_ < ackNackResponseTimer_.Remaining())
                    ackNackResponseTimer_.Start(this->heartBeatResponseDelay_);
            }
            else if(!heartbeat->HasLivelinessFlag())
            {
                // --------------------------------------------------------
                // State is (finalFlag == true && livelinessFlag == false)
                // May send AckNack only if missing changes
                //
                // Logical sleep-time:
                // int64 responseTime = std::min(this->heartBeatResponseDelay_, ackNackResponseTimer_.Remaining());
                // --------------------------------------------------------

                if(writerProxy->HasMissingChanges())
                {
                    if(this->heartBeatResponseDelay_ < ackNackResponseTimer_.Remaining())
                        ackNackResponseTimer_.Start(this->heartBeatResponseDelay_);
                }
            }
            //else state is (finalFlag == true && livelinessFlag == true)

            return true;
        }
        default:
            return false;
    }
    return true;
}

/*-----------------------------------------------------------
 This operation adds the WriterProxy a_writer_proxy to the StatefulReader::matched_writers.
ADD a_writer_proxy TO {this.matched_writers};
-----------------------------------------------------------*/
void StatefulReader::MatchedWriterAdd(WriterProxy::Ptr &writerProxy)
{
    ASSERT(mapMatchedWriters_.count(writerProxy->GetWriterGUID()) <= 0);

    // Add the writerProxy to this.matchedWriters;
    mapMatchedWriters_.insert(std::pair<GUID, WriterProxy::Ptr>(writerProxy->GetWriterGUID(), writerProxy));
}

/*-----------------------------------------------------------
This operation removes the ReaderProxy a_reader_proxy from the set
StatefulWriter::matched_readers.

REMOVE a_reader_proxy FROM {this.matched_readers};
delete proxy;
-----------------------------------------------------------*/
void StatefulReader::MatchedWriterRemove(WriterProxy::Ptr &writerProxy)
{
    ASSERT(mapMatchedWriters_.count(writerProxy->GetWriterGUID()) <= 0);

    size_t sz = mapMatchedWriters_.erase(writerProxy->GetWriterGUID());
    ASSERT(sz == 1);
}

bool StatefulReader::MatchedWriterRemove(const GUID &writerGuid)
{
    WriterProxy::Ptr writerProxy = this->MatchedWriterLookup(writerGuid);
    if(!writerProxy) return false;

    this->MatchedWriterRemove(writerProxy);
    return true;
}

/*-----------------------------------------------------------
find and return writerProxy in this.matchedWriters SUCH-THAT writerProxy.GetGUID() == writerGuid;
-----------------------------------------------------------*/
WriterProxy::Ptr StatefulReader::MatchedWriterLookup(const GUID &writerGuid)
{
    if(mapMatchedWriters_.count(writerGuid) <= 0)
    {
        IWARNING() << "WriterProxy not found for GUID!";
        return WriterProxy::Ptr();
    }

    return mapMatchedWriters_[writerGuid];
}

}
