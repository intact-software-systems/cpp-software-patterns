/*
 * Subscriber.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#include"DCPS/Sub/Subscriber.h"
#include"DCPS/Domain/DomainParticipant.h"
#include"DCPS/Factory/DCPSConfig.h"

using namespace RxData;

namespace DCPS { namespace Subscription
{

/**
 * @brief SubscriberState::SubscriberState
 * @param participant
 * @param description
 */
SubscriberState::SubscriberState()
    : participant_()
    , sampleLostStatus_()
    , cache_()
{ }

SubscriberState::SubscriberState(Domain::DomainParticipantPtr participant, CacheDescription description)
    : participant_(participant)
    , sampleLostStatus_()
    , cache_(CacheFactory::Instance().getOrCreateCache(description))
{ }

SubscriberState::~SubscriberState()
{ }

Status::SampleLostStatus &SubscriberState::GetSampleLostStatus()
{
    return sampleLostStatus_;
}

Domain::DomainParticipantPtr SubscriberState::GetParticipant() const
{
    return participant_.lock();
}

const Status::SampleLostStatus &SubscriberState::GetSampleLostStatus() const
{
    return sampleLostStatus_;
}

DCPSConfig::DataReaderAccess SubscriberState::readers()
{
    return DCPSConfig::DataReaderAccess(cache_, DCPSConfig::DataReaderCacheTypeName());
}

/**
 * @brief Subscriber::Subscriber
 * @param qos
 * @param listener
 * @param statuses
 */
Subscriber::Subscriber(DDS::SubscriberQoS qos,
                       Domain::DomainParticipantPtr participant,
                       SubscriberListener::Ptr listener,
                       Status::StatusMask statuses)
    : Entity(statuses)
    , Templates::ContextObject<SubscriberConfig, SubscriberState, Templates::NullCriticalState>
      (
          SubscriberConfig(qos, DDS::DataReaderQoS::Default()),
          SubscriberState(participant, DDS::Factory::DDSConfig::DDSCacheDescription(participant->GetDomainId()))
      )
    , subscriberListener_(listener)
{ }

/**
 * @brief Subscriber::~Subscriber
 */
Subscriber::~Subscriber()
{ }

/**
 *	@brief
 *
 * This operation indicates that the application is about to access the data samples in any of the DataReader objects attached
 * to the Subscriber.
 * The application is required to use this operation only if PRESENTATION QosPolicy of the Subscriber to which the
 * DataReader belongs has the access_scope set to ‘GROUP.’
 * In the aforementioned case, the operation begin_access must be called prior to calling any of the sample-accessing
 * operations, namely: get_datareaders on the Subscriber and read, take, read_w_condition, take_w_condition on any
 * DataReader. Otherwise the sample-accessing operations will return the error PRECONDITION_NOT_MET. Once the
 * application has finished accessing the data samples it must call end_access.
 * It is not required for the application to call begin_access/end_access if the PRESENTATION QosPolicy has the
 * access_scope set to something other than ‘GROUP.’ Calling begin_access/end_access in this case is not considered an
 * error and has no effect.
 * The calls to begin_access/end_access may be nested. In that case, the application must call end_access as many times as
 * it called begin_access.
 * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
 */
ReturnCode::Type Subscriber::BeginAccess()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * Indicates that the application has finished accessing the data samples in DataReader objects managed by the Subscriber.
 * This operation must be used to ‘close’ a corresponding begin_access.
 * After calling end_access the application should no longer access any of the Data or SampleInfo elements returned from
 * the sample-accessing operations. This call must close a previous call to begin_access otherwise the operation will return
 * the error PRECONDITION_NOT_MET.
 * Possible error codes returned in addition to the standard ones: PRECONDITION_NOT_MET.
 */
ReturnCode::Type Subscriber::EndAccess()
{
    return ReturnCode::Ok;
}


/**
 * @brief
 *
 * This operation invokes the operation on_data_available on the DataReaderListener objects attached to contained
 * DataReader entities with a DATA_AVAILABLE status that is considered changed as described in Section 7.1.4.2.2,
 * “Changes in Read Communication Statuses.”
 *
 * This operation is typically invoked from the on_data_on_readers operation in the SubscriberListener. That way the
 * SubscriberListener can delegate to the DataReaderListener objects the handling of the data.
 */
ReturnCode::Type Subscriber::NotifyDataReaders()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation deletes all the entities that were created by means of the “create” operations on the Subscriber. That is, it
 * deletes all contained DataReader objects. This pattern is applied recursively. In this manner the operation
 * delete_contained_entities on the Subscriber will end up deleting all the entities recursively contained in the Subscriber,
 * that is also the QueryCondition and ReadCondition objects belonging to the contained DataReaders.
 *
 * The operation will return PRECONDITION_NOT_MET if the any of the contained entities is in a state where it cannot
 * be deleted. This will occur, for example, if a contained DataReader cannot be deleted because the application has called
 * a read or take operation and has not called the corresponding return_loan operation to return the loaned samples.
 *
 * Once delete_contained_entities returns successfully, the application may delete the Subscriber knowing that it has no
 * contained DataReader objects.
 */
ReturnCode::Type Subscriber::DeleteContainedEntities()
{
    return ReturnCode::Ok;
}

/**
 * @brief
 *
 * This operation returns the DomainParticipant to which the Subscriber belongs.
 */
DCPS::Domain::DomainParticipantPtr Subscriber::GetParticipant() const
{
    return this->state().GetParticipant();
}

/**
 * @brief This operation installs a Listener on the Entity. The listener will only be invoked on the changes of communication
 * status indicated by the specified mask.
 *
 * It is permitted to use ‘nil’ as the value of the listener. The ‘nil’ listener behaves as a Listener whose operations perform
 * no action. Only one listener can be attached to each Entity. If a listener was already set, the operation set_listener will
 * replace it with the new one. Consequently if the value ‘nil’ is passed for the listener parameter to the set_listener
 * operation, any existing listener will be removed.
 */
ReturnCode::Type Subscriber::SetListener(const SubscriberListener::Ptr &listener, const Status::StatusMask &statusMask)
{
    this->subscriberListener_ = listener;
    this->statusMask_ = statusMask;

    return ReturnCode::Ok;
}

/**
 * @brief Subscriber::GetListener This operation allows access to the existing Listener attached to the Entity.
 * @return
 */
SubscriberListener::Ptr Subscriber::GetListener()
{
    return subscriberListener_.lock();
}

/**
 * @brief Subscriber::GetInstanceHandle
 * @return
 */
InstanceHandle Subscriber::GetInstanceHandle() const
{
    return InstanceHandle::NIL();
}

/**
 * @brief Subscriber::Enable
 *
 * According spec:
 *  - Calling enable on an already enabled Entity returns OK and has no effect.
 *  - Calling enable on an Entity whose factory is not enabled will fail and return PRECONDITION_NOT_MET.
 *
 * @return
 */
ReturnCode::Type Subscriber::Enable()
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

    return DCPS::Infrastructure::Entity::Enable();
}

}} // namespace DCPS::Subscription
