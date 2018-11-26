/*
 * StatelessWriter.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include"RTPS/Stateless/StatelessWriter.h"
#include"RTPS/Submessages/IncludeLibs.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"

namespace RTPS
{
/*-------------------------------------------------------------------------
    constructors/destructor
-------------------------------------------------------------------------*/
StatelessWriter::StatelessWriter(const Duration &resendDataPeriod)
    : RtpsWriter()
    , resendDataPeriod_(resendDataPeriod)
    , heartbeatTimer_()
    , nackResponseTimer_()
{
    init();
}

StatelessWriter::StatelessWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind, const Duration &resendDataPeriod)
    : RtpsWriter(guid, topicKind, reliabilityKind)
    , resendDataPeriod_(resendDataPeriod)
    , heartbeatTimer_()
    , nackResponseTimer_()
{
    init();
}

StatelessWriter::~StatelessWriter()
{
    IDEBUG() << "Called!";
}

/*-------------------------------------------------------------------------
initializes conditions - called once from constructors
-------------------------------------------------------------------------*/
void StatelessWriter::init()
{
    //this->newChangeGuard_.SetHandler(*this);
    waitSet_.Attach(newChangeGuard_);

    //if(this->GetReliabilityKind() == ReliabilityKindId::Reliable)
    //{
    //	this->heartbeatTimer_.Start(5000);      //this->heartBeatPeriod_.get_in_milliseconds());
    //	this->nackResponseTimer_.Start(200);   //this->nackResponseDelay_
    //}
}

/*-------------------------------------------------------------------------
    called from VirtualMachine
-------------------------------------------------------------------------*/
DCPS::ReturnCode::Type StatelessWriter::WaitDispatch(int64 milliseconds)
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
                sleepTime = std::min(sleepTime, heartbeatTimer_.Remaining().InMillis());
                sleepTime = std::min(sleepTime, nackResponseTimer_.Remaining().InMillis());
            }

            bool state = waitSet_.WaitDispatch(sleepTime);

            // --------------------------------
            // Send out Heartbeats every period
            // --------------------------------
            if(heartbeatTimer_.HasExpired())
            {
                bool sentHeartbeat = sendHeartbeat();            // send out heart-beat

                // -- debug --
                if(sentHeartbeat == false)
                    IWARNING() << "Heartbeat response timer expired but no Heartbeat sent!";
                // -- debug --
            }

            // --------------------------------
            // Send out requested changes every period
            // --------------------------------
            if(nackResponseTimer_.HasExpired())
            {
                bool sentRequestedChanges = sendRequestedChanges();     // for all: readerLocator->GetRequestedChanges();

                // -- debug --
                if(sentRequestedChanges == false)
                    IWARNING() << "nackResponseTimer_ expired but no Data sent!";
                // -- debug --
            }

            return DCPS::ReturnCode::Ok;
        }
        default:
            return DCPS::ReturnCode::BAD_PARAMETER;
    }

    return DCPS::ReturnCode::PRECONDITION_NOT_MET;
}

/*-------------------------------------------------------------------------
Callback from waitSet_: Trigger value set in RtpsWriter::NewCacheChange
      called when new cache change arrives
        -> Sets the ReaderLocator::canSend() == true if there are unsent changes
        -> There are unsent changes if dataCache.highestSeqNum > readerLocator.highestSeqNumSent
--------------------------------------------------------------------------*/
void StatelessWriter::operator()()
{
    IDEBUG() << "Callback after new cache object added!";

    if(!messageSender_)
    {
        ICRITICAL() << "Message sender is not configured, returning!";
        return;
    }

    SequenceNumber highestSeqNum = writerCache_.GetSequenceNumberMax();

    // called when new cache change arrives
    for(MapLocatorReaders::iterator it = mapReaderLocators_.begin(), it_end = mapReaderLocators_.end(); it != it_end; ++it)
    {
        ReaderLocator::Ptr& readerLocator = it->second;

        // -- debug --
        ASSERT(readerLocator);
        // -- debug --

        SequenceNumber highestSeqNumSent    = readerLocator->GetHighestSeqNumSent();
        GuardCondition::Ptr guard           = readerLocator->GetGuardCondition();

        // -- debug --
        ASSERT(highestSeqNum != SequenceNumber::UNKNOWN());
        ASSERT(highestSeqNumSent != SequenceNumber::UNKNOWN());
        ASSERT(guard->HasHandler());
        // -- debug --

        if(highestSeqNum > highestSeqNumSent)   // set trigger to true
        {
            guard->SetTriggerValue(true);        // triggers waitSet_ and ->operator()(const ReaderLocator &readerLocator);
        }
        else // set trigger to false
        {
            guard->SetTriggerValue(false);
        }
    }

    // --------------------------------------------------
    //  Start or re-start heartbeat-timer that is used in main thread
    // --------------------------------------------------
    if(this->heartBeatPeriod_ < this->heartbeatTimer_.Remaining())
        this->heartbeatTimer_.Start(this->heartBeatPeriod_);
}

