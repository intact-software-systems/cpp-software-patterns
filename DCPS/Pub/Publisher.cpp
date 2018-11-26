/*
 * Publisher.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#include"DCPS/Pub/Publisher.h"
#include"DCPS/Domain/DomainParticipant.h"
#include"DCPS/Factory/DCPSConfig.h"

using namespace RxData;

namespace DCPS { namespace Publication
{

/**
 * @brief PublisherConfig::PublisherConfig
 * @param qos
 */
PublisherConfig::PublisherConfig(DDS::PublisherQoS qos)
    : publisherQos_(qos)
    , defaultDataWriterQos_()
{}

PublisherConfig::~PublisherConfig()
{}

ReturnCode::Type PublisherConfig::SetDefaultDataWriterQoS(const DDS::DataWriterQoS &dataWriterQos)
{
    defaultDataWriterQos_ = dataWriterQos;
    return ReturnCode::Ok;
}

ReturnCode::Type PublisherConfig::SetQoS(const DDS::PublisherQoS &publisherQoS)
{
    publisherQos_ = publisherQoS;
    return ReturnCode::Ok;
}

ReturnCode::Type PublisherConfig::GetDefaultDataWriterQoS(DDS::DataWriterQoS &dataWriterQoS) const
{
    dataWriterQoS = defaultDataWriterQos_;
    return ReturnCode::Ok;
}

DDS::PublisherQoS PublisherConfig::GetQoS() const
{
    return publisherQos_;
}

/**
 * @brief PublisherState::PublisherState
 */
PublisherState::PublisherState()
    : participant_()
    , cache_()
{ }

PublisherState::PublisherState(Domain::DomainParticipantPtr participant, CacheDescription description)
    : participant_(participant)
    , cache_(CacheFactory::Instance().getOrCreateCache(description))
{ }

PublisherState::~PublisherState()
{ }

DCPSConfig::DataWriterAccess PublisherState::writers()
{
    return DCPSConfig::DataWriterAccess(cache_, DCPSConfig::DataWriterCacheTypeName());
}

Domain::DomainParticipantPtr PublisherState::GetParticipant() const
{
    return participant_.lock();
}

/**
 * @brief Publisher::Publisher
 * @param qos
 * @param listener
 * @param statuses
 */
Publisher::Publisher(DDS::PublisherQoS qos,
                     Domain::DomainParticipantPtr participant,
                     PublisherListener::Ptr listener,
                     Status::StatusMask statuses)
    : Entity(statuses)
    , Templates::ContextObject<PublisherConfig, PublisherState, Templates::NullCriticalState>
      (
          PublisherConfig(qos),
          PublisherState(participant, DDS::Factory::DDSConfig::DDSCacheDescription(participant->GetDomainId()))
      )
    , publisherListener_(listener)
{ }

Publisher::~Publisher()
{ }

/**
 * @brief
 *
 * This operation indicates to the Service that the application is about to make multiple modifications using DataWriter
 * objects belonging to the Publisher.
 *
 * It is a hint to the Service so it can optimize its performance by e.g., holding the dissemination of the modifications and
 * then batching them.
 *
 * It is not required that the Service use this hint in any way.
 * The use of this operation must be matched by a corresponding call to resume_publications indicating that the set of
 * modifications has completed. If the Publisher is deleted before resume_publications is called, any suspended updates yet
 * to be published will be discarded.
 */
ReturnCode::Type Publisher::SuspendPublications()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation indicates to the Service that the application has completed the multiple changes initiated by the previous
 * suspend_publications. This is a hint to the Service that can be used by a Service implementation to e.g., batch all the
 * modifications made since the suspend_publications.
 *
 * The call to resume_publications must match a previous call to suspend_publications. Otherwise the operation will return
 * the error PRECONDITION_NOT_MET.
 * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
 */
ReturnCode::Type Publisher::ResumePublications()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation requests that the application will begin a 'coherent set' of modifications using DataWriter objects attached
 * to the Publisher. The 'coherent set' will be completed by a matching call to end_coherent_changes.
 * A 'coherent set' is a set of modifications that must be propagated in such a way that they are interpreted at the receivers'
 * side as a consistent set of modifications; that is, the receiver will only be able to access the data after all the modifications
 * in the set are available at the receiver end12.
 *
 * A connectivity change may occur in the middle of a set of coherent changes; for example, the set of partitions used by the
 * Publisher or one of its Subscribers may change, a late-joining DataReader may appear on the network, or a
 * communication failure may occur. In the event that such a change prevents an entity from receiving the entire set of
 * coherent changes, that entity must behave as if it had received none of the set.
 *
 * These calls can be nested. In that case, the coherent set terminates only with the last call to end_coherent_ changes.
 * The support for 'coherent changes' enables a publishing application to change the value of several data-instances that
 * could belong to the same or different topics and have those changes be seen 'atomically' by the readers. This is useful in
 * cases where the values are inter-related. For example, if there are two data-instances representing the 'altitude' and
 * 'velocity vector' of the same aircraft and both are changed, it may be useful to communicate those values in a way the
 * reader can see both together; otherwise, it may e.g., erroneously interpret that the aircraft is on a collision course.
 */
