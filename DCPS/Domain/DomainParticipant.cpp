/*
 * DomainParticipant.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#include"DCPS/Domain/DomainParticipant.h"
#include"DCPS/Factory/DCPSConfig.h"

namespace DCPS { namespace Domain
{

/**
 * @brief DomainParticipant::DomainParticipant constructor for DomainParticipant.
 *
 * @param domainId
 * @param qos
 * @param listener
 * @param mask
 */
DomainParticipant::DomainParticipant(
        const DDS::Elements::DomainId &domainId,
        const DDS::DomainParticipantQoS &qos,
        const DomainParticipantListener::Ptr &listener,
        const Status::StatusMask &mask)
    : Infrastructure::DomainEntity(mask)
    , Templates::ContextObject<DomainParticipantConfig, DomainParticipantState, Templates::NullCriticalState>(
          DomainParticipantConfig(domainId, qos),
          DomainParticipantState(DDS::Factory::DDSConfig::DDSCacheDescription(domainId)))
    , domainListener_(listener)
{ }

/**
 * @brief DomainParticipant::~DomainParticipant destructs this object.
 */
DomainParticipant::~DomainParticipant()
{ }

/**
 * @brief DomainParticipant::CreatePublisher creates a Publisher that belongs to the DomainParticipant.
 *
 * @return
 */
Publication::Publisher::Ptr DomainParticipant::CreatePublisher()
{
    return this->createPublisher(
                this->config().GetDefaultPublisherQos(),
                Publication::PublisherListener::Ptr(),
                Status::StatusMask::All());
}

/**
 * @brief DomainParticipant::CreatePublisher creates a Publisher that belongs to the DomainParticipant.
 *
 * @param qos
 * @param listener
 * @param statuses
 * @return
 */
Publication::Publisher::Ptr DomainParticipant::CreatePublisher(
        const DDS::PublisherQoS &qos,
        const Publication::PublisherListener::Ptr &listener,
        const Status::StatusMask &statuses)
{
    return this->createPublisher(qos, listener, statuses);
}

/**
 * @brief DomainParticipant::createPublisher creates a Publisher that belongs to the DomainParticipant.
 *
 * @param qos
 * @param listener
 * @param statuses
 * @return
 */
Publication::Publisher::Ptr DomainParticipant::createPublisher(
        const DDS::PublisherQoS &qos,
        const Publication::PublisherListener::Ptr &listener,
        const Status::StatusMask &statuses)
{

    // TODO: InstanceHandle generator? boost uuid library
    // generate instancehandles in base Entity class?
    // Note: dependency to boost....
    // Idea: atomic int that is threadsafe?
    //std::atomic<int> atomicInt;
    //atomicInt++;

    Publication::Publisher::Ptr publisher = Publication::Publisher::Ptr(
                new Publication::Publisher(
                    qos,
                    this->GetPtr(),
                    listener,
                    statuses));

    this->state().publishers().Write(publisher->GetPtr(), publisher->GetInstanceHandle());

    return publisher;
}

/**
 * @brief DomainParticipant::CreateSubscriber
 * @return
 */
Subscription::Subscriber::Ptr DomainParticipant::CreateSubscriber()
{
    return createSubscriber(
                this->config().GetDefaultSubscriberQos(),
                Subscription::SubscriberListener::Ptr(),
                Status::StatusMask::All());
}

/**
 * @brief DomainParticipant::CreateSubscriber
 * @param qos
 * @param listener
 * @param statuses
 * @return
 */
Subscription::Subscriber::Ptr DomainParticipant::CreateSubscriber(
        const DDS::SubscriberQoS &qos,
        const Subscription::SubscriberListener::Ptr &listener,
        const Status::StatusMask &statuses)
{
    return createSubscriber(
                qos,
                listener,
                statuses);
}

/**
 * @brief DomainParticipant::createSubscriber
 * @param qos
 * @param listener
 * @param statuses
 * @return
 */
