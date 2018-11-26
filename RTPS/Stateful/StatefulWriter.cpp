/*
 * StatefulWriter.cpp
 *
 *  Created on: Mar 12, 2012
 *      Author: knuthelv
 */

#include"RTPS/Stateful/StatefulWriter.h"
#include"RTPS/Machine/RtpsMessageReceiver.h"

namespace RTPS
{

/*---------------------------------------------------------
Specialization of RTPS Writer used for the Stateful Reference
Implementation. The RTPS StatefulWriter is configured with
the knowledge of all matched RTPS Reader endpoints
and maintains state on each matched RTPS Reader endpoint.
-----------------------------------------------------------*/
StatefulWriter::StatefulWriter()
    : RtpsWriter()
    , heartbeatTimer_()
    , nackResponseTimer_()
{
    init();
}

StatefulWriter::StatefulWriter(const GUID &guid, TopicKindId::Type topicKind, ReliabilityKindId::Type reliabilityKind)
    : RtpsWriter(guid, topicKind, reliabilityKind)
    , heartbeatTimer_()
    , nackResponseTimer_()
{
    init();
}

StatefulWriter::~StatefulWriter()
{
    // nothing to do as long as all ReaderProxies are smart-pointers
}

// ----------------------------------------------------------------------------------
// initializes conditions - called once from constructors
// ----------------------------------------------------------------------------------
void StatefulWriter::init()
{
    //this->newChangeGuard_.SetHandler(*this);
    waitSet_.Attach(newChangeGuard_);
}

// ----------------------------------------------------------------------------------
// virtual function reimplemented from RtpsWriter
// ----------------------------------------------------------------------------------
CacheChange::Ptr StatefulWriter::NewChange(ChangeKindId::Type kind, const RtpsData::Ptr &data, const InstanceHandle &handle)
{
    ++lastChangeSequenceNumber_;

    // TODO: Initialize RtpsData more?
    if(data)
    {
        data->SetWriterSN(lastChangeSequenceNumber_);
        data->SetWriterId(this->GetGUID().GetEntityId());
        //data->SetSrcGuidPrefix(this->GetGUID().GetGuidPrefix());
    }

    // Create CacheChange
    CacheChange::Ptr aChange(new CacheChange(kind, this->GetGUID(), lastChangeSequenceNumber_, data, handle) );

    // -- debug --
    ASSERT(aChange->IsValid());
    // -- debug --

    writerCache_.AddChange(aChange);

    // Add CacheChange to ReaderProxies
    for(MapGUIDReaders::iterator it = mapMatchedReaders_.begin(), it_end = mapMatchedReaders_.end(); it != it_end; ++it)
    {
        ReaderProxy::Ptr &readerProxy = it->second;

        // -- debug --
        ASSERT(readerProxy);
        // -- debug --

        bool isAdded = this->addChangeForReader(readerProxy, aChange);
        ASSERT(isAdded);
    }

    // Set trigger value true which causes call-back to this->operator()()
    this->newChangeGuard_->SetTriggerValue(true);

    return aChange;
}

// ----------------------------------------------------------------------------------
// called from VirtualMachine
// ----------------------------------------------------------------------------------
DCPS::ReturnCode::Type StatefulWriter::WaitDispatch(int64 milliseconds)
{
    switch(this->GetReliabilityKind())
    {
        case ReliabilityKindId::BestEffort:
        {
            //bool state =
            waitSet_.WaitDispatch(milliseconds);
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

            //bool state =
            waitSet_.WaitDispatch(sleepTime);

            // --------------------------------
            // Send out Heartbeats every period
            // --------------------------------
            if(heartbeatTimer_.HasExpired())
            {
                bool sentHeartbeat = sendHeartbeat();            // send out heart-beat

                // -- debug --
                if(!sentHeartbeat)
                    IWARNING() << "Heartbeat response timer expired but no Heartbeat sent!";
                // -- debug --
            }

            // --------------------------------
            // Send out requested changes every period
            // --------------------------------
            if(nackResponseTimer_.HasExpired())
            {
                bool sentRequestedChanges = sendRequestedChanges();     // for all: readerProxy->GetRequestedChanges();

                // -- debug --
                if(!sentRequestedChanges)
                    IWARNING() << "nackResponseTimer_ expired but no Data sent!";
                // -- debug --
            }

            return DCPS::ReturnCode::Ok;
        }
        case ReliabilityKindId::UNKNOWN:
        default:
            return DCPS::ReturnCode::BAD_PARAMETER;
    }

    return DCPS::ReturnCode::PRECONDITION_NOT_MET;
}

// ----------------------------------------------------------------------------------
// Callback from waitSet_: Trigger value set in RtpsWriter::NewCacheChange
//      called when new cache change arrives
//
//      -> Sets the ReaderLocator::canSend() == true if there are unsent changes
//      -> There are unsent changes if dataCache.highestSeqNum > readerLocator.highestSeqNumSent
// ----------------------------------------------------------------------------------
void StatefulWriter::operator()()
{
    // -- prechecks --
    {
        IDEBUG() << "Callback after new cache object added!";

        if(!messageSender_)
        {
            ICRITICAL() << "Message sender is not configured, returning!";
            return;
        }
        if(mapMatchedReaders_.empty())
        {
            IWARNING() << "No matched readers";
        }
    }
    // -- prechecks --


    // Iterate through all ReaderProxy and determine whether there are unsent/unchecked/relevant changes
    for(MapGUIDReaders::iterator it = mapMatchedReaders_.begin(), it_end = mapMatchedReaders_.end(); it != it_end; ++it)
    {
        ReaderProxy::Ptr &readerProxy = it->second;

        // -- debug --
        ASSERT(readerProxy);
        // -- debug --

        GuardCondition::Ptr guard = readerProxy->GetGuardCondition();

        // -- debug --
        ASSERT(guard->HasHandler());
        // -- debug --

        bool hasUnsentChanges = readerProxy->HasUnsentChanges();
        if(!hasUnsentChanges)
        {
            guard->SetTriggerValue(false);
            continue;
        }

        // ReaderProxy has unsent changes
        guard->SetTriggerValue(true);            // triggers waitSet_ and ->operator()(const ReaderProxy &readerProxy);
    }
}

/*-------------------------------------------------------------------------
Callback from waitSet_: Trigger value set in operator()()
- called when new cache change (Data) arrives and not sent out by ReaderLocator yet


This transition is triggered by the guard condition [RP::can_send() == true] indicating that the RTPS Writer
‘the_rtps_writer’ has the resources needed to send a change to the RTPS Reader represented by the ReaderProxy
‘the_reader_proxy.’

The transition performs the following logical actions in the virtual machine:

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

The above logic is not meant to imply that each DATA Submessage is sent in a separate RTPS Message. Rather multiple
Submessages can be combined into a single RTPS message.

After the transition, the following post-conditions hold:

( a_change BELONGS-TO the_reader_proxy.unsent_changes() ) == FALSE
--------------------------------------------------------------------------*/
template<>
void StatefulWriter::operator()(ReaderProxy::Ptr &readerProxy)
{
    // -- debug --
    IDEBUG() << "Callback when new object to cache added and reader locator not sent all!";
    ASSERT(messageSender_);

    bool hasUnsentChanges = readerProxy->HasUnsentChanges();
    ASSERT(hasUnsentChanges);
    // -- debug --

    /*----------------------------------------------------------
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
    ---------------------------------------------------------- */

    // ------------------------------------
    // do send out unsent changes
    // ------------------------------------

    ReaderProxy::MapChangeForReader unsentChanges;
    readerProxy->UnsentChanges(unsentChanges);

    DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerProxy, unsentChanges);