/*-------------------------------------------------------------------------
Callback from waitSet_: Trigger value set in operator()()
        - called when new cache change (Data) arrives and not sent out by ReaderLocator yet
--------------------------------------------------------------------------*/
template<>
void StatelessWriter::operator()(ReaderLocator::Ptr &readerLocator)
{
    // -- debug --
    IDEBUG() << "Callback when new object to cache added and reader locator not sent all!";
    ASSERT(messageSender_);
    // -- debug --


    { // Data

        SequenceNumber highestSeqNum        = writerCache_.GetSequenceNumberMax();
        SequenceNumber highestSeqNumSent    = readerLocator->GetHighestSeqNumSent();

        if(highestSeqNum > highestSeqNumSent)
        {
            // send to Locator
            SequenceRange seqRange(highestSeqNumSent, highestSeqNum);

            DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerLocator, seqRange);

            // -- debug --
            if(retType != DCPS::ReturnCode::Ok)
                ICRITICAL() << "Could not send Data to ReaderLocator";
            // -- debug --

            readerLocator->SetHighestSeqNumSent(highestSeqNum);
        }

        // -- post-condition --
        ASSERT(highestSeqNum == readerLocator->GetHighestSeqNumSent());
    }
}

/*-------------------------------------------------------------------------
This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeatPeriod.
The transition performs the following logical actions in the virtual machine for the Writer ‘the_rtps_writer’ and
ReaderLocator ‘the_reader_locator.’

seq_num_min := the_rtps_writer.writer_cache.get_seq_num_min();
seq_num_max := the_rtps_writer.writer_cache.get_seq_num_max();
HEARTBEAT := new HEARTBEAT(the_rtps_writer.writerGuid, seq_num_min, seq_num_max);
HEARTBEAT.FinalFlag := SET;
HEARTBEAT.readerId := ENTITYID_UNKNOWN;
sendto the_reader_locator, HEARTBEAT;
--------------------------------------------------------------------------*/
bool StatelessWriter::sendHeartbeat()
{
    if(!messageSender_) return false;
    if(mapReaderLocators_.empty() == true) return false;

    // --------------------------------
    // do send out heart-beat
    // --------------------------------

    SequenceNumber seqNumMin = writerCache_.GetSequenceNumberMin();
    SequenceNumber seqNumMax = writerCache_.GetSequenceNumberMax();

    RtpsHeartbeat::Ptr heartbeat = RtpsHeartbeat::Ptr( new RtpsHeartbeat() );
    heartbeat->SetWriterId(this->GetGUID().GetEntityId());
    heartbeat->SetFirstSN(seqNumMin);
    heartbeat->SetLastSN(seqNumMax);
    heartbeat->SetFinalFlag(true);
    heartbeat->SetReaderId(EntityId::UNKNOWN());

    for(MapLocatorReaders::iterator it = mapReaderLocators_.begin(), it_end = mapReaderLocators_.end(); it != it_end; ++it)
    {
        ReaderLocator::Ptr readerLocator = it->second;
        ASSERT(readerLocator);

        DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerLocator, heartbeat);

        // -- debug --
        if(retType != DCPS::ReturnCode::Ok)
            ICRITICAL() << "Could not send Heartbeat to ReaderLocator";
        // -- debug --
    }

    return true;
}