ReturnCode::Type Publisher::BeginCoherentChanges()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation terminates the 'coherent set' initiated by the matching call to begin_coherent_ changes. If there is no
 * matching call to begin_coherent_ changes, the operation will return the error PRECONDITION_NOT_MET.
 * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
 */
ReturnCode::Type Publisher::EndCoherentChanges()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation blocks the calling thread until either all data written by the reliable DataWriter entities is acknowledged by
 * all matched reliable DataReader entities, or else the duration specified by the max_wait parameter elapses, whichever
 * happens first. A return value of OK indicates that all the samples written have been acknowledged by all reliable matched
 * data readers; a return value of TIMEOUT indicates that max_wait elapsed before all the data was acknowledged.
 */
ReturnCode::Type Publisher::WaitForAcknowledgments(Duration &maxWait)
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation deletes all the entities that were created by means of the "create" operations on the Publisher. That is, it
 * deletes all contained DataWriter objects.
 *
 * The operation will return PRECONDITION_NOT_MET if the any of the contained entities is in a state where it cannot be deleted.
 * Once delete_contained_entities returns successfully, the application may delete the Publisher knowing that it has no contained
 * DataWriter objects.
 */
ReturnCode::Type Publisher::DeleteContainedEntities()
{
    this->state().writers().DisposeAll();

    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation copies the policies in the a_topic_qos to the corresponding policies in the a_datawriter_qos (replacing
 * values in the a_datawriter_qos, if present).
 * This is a "convenience" operation most useful in combination with the operations get_default_datawriter_qos and
 * Topic::get_qos. The operation copy_from_topic_qos can be used to merge the DataWriter default QoS policies with the
 * corresponding ones on the Topic. The resulting QoS can then be used to create a new DataWriter, or set its QoS.
 * This operation does not check the resulting a_datawriter_qos for consistency. This is because the 'merged'
 * a_datawriter_qos may not be the final one, as the application can still modify some policies prior to applying the policies
 * to the DataWriter.
 */
ReturnCode::Type Publisher::CopyFromTopicQos(DDS::DataWriterQoS &dataWriterQoS, const DDS::TopicQoS &topicQoS)
{
    return ReturnCode::Ok;
}

/**
 * @brief Publisher::GetParticipant
 *
 * This operation returns the DomainParticipant to which the Publisher belongs.
 *
 * @return
 */
Domain::DomainParticipant::Ptr Publisher::GetParticipant() const
{
    return this->state().GetParticipant();
}

/**
 * @brief Publisher::SetListener installs a Listener on the Entity. The listener will only be invoked on the changes of communication status
 * indicated by the specified mask.
 *
 * It is permitted to use 'nil' as the value of the listener. The 'nil' listener behaves as a Listener whose operations perform
 * no action.
 *
 * Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will replace it
 * with the new one. Consequently if the value 'nil' is passed for the listener parameter to the set_listener operation, any
 * existing listener will be removed.
 *
 * @param listener
 * @param statusMask
 * @return
 */
ReturnCode::Type Publisher::SetListener(const PublisherListener::Ptr &listener, const Status::StatusMask &statusMask)
{
    this->publisherListener_ = listener;
    this->statusMask_ = statusMask;

    return ReturnCode::Ok;
}

/**
 * @brief Publisher::GetListener allows access to the existing Listener attached to the Entity.
 * @return
 */
PublisherListener::Ptr Publisher::GetListener()
{
    return publisherListener_.lock();
}

InstanceHandle Publisher::GetInstanceHandle() const
{
    return InstanceHandle::NIL();
}

/**
 * @brief Publisher::Enable
 *
 * According spec:
 * - Calling enable on an already enabled Entity returns OK and has no effect.
 * - Calling enable on an Entity whose factory is not enabled will fail and return PRECONDITION_NOT_MET.
 *
 * @return
 */
ReturnCode::Type Publisher::Enable()
{
    if(IsEnabled())
        return ReturnCode::Ok;

    Domain::DomainParticipant::Ptr participantPtr = this->state().GetParticipant();

    if(participantPtr)
    {
        if(participantPtr->IsEnabled() == false)
            return ReturnCode::PRECONDITION_NOT_MET;
    }
    else // participant is NULL
    {
        // -- debug --
        ICRITICAL() << "Domain participant is NULL!";
        // -- debug --

        return ReturnCode::PRECONDITION_NOT_MET;
    }

    return Entity::Enable();
}

}}