    // -- debug --
    if(retType != DCPS::ReturnCode::Ok)
        ICRITICAL() << "Could not send " << typeid(unsentChanges).name() << " to " << typeid(readerProxy).name();
    // -- debug --

    // -- post-condition --
    ASSERT(!readerProxy->HasUnsentChanges());
}

/*-------------------------------------------------------------------------
This transition is triggered by the firing of a periodic timer configured to fire each W::heartbeatPeriod.
The transition performs the following logical actions for the StatefulWriter ‘the_rtps_writer’ in the virtual machine:

seq_num_min := the_rtps_writer.writer_cache.get_seq_num_min();
seq_num_max := the_rtps_writer.writer_cache.get_seq_num_max();
HEARTBEAT := new HEARTBEAT(the_rtps_writer.writerGuid, seq_num_min, seq_num_max);
HEARTBEAT.FinalFlag := NOT_SET;
HEARTBEAT.readerId := ENTITYID_UNKNOWN;
send HEARTBEAT;
--------------------------------------------------------------------------*/
bool StatefulWriter::sendHeartbeat()
{
    if(!messageSender_) return false;
    if(mapMatchedReaders_.empty()) return false;

    // ------------------------------------
    // do send out heart-beat
    // ------------------------------------

    SequenceNumber seqNumMin = writerCache_.GetSequenceNumberMin();
    SequenceNumber seqNumMax = writerCache_.GetSequenceNumberMax();

    RtpsHeartbeat::Ptr heartbeat = RtpsHeartbeat::Ptr( new RtpsHeartbeat() );
    heartbeat->SetWriterId(this->GetGUID().GetEntityId());
    heartbeat->SetFirstSN(seqNumMin);
    heartbeat->SetLastSN(seqNumMax);
    heartbeat->SetFinalFlag(false);
    heartbeat->SetReaderId(EntityId::UNKNOWN());

    for(MapGUIDReaders::iterator it = mapMatchedReaders_.begin(), it_end = mapMatchedReaders_.end(); it != it_end; ++it)
    {
        ReaderProxy::Ptr &readerProxy = it->second;
        ASSERT(readerProxy);

        DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerProxy, heartbeat);

        // -- debug --
        if(retType != DCPS::ReturnCode::Ok)
            ICRITICAL() << "Could not send " << typeid(heartbeat).name() << " to " << typeid(readerProxy).name();
        // -- debug --
    }

    return true;
}