Subscription::Subscriber::Ptr DomainParticipant::createSubscriber(
        const DDS::SubscriberQoS &qos,
        const Subscription::SubscriberListener::Ptr &listener,
        const Status::StatusMask &statuses)
{
    Subscription::Subscriber::Ptr subscriber = Subscription::Subscriber::Ptr(
                new Subscription::Subscriber(
                    qos,
                    this->GetPtr(),
                    listener,
                    statuses));

    this->state().subscribers().Write(subscriber->GetPtr(), subscriber->GetInstanceHandle());

    return subscriber;
}

/**
 * @brief DomainParticipant::GetBuiltinSubscriber
 * @return
 */
Subscription::Subscriber::Ptr DomainParticipant::GetBuiltinSubscriber()
{
    return Subscription::Subscriber::Ptr();
}

/**
 * @brief DomainParticipant::CloseContainedEntities
 */
void DomainParticipant::CloseContainedEntities()
{

}

/**
 * @brief DomainParticipant::IgnoreParticipant
 * @param handle
 */
void DomainParticipant::IgnoreParticipant(const InstanceHandle &handle)
{

}

/**
 * @brief DomainParticipant::IgnoreTopic
 * @param handle
 */
void DomainParticipant::IgnoreTopic(const InstanceHandle &handle)
{

}

/**
 * @brief DomainParticipant::IgnorePublication
 * @param handle
 */
void DomainParticipant::IgnorePublication(const InstanceHandle &handle)
{

}

/**
 * @brief DomainParticipant::IgnoreSubscription
 * @param handlee
 */
void DomainParticipant::IgnoreSubscription(const InstanceHandle &handlee)
{

}

/**
 * @brief DomainParticipant::AssertLiveliness
 */
void DomainParticipant::AssertLiveliness()
{

}

/**
 * @brief DomainParticipant::GetDiscoveredParticipants
 *
 * TODO: Call the implementation of the discovery service and get the list there
 *
 * @param participantHandles
 * @return
 */
InstanceHandleSeq DomainParticipant::GetDiscoveredParticipants(const InstanceHandleSeq &participantHandles)
{
    return InstanceHandleSeq();
}

/**
 * @brief DomainParticipant::GetDiscoveredTopics
 *
 * TODO: Call the implementation of the discovery service and get the list there
 *
 * @param topicHandles
 * @return
 */
InstanceHandleSeq DomainParticipant::GetDiscoveredTopics(const InstanceHandleSeq &topicHandles)
{
    return InstanceHandleSeq();
}

/**
 * @brief DomainParticipant::ContainsEntity
 * @param handle
 * @return
 */
bool DomainParticipant::ContainsEntity(const InstanceHandle &handle)
{
    return false;
}


/**
 * @brief
 *
 * This operation installs a Listener on the Entity. The listener will only be invoked on the changes of communication status
 * indicated by the specified mask.
 * It is permitted to use ‘nil’ as the value of the listener. The ‘nil’ listener behaves as a Listener whose operations perform no action.
 *
 * Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will replace it
 * with the new one. Consequently if the value ‘nil’ is passed for the listener parameter to the set_listener operation, any
 * existing listener will be removed.
 */
ReturnCode::Type DomainParticipant::SetListener(
        const DomainParticipantListener::Ptr &listener,
        const Status::StatusMask &statusMask)
{
    this->domainListener_ = listener;
    this->statusMask_ = statusMask;

    return ReturnCode::Ok;
}

/**
 * @brief DomainParticipant::GetListener
 * @return
 */
DomainParticipantListener::Ptr DomainParticipant::GetListener()
{
    return domainListener_.lock();
}

/**
 * @brief DomainParticipant::GetInstanceHandle
 * @return
 */
InstanceHandle DomainParticipant::GetInstanceHandle() const
{
    return InstanceHandle::NIL();
}

/**
 * @brief DomainParticipant::Enable
 *
 * According spec:
 * - Calling enable on an already enabled Entity returns OK and has no effect.
 * - Calling enable on an Entity whose factory is not enabled will fail and return PRECONDITION_NOT_MET.
 *
 * @return
 */
ReturnCode::Type DomainParticipant::Enable()
{
    if(IsEnabled())
        return ReturnCode::Ok;

    return DCPS::Infrastructure::Entity::Enable();
}

}}
