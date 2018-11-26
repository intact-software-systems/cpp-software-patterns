#include"DCPS/Factory/DCPSFactory.h"
#include"DCPS/Sub/IncludeLibs.h"
#include"DCPS/Discovery/IncludeLibs.h"
#include"DCPS/Topic/IncludeLibs.h"
#include"DCPS/Domain/DomainParticipant.h"

namespace DCPS { namespace Factory {

/**
 * @brief DCPSFactory::factory_
 */
Singleton<DCPSFactory> DCPSFactory::factory_;

/**
 * @brief DCPSFactory::Instance
 * @return
 */
DCPSFactory &DCPSFactory::Instance()
{
    return factory_.GetRef();
}

/**
 * @brief DCPSFactory::CreateBuiltinSubscriber
 * @param participant
 * @return
 */
Subscription::SubscriberPtr DCPSFactory::CreateBuiltinSubscriber(Domain::DomainParticipantPtr participant)
{
    // -------------------------------------------------
    // Create builtin subscriber
    // -------------------------------------------------
    // TODO: Init subscriberQos
    DDS::SubscriberQoS subscriberQos;

    Subscription::Subscriber::Ptr subscriber =
            participant->CreateSubscriber
            (
                subscriberQos,
                Subscription::SubscriberListener::Ptr(),
                Status::StatusMask::All()
            );

    DDS::TopicQoS topicQos(
                DDS::Policy::TopicData(),
                DDS::Policy::Durability(DDS::Policy::DurabilityKind::TRANSIENT_LOCAL),
                DDS::Policy::Deadline(Duration::Infinite()),
                DDS::Policy::LatencyBudget(Duration::Infinite()),
                DDS::Policy::Liveliness::Automatic(),
                DDS::Policy::Reliability::Reliable(Duration::FromMilliseconds(100)),
                DDS::Policy::DestinationOrder::ReceptionTimestamp(),
                DDS::Policy::History::KeepLast(1),
                DDS::Policy::ResourceLimits::Unlimited(),
                DDS::Policy::TransportPriority(1),
                DDS::Policy::Lifespan(Duration::Infinite()),
                DDS::Policy::Ownership::Shared());

    DCPSFactory::Instance().InitializeBuiltinTopic<DDS::Discovery::TopicBuiltinTopicData>(participant, subscriber, topicQos);
    DCPSFactory::Instance().InitializeBuiltinTopic<DDS::Discovery::SubscriptionBuiltinTopicData>(participant, subscriber, topicQos);
    DCPSFactory::Instance().InitializeBuiltinTopic<DDS::Discovery::PublicationBuiltinTopicData>(participant, subscriber, topicQos);
    DCPSFactory::Instance().InitializeBuiltinTopic<DDS::Discovery::ParticipantBuiltinTopicData>(participant, subscriber, topicQos);

    return subscriber->GetPtr();
}

// ------------------------------------------------------------------------------
// Create functions for each init type
// ------------------------------------------------------------------------------

DomainParticipantInit::Ptr DCPSFactory::CreateDomainParticipantInit()
{
    DomainParticipantInitBuilder builder(DCPSFactory::Instance().context());
    return builder.Create();
}

PublisherInit::Ptr DCPSFactory::CreatePublisherInit()
{
    PublisherInitBuilder builder(DCPSFactory::Instance().context());
    return builder.Create();
}

SubscriberInit::Ptr DCPSFactory::CreateSubscriberInit()
{
    SubscriberInitBuilder builder(DCPSFactory::Instance().context());
    return builder.Create();
}

// ------------------------------------------------------------------------------
// Create functions for each builder type
// ------------------------------------------------------------------------------

DomainParticipantInit::Ptr DomainParticipantInitBuilder::Create()
{
    return RTPSDomainParticipantInit::Ptr( new RTPSDomainParticipantInit());
}

SubscriberInit::Ptr SubscriberInitBuilder::Create()
{
    return RTPSSubscriberInit::Ptr( new RTPSSubscriberInit());
}

PublisherInit::Ptr PublisherInitBuilder::Create()
{
    return RTPSPublisherInit::Ptr( new RTPSPublisherInit());
}

}}