/*-------------------------------------------------------------------------
This transition is triggered by the guard condition [RP::can_send() == true] indicating that the RTPS Writer
‘the_rtps_writer’ has the resources needed to send a change to the RTPS Reader represented by the ReaderProxy
‘the_reader_proxy.’

The transition performs the following logical actions in the virtual machine:

a_change := the_reader_proxy.next_requested_change();
a_change.status := UNDERWAY;
if (a_change.is_relevant)
{
    DATA = new DATA(a_change, the_reader_proxy.remoteReaderGuid);
    IF (the_reader_proxy.expectsInlineQos)
    {
        DATA.inlineQos := the_rtps_writer.related_dds_writer.qos;
    }
    send DATA;
}
else
{
    GAP = new GAP(a_change.sequenceNumber, the_reader_proxy.remoteReaderGuid);
    send GAP;
}

The above logic is not meant to imply that each DATA or GAP Submessage is sent in a separate RTPS message. Rather
multiple Submessages can be combined into a single RTPS message.
The above illustrates the simplified case where a GAP Submessage includes a single sequence number. This would result
in potentially many Submessages in cases where many sequence numbers in close proximity refer to changes that are not
relevant to the Reader. Efficient implementations will combine multiple ‘irrelevant’ sequence numbers as much as
possible into a single GAP message.

After the transition the following post-condition holds:

( a_change BELONGS-TO the_reader_proxy.requested_changes() ) == FALSE
--------------------------------------------------------------------------*/
bool StatefulWriter::sendRequestedChanges()
{
    if(!messageSender_) return false;
    if(mapMatchedReaders_.empty()) return false;

    // ------------------------------------
    // do send out requested changes
    // ------------------------------------

    for(MapGUIDReaders::iterator it = mapMatchedReaders_.begin(), it_end = mapMatchedReaders_.end(); it != it_end; ++it)
    {
        ReaderProxy::Ptr readerProxy = it->second;
        ASSERT(readerProxy);

        if(!readerProxy->HasRequestedChanges())
        {
            IWARNING() << "No requested changes for reader proxy!";
            continue;
        }

        ReaderProxy::MapChangeForReader requestedChanges;
        readerProxy->RequestedChanges(requestedChanges);

        DCPS::ReturnCode::Type retType = messageSender_->SendTo(this, readerProxy, requestedChanges);

        // -- debug --
        if(retType != DCPS::ReturnCode::Ok)
            ICRITICAL() << "Could not send " << typeid(requestedChanges).name() << " to " << typeid(readerProxy).name();
        // -- debug --

        // Clear out requestedChangeSet
        readerProxy->SetRequestedChangesSet(SequenceNumberSet::UNKNOWN());
    }

    return true;
}