/*-------------------------------------------------------------------------
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
--------------------------------------------------------------------------*/
bool StatelessWriter::sendRequestedChanges()
{
    if(!messageSender_) return false;
    if(mapReaderLocators_.empty() == true) return false;

    // ------------------------------------
    // do send out requested changes
    // ------------------------------------

    for(MapLocatorReaders::iterator it = mapReaderLocators_.begin(), it_end = mapReaderLocators_.end(); it != it_end; ++it)
    {
        ReaderLocator::Ptr readerLocator = it->second;
        ASSERT(readerLocator);

        SequenceNumberSet seqNumSet = readerLocator->GetRequestedChangesSet();

        if(seqNumSet.IsEmpty())
        {
            IWARNING() << "No valid sequenceNumberSet for requested changes!";
            continue;
        }

        // Create SequenceSet and send out relevant changes
        SequenceSet seqSet;
        {
            SequenceNumber relevantSeq = seqNumSet.GetBitmapBase();

            seqSet.insert(relevantSeq);

            for(unsigned int bitPos = 0; bitPos < seqNumSet.GetNumBits(); bitPos++)
            {
                relevantSeq++;
                if(seqNumSet.TestBitAtPosition(bitPos))
                {
                    seqSet.insert(relevantSeq);
                }
            }
        }

        DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerLocator, seqSet);

        // -- debug --
        if(retType != DCPS::ReturnCode::Ok)
            ICRITICAL() << "Could not send Requested Change Data to ReaderLocator";
        // -- debug --

        // Clear out requestedChangeSet
        readerLocator->SetRequestedChangesSet(SequenceNumberSet::UNKNOWN());
    }

    return true;
}

/*-------------------------------------------------------------------------
This Submessage is used to communicate the state of a Reader to a Writer.
The Submessage allows the Reader to inform the Writer about the sequence
numbers it has received and which ones it is still missing. This Submessage
can be used to do both positive and negative acknowledgments.

This transition is triggered by the reception of an ACKNACK message destined to the RTPS StatelessWriter
‘the_rtps_writer’ originating from some RTPS Reader.

The transition performs the following logical actions in the virtual machine:

FOREACH reply_locator_t IN { Receiver.unicastReplyLocatorList, Receiver.multicastReplyLocatorList }
    reader_locator := the_rtps_writer.reader_locator_lookup(reply_locator_t);
    reader_locator.requested_changes_set(ACKNACK.readerSNState.set);

Note that the processing of this message uses the reply locators in the RTPS Receiver. This is the only source of
information for the StatelessWriter to determine where to send the reply to. Proper functioning of the protocol requires
that the RTPS Reader inserts an InfoReply Submessage ahead of the AckNack such that these fields are properly set.
--------------------------------------------------------------------------*/
bool StatelessWriter::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsAckNack *ackNack)
{
    // -- debug --
    ASSERT(messageReceiver);
    ASSERT(ackNack);
    // -- debug --

    const SequenceNumberSet& ackNackReaderSNState = ackNack->GetReaderSNState();

    { // unicast

        const LocatorList &unicastLocatorList = messageReceiver->GetUnicastReplyLocatorList();
        const LocatorVector &locatorVector = unicastLocatorList.GetLocatorVector();

        for(LocatorVector::const_iterator it = locatorVector.begin(), it_end = locatorVector.end(); it != it_end; ++it)
        {
            const Locator &locator = *it;

            ReaderLocator::Ptr readerLocator = getOrCreateReaderLocator(locator);

            // -- debug --
            ASSERT(readerLocator);
            // -- debug --

            // TODO: Potential race condition by overwriting existing requested change set!
            readerLocator->SetRequestedChangesSet(ackNackReaderSNState);

            // --------------------------------------------------
            //  Start or re-start nack-response-timer that is used in main thread
            // --------------------------------------------------
            if(ackNackReaderSNState.IsEmpty() == false)
            {
                if(this->nackResponseDelay_ < this->nackResponseTimer_.Remaining())
                    this->nackResponseTimer_.Start(this->nackResponseDelay_);
            }
        }
    }

    { // multicast

        const LocatorList &multicastLocatorList = messageReceiver->GetMulticastReplyLocatorList();
        const LocatorVector &locatorVector = multicastLocatorList.GetLocatorVector();

        for(LocatorVector::const_iterator it = locatorVector.begin(), it_end = locatorVector.end(); it != it_end; ++it)
        {
            const Locator &locator = *it;

            ReaderLocator::Ptr readerLocator = getOrCreateReaderLocator(locator);

            // -- debug --
            ASSERT(readerLocator);
            // -- debug --

            // TODO: Potential race condition by overwriting existing requested change set!
            readerLocator->SetRequestedChangesSet(ackNackReaderSNState);

            // --------------------------------------------------
            //  Start or re-start nack-response-timer that is used in main thread
            // --------------------------------------------------
            if(ackNackReaderSNState.IsEmpty() == false)
            {
                if(this->nackResponseDelay_ < this->nackResponseTimer_.Remaining())
                    this->nackResponseTimer_.Start(this->nackResponseDelay_);
            }
        }
    }

    return true;
}

