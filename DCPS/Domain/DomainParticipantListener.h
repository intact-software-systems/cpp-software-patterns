/*
 * DomainParticipantListener.h
 *
 *  Created on: Nov 4, 2012
 *      Author: knuthelv
 */

#ifndef DCPS_Domain_DomainParticipantListener_h_IsIncluded
#define DCPS_Domain_DomainParticipantListener_h_IsIncluded

#include "DCPS/Infrastructure/Listener.h"
#include "DCPS/Status/StatusFwd.h"
#include "DCPS/Sub/SubFwd.h"
#include "DCPS/Pub/PubFwd.h"
#include "DCPS/Topic/TopicFwd.h"

namespace DCPS { namespace Domain
{

/**
 * @brief
 *
 * The purpose of the DomainParticipantListener is to be the listener of last resort that is notified of all status
 * changes not captured by more specific listeners attached to the DomainEntity objects. When a relevant status
 * change occurs, the DCPS Service will first attempt to notify the listener attached to the concerned DomainEntity
 * if one is installed. Otherwise, the DCPS Service will notify the Listener attached to the DomainParticipant.
 * The relationship between listeners is described in Section 7.1.4, Listeners, Conditions, and Wait-sets, on page 120.
 */
class DomainParticipantListener : public DCPS::Infrastructure::Listener
{
public:
    virtual ~DomainParticipantListener() { }

    CLASS_TRAITS(DomainParticipantListener)

public:
    // ----------------------------------------------------
    // From DataWriterListener
    // ----------------------------------------------------
    virtual void OnOfferedIncompatibleQos(DCPS::Publication::AnyDataWriterHolderBase *writer, const DCPS::Status::OfferedIncompatibleQosStatus &status) = 0;
    virtual void OnOfferedDeadlineMissed(DCPS::Publication::AnyDataWriterHolderBase *writer, const DCPS::Status::OfferedDeadlineMissedStatus &status) = 0;
    virtual void OnLivelinessLost(DCPS::Publication::AnyDataWriterHolderBase* writer, const DCPS::Status::LivelinessLostStatus &status) = 0;
    virtual void OnPublicationMatch(DCPS::Publication::AnyDataWriterHolderBase* writer, const DCPS::Status::PublicationMatchedStatus &status) = 0;

    // ----------------------------------------------------
    // From DataReaderListener
    // ----------------------------------------------------
    virtual void OnRequestedDeadlineMissed(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::RequestedDeadlineMissedStatus& status) = 0;
    virtual void OnRequestedIncompatibleQos(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::RequestedIncompatibleQosStatus& status) = 0;
    virtual void OnSampleRejected(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::SampleRejectedStatus& status) = 0;
    virtual void OnLivelinessChanged(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::LivelinessChangedStatus& status) = 0;
    virtual void OnDataAvailable(DCPS::Subscription::AnyDataReaderHolderBase *reader) = 0;
    virtual void OnSubscriptionMatched(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::SubscriptionMatchedStatus& status) = 0;
    virtual void OnSampleLost(DCPS::Subscription::AnyDataReaderHolderBase *reader, const DCPS::Status::SampleLostStatus& status) = 0;

    // ----------------------------------------------------
    // From TopicListener
    // ----------------------------------------------------
    virtual void OnInconsistentTopic(DCPS::Topic::TopicHolder *topic, const DCPS::Status::InconsistentTopicStatus &status) = 0;

    // ----------------------------------------------------
    // From SubscriberListener
    // ----------------------------------------------------
    virtual void OnDataOnReaders(DCPS::Subscription::Subscriber *subscriber) = 0;
};

/**
 * @brief The NoOpDomainParticipantListener class is a no-op implementation of the DomainParticipantListener
 */
class NoOpDomainParticipantListener : public DomainParticipantListener
{
public:
    virtual ~NoOpDomainParticipantListener() { }

    CLASS_TRAITS(NoOpDomainParticipantListener)

public:
    // ----------------------------------------------------
    // From DataWriterListener
    // ----------------------------------------------------
    virtual void OnOfferedIncompatibleQos(DCPS::Publication::AnyDataWriterHolderBase*, const DCPS::Status::OfferedIncompatibleQosStatus &) { }
    virtual void OnOfferedDeadlineMissed(DCPS::Publication::AnyDataWriterHolderBase*, const DCPS::Status::OfferedDeadlineMissedStatus &) { }
    virtual void OnLivelinessLost(DCPS::Publication::AnyDataWriterHolderBase*, const DCPS::Status::LivelinessLostStatus &) { }
    virtual void OnPublicationMatch(DCPS::Publication::AnyDataWriterHolderBase*, const DCPS::Status::PublicationMatchedStatus &) { }

    // ----------------------------------------------------
    // From DataReaderListener
    // ----------------------------------------------------
    virtual void OnRequestedDeadlineMissed(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::RequestedDeadlineMissedStatus& ) { }
    virtual void OnRequestedIncompatibleQos(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::RequestedIncompatibleQosStatus& ) { }
    virtual void OnSampleRejected(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::SampleRejectedStatus& ) { }
    virtual void OnLivelinessChanged(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::LivelinessChangedStatus& ) { }
    virtual void OnDataAvailable(DCPS::Subscription::AnyDataReaderHolderBase *) { }
    virtual void OnSubscriptionMatched(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::SubscriptionMatchedStatus& ) { }
    virtual void OnSampleLost(DCPS::Subscription::AnyDataReaderHolderBase *, const DCPS::Status::SampleLostStatus& ) { }

    // ----------------------------------------------------
    // From TopicListener
    // ----------------------------------------------------
    virtual void OnInconsistentTopic(DCPS::Topic::TopicHolder *, const DCPS::Status::InconsistentTopicStatus &) { }

    // ----------------------------------------------------
    // From SubscriberListener
    // ----------------------------------------------------
    virtual void OnDataOnReaders(DCPS::Subscription::Subscriber *) { }
};

}}

#endif // DCPS_Domain_DomainParticipantListener_h_IsIncluded