/*-------------------------------------------------------------------------
This Submessage is used to communicate the state of a Reader to a Writer.
The Submessage allows the Reader to inform the Writer about the sequence
numbers it has received and which ones it is still missing. This Submessage
can be used to do both positive and negative acknowledgments.

This transition is triggered by the reception of an ACKNACK Message destined to the RTPS StatefulWriter
‘the_rtps_writer’ originating from the RTPS Reader represented by the ReaderProxy ‘the_reader_proxy.’

The transition performs the following logical actions in the virtual machine:

the_rtps_writer.acked_changes_set(ACKNACK.readerSNState.base - 1);
the_reader_proxy.requested_changes_set(ACKNACK.readerSNState.set);

After the transition the following post-conditions hold:

MIN { change.sequenceNumber IN the_reader_proxy.unacked_changes() } >= ACKNACK.readerSNState.base - 1
--------------------------------------------------------------------------*/
bool StatefulWriter::ReceiveSubmessage(RtpsMessageReceiver *messageReceiver, RtpsAckNack *ackNack)
{
    // -- debug --
    ASSERT(messageReceiver);
    ASSERT(ackNack);
    ASSERT(ackNack->IsValid());
    // -- debug --

   /* --------------------------------------------------------------------------
    The Writer is uniquely identified by its GUID. The Writer GUID is obtained
    using the state of the Receiver:

    writerGUID = { Receiver.destGuidPrefix, AckNack.writerId }
    ----------------------------------------------------------------------------*/

    GUID writerGUID(messageReceiver->GetDestGuidPrefix(), ackNack->GetWriterId());

    if(mapMatchedReaders_.count(writerGUID) <= 0)
    {
        ICRITICAL() << "Reader is not identified!";
        return false;
    }

    ReaderProxy::Ptr readerProxy = mapMatchedReaders_[writerGUID];
    ASSERT(readerProxy);

    SequenceNumber ackSeqNum(ackNack->GetReaderSNState().GetBitmapBase().GetSequenceNumber() - 1);

    readerProxy->AckedChangesSet(ackSeqNum);
    readerProxy->RequestedChangesSet(ackNack->GetReaderSNState());
    readerProxy->SetRequestedChangesSet(ackNack->GetReaderSNState());

    if(readerProxy->HasUnackedChanges())
    {
        if(heartBeatPeriod_ < heartbeatTimer_.Remaining())
            heartbeatTimer_.Start(heartBeatPeriod_);
    }

    return true;
}

/*-------------------------------------------------------------------------
The NackFrag Submessage is used to communicate the state of a Reader to a Writer.
When a data change is sent as a series of fragments, the NackFrag Submessage
allows the Reader to inform the Writer about specific fragment numbers
it is still missing.
--------------------------------------------------------------------------*/
bool StatefulWriter::ReceiveSubmessage(RtpsMessageReceiver *receiver, RtpsNackFrag *nackFrag)
{
    ASSERT(receiver);
    ASSERT(nackFrag);

    //TODO: Currently not supported by this StatefulWriter implementation
    IWARNING() << "NackFrag is currently not supported by this StatefulWriter implementation";
    return true;
}


/*-----------------------------------------------------------
this.attributes := <as specified in the constructor>;
this.changes_for_reader := RTPS::Writer.writer_cache.changes;
FOR_EACH change IN (this.changes_for_reader) DO
{
    IF ( DDS_FILTER(this, change) THEN change.is_relevant := FALSE;
    ELSE change.is_relevant := TRUE;

    IF ( RTPS::Writer.pushMode == true) THEN change.status := UNSENT;
    ELSE change.status := UNACKNOWLEDGED;
}

The above logic indicates that the newly-created ReaderProxy initializes its set of ‘changes_for_reader’ to contain all the
CacheChanges in the Writer’s HistoryCache.
The change is marked as ‘irrelevant’ if the application of any of the DDS-DataReader filters indicates the change is not
relevant to that particular reader. The DDS specification indicates that a DataReader may provide a time-based filter as
well as a content-based filter. These filters should be applied in a manner consistent with the DDS specification to select
any changes that are irrelevant to the DataReader.
The status is set depending on the value of the RTPS Writer attribute ‘pushMode.’
-----------------------------------------------------------*/
ReaderProxy::Ptr StatefulWriter::NewReaderProxy(const GUID &remoteReaderGuid)
{
    ReaderProxy::Ptr readerProxy = ReaderProxy::Ptr(new ReaderProxy(remoteReaderGuid));

    DataCache::MapSeqCacheChange &mapCacheChange= this->writerCache_.GetCacheChanges();
    for(DataCache::MapSeqCacheChange::iterator it = mapCacheChange.begin(), it_end = mapCacheChange.end(); it != it_end; ++it)
    {
        CacheChange::Ptr cacheChange = it->second;
        ASSERT(cacheChange);

        bool isAdded = this->addChangeForReader(readerProxy, cacheChange);
        ASSERT(isAdded);
    }

    // Initialize Functor and attach to wait-set
    {
        bool setHandler = readerProxy->SetHandler(this, readerProxy);
        bool attach = readerProxy->AttachWaitSet(waitSet_);

        ASSERT(setHandler && attach);
    }

    return readerProxy;
}