/*-------------------------------------------------------------------------
The NackFrag Submessage is used to communicate the state of a Reader to a Writer.
When a data change is sent as a series of fragments, the NackFrag Submessage
allows the Reader to inform the Writer about specific fragment numbers
it is still missing.
--------------------------------------------------------------------------*/
bool StatelessWriter::ReceiveSubmessage(RtpsMessageReceiver *, RtpsNackFrag *)
{
    //TODO: Currently not supported by this StatelessWriter implementation
    IWARNING() << "NackFrag is currently not supported by this StatelessWriter implementation";
    return true;
}

/*-------------------------------------------------------------------------
This operation adds the Locator_t a_locator to the StatelessWriter::reader_locators.
ADD a_locator TO {this.reader_locators};
--------------------------------------------------------------------------*/
void StatelessWriter::ReaderLocatorAdd(const Locator &locator)
{
    if(mapReaderLocators_.count(locator))
    {
        IWARNING() << "Already added locator!";
        return;
    }

    ReaderLocator::Ptr readerLocator = addReaderLocator(locator);
    ASSERT(readerLocator);
}

/*-------------------------------------------------------------------------
This operation removes the Locator_t a_locator from the StatelessWriter::reader_locators.
REMOVE a_locator FROM {this.reader_locators};
--------------------------------------------------------------------------*/
void StatelessWriter::ReaderLocatorRemove(const Locator &locator)
{
    if(mapReaderLocators_.count(locator) == 0)
    {
        IWARNING() << "Already removed locator!";
        return;
    }

    size_t sz = mapReaderLocators_.erase(locator);
    ASSERT(sz == 1);
}

/*-------------------------------------------------------------------------
This operation modifies the set of ‘unsent_changes’ for all the ReaderLocators in the StatelessWriter::reader_locators.
The list of unsent changes is reset to match the complete list of changes available in the writer’s HistoryCache.

FOREACH readerLocator in {this.reader_locators} DO
readerLocator.unsent_changes := {this.writer_cache.changes}
--------------------------------------------------------------------------*/
void StatelessWriter::UnsentChangesReset()
{
    SequenceNumber seqNumMin = writerCache_.GetSequenceNumberMin();

    for(MapLocatorReaders::iterator it = mapReaderLocators_.begin(), it_end = mapReaderLocators_.end(); it != it_end; ++it)
    {
        ReaderLocator::Ptr readerLocator = it->second;
        ASSERT(readerLocator);

        readerLocator->SetHighestSeqNumSent(seqNumMin);
    }
}

/*-------------------------------------------------------------------------
private helper functions to get, create, add, etc ReaderLocators
--------------------------------------------------------------------------*/
ReaderLocator::Ptr StatelessWriter::getOrCreateReaderLocator(const Locator &locator)
{
    ReaderLocator::Ptr readerLocator;

    MapLocatorReaders::const_iterator it = mapReaderLocators_.find(locator);
    if(it == mapReaderLocators_.end())
    {
        readerLocator = addReaderLocator(locator);
    }
    else
    {
        readerLocator = it->second;
    }

    return readerLocator;
}

ReaderLocator::Ptr StatelessWriter::addReaderLocator(const Locator &locator)
{
    ReaderLocator::Ptr readerLocator = ReaderLocator::Ptr( new ReaderLocator(locator));

    // Initialize Functor and attach to wait-set
    {
        bool ret = readerLocator->SetHandler(this, readerLocator);
        ret = readerLocator->AttachWaitSet(waitSet_);

        ASSERT(ret);
    }

    std::pair<MapLocatorReaders::iterator, bool> inserted = mapReaderLocators_.insert(std::pair<Locator,ReaderLocator::Ptr>(locator, readerLocator));
    ASSERT(inserted.second);

    return readerLocator;
}

} // namespace RTPS