/*-----------------------------------------------------------
This operation takes a CacheChange a_change as a parameter and determines whether all the ReaderProxy have
acknowledged the CacheChange. The operation will return true if all ReaderProxy have acknowledged the corresponding
CacheChange and false otherwise.

return true IF and only IF
    FOREACH proxy IN this.matched_readers
        IF change IN proxy.changes_for_reader THEN
            change.is_relevant == TRUE AND change.status == ACKNOWLEDGED
-----------------------------------------------------------*/
bool StatefulWriter::IsAckedByAll(const CacheChange::Ptr &cacheChange) const
{
    bool ackedAll = true;
    for(MapGUIDReaders::const_iterator it = mapMatchedReaders_.begin(), it_end = mapMatchedReaders_.end(); it != it_end; ++it)
    {
        const ReaderProxy::Ptr &readerProxy = it->second;
        ASSERT(readerProxy);

        ackedAll = readerProxy->IsAckedChange(cacheChange->GetSequenceNumber());
        if(!ackedAll) break;
    }

    return ackedAll;
}

/*-----------------------------------------------------------
Add the readerProxy to this.matchedReaders;
-----------------------------------------------------------*/
void StatefulWriter::MatchedReaderAdd(ReaderProxy::Ptr readerProxy)
{
    ASSERT(mapMatchedReaders_.count(readerProxy->GetReaderGUID()) == 0);

    mapMatchedReaders_.insert(std::pair<GUID, ReaderProxy::Ptr>(readerProxy->GetReaderGUID(), readerProxy));
}

/*-----------------------------------------------------------
Remove readerProxy from this.matchedReaders;
delete readerProxy;
-----------------------------------------------------------*/
void StatefulWriter::MatchedReaderRemove(ReaderProxy::Ptr readerProxy)
{
    int64 foundReaderProxy = mapMatchedReaders_.erase(readerProxy->GetReaderGUID());

    ASSERT(foundReaderProxy == 1);
}

bool StatefulWriter::MatchedReaderRemove(const GUID &readerGuid)
{
    return mapMatchedReaders_.erase(readerGuid) == 1;
}

/*-----------------------------------------------------------
find and return readerProxy in this.matchedReaders SUCH-THAT readerProxy.GetGUID() == readerGuid;
-----------------------------------------------------------*/
ReaderProxy::Ptr StatefulWriter::MatchedReaderLookup(const GUID &readerGuid) const
{
    MapGUIDReaders::const_iterator it = mapMatchedReaders_.find(readerGuid);

    return it != mapMatchedReaders_.end()
           ? it->second
           : ReaderProxy::Ptr();
}

/*-----------------------------------------------------------
private function adds CacheChange to reader
-----------------------------------------------------------*/
bool StatefulWriter::addChangeForReader(ReaderProxy::Ptr &readerProxy, CacheChange::Ptr &cacheChange)
{
    ChangeForReader::Ptr changeForReader = ChangeForReader::Ptr( new ChangeForReader(true, ChangeForReaderStatusKind::UNSENT) );

    // TODO: Add DDS filter
    if(false) //ddsFilter(readerProxy, cacheChange))
    {
        changeForReader->SetRelevant(false);
    }
    else
    {
        changeForReader->SetRelevant(true);
    }

    if(isPushMode())
    {
        changeForReader->SetStatus(ChangeForReaderStatusKind::UNSENT);
    }
    else
    {
        changeForReader->SetStatus(ChangeForReaderStatusKind::UNACKNOWLEDGED);
    }

    // Add change to ReaderProxy
    bool isAdded = readerProxy->AddChangeForReader(cacheChange->GetSequenceNumber(), changeForReader);

    return isAdded;
}

}
