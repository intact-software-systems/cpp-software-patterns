/*
 * Entity.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#include"DCPS/Infrastructure/Entity.h"
#include"DCPS/Status/StatusCondition.h"
#include"DCPS/Domain/DomainParticipant.h"

namespace DCPS { namespace Infrastructure
{

volatile unsigned int Entity::entityID_ = 0;

/**
 * @brief Entity::Entity is the abstract base class for all the DCPS objects that support QoS policies,
 * a listener and a status condition.
 *
 * @param enabled
 */
Entity::Entity(bool enabled)
    : enabled_(enabled)
    , statusCondition_(NULL)
    , statusMask_()
{
    statusCondition_ = new Status::StatusCondition(this, statusMask_);
}

/**
 * @brief Entity::Entity
 * @param mask
 * @param enabled
 */
Entity::Entity(const Status::StatusMask &mask, bool enabled)
    : enabled_(enabled)
    , statusCondition_(NULL)
    , statusMask_(mask)
{
    statusCondition_ = new Status::StatusCondition(this, statusMask_);
}

/**
 * @brief Entity::~Entity
 */
Entity::~Entity()
{
    if(statusCondition_)
    {
        delete statusCondition_;
        statusCondition_ = NULL;
    }
}

/**
 * @brief
 *
 * This operation allows access to the StatusCondition (Section 7.1.2.1.9, StatusCondition Class) associated with the
 * Entity. The returned condition can then be added to a WaitSet (Section 7.1.2.1.6, WaitSet Class) so that the application
 * can wait for specific status changes that affect the Entity.
 */
Status::StatusCondition* Entity::GetStatusCondition()
{
    return this->statusCondition_;
}

/**
 * @brief Entity::NotifyStatusCondition
 */
void Entity::NotifyStatusCondition()
{
    this->statusCondition_->SignalAll();
}

/**
 * @brief
 *
 * This operation retrieves the list of communication statuses in the Entity that are triggered. That is, the list of statuses
 * whose value has changed since the last time the application read the status. The precise definition of the triggered state
 * of communication statuses is given in Section 7.1.4.2, Changes in Status, on page 126.
 *
 * When the entity is first created or if the entity is not enabled, all communication statuses are in the untriggered state so
 * the list returned by the get_status_changes operation will be empty.
 *
 * The list of statuses returned by the get_status_changes operation refers to the statuses that are triggered on the Entity
 * itself and does not include statuses that apply to contained entities.
 */
Status::StatusMask Entity::GetStatusChanges() const
{
    return this->statusChanges_;
}

/**
 * @brief
 *
 * This operation enables the Entity. Entity objects can be created either enabled or disabled. This is controlled by the value
 * of the ENTITY_FACTORY Qos policy (Section 7.1.3.20, ENTITY_FACTORY) on the corresponding factory for the
 * Entity.
 * The default setting of ENTITY_FACTORY is such that, by default, it is not necessary to explicitly call enable on newly
 * created entities (see Section 7.1.3.20, ENTITY_FACTORY).
 * The enable operation is idempotent. Calling enable on an already enabled Entity returns OK and has no effect.
 * If an Entity has not yet been enabled, the following kinds of operations may be invoked on it:
 * - Operations to set or get an Entity's QoS policies (including default QoS policies) and listener
 * - get_statuscondition
 * - 'factory' operations
 * - get_status_changes and other get status operations (although the status of a disabled entity never changes)
 * - 'lookup' operations
 * Other operations may explicitly state that they may be called on disabled entities; those that do not will return the error
 * NOT_ENABLED.
 * It is legal to delete an Entity that has not been enabled by calling the proper operation on its factory.
 * Entities created from a factory that is disabled, are created disabled regardless of the setting of the ENTITY_FACTORY
 * Qos policy.
 * Calling enable on an Entity whose factory is not enabled will fail and return PRECONDITION_NOT_MET.
 * If the ENTITY_FACTORY Qos policy has autoenable_created_entities set to TRUE, the enable operation on the factory
 * will automatically enable all entities created from the factory.
 * The Listeners associated with an entity are not called until the entity is enabled. Conditions associated with an entity that
 * is not enabled
 */
ReturnCode::Type Entity::Enable()
{
    if(enabled_) return ReturnCode::Ok;

    enabled_ = true;

    return ReturnCode::Ok;
}

/**
 * @brief Entity::IsEnabled
 * @return
 */
bool Entity::IsEnabled() const
{
    return enabled_;
}

/**
 * @brief
 *
 * This operation returns the InstanceHandle_t that represents the Entity.
 */
InstanceHandle Entity::GetInstanceHandle() const
{
    return InstanceHandle::NIL();
}

/**
 * @brief
 *
 * Set the status in StatusMask as changed or not-changed according to flag.
 */
void Entity::SetStatusChangedFlag(DCPS::Status::StatusMask &mask, bool isChanged)
{
    // TODO: Finish implementation
    // TODO: Mark initialized statuses in mask as changed/not-changed statusChanges_
}

/**
 * @brief
 *
 * According spec:
 * - Calling enable on an already enabled Entity returns OK and has no effect.
 * - Calling enable on an Entity whose factory is not enabled will fail and
 * return PRECONDITION_NOT_MET.
 */
ReturnCode::Type Entity::EnableParticipant(Domain::DomainParticipantPtr participantPtr)
{
    if(IsEnabled())
        return ReturnCode::Ok;

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

/*------------------------------------------------------------------------
    TODO: Implement logic
------------------------------------------------------------------------*/
void Entity::Close()
{
    // TODO: Implement
}

void Entity::Retain()
{
    // TODO: Implement
}

}} // namespace DCPS::Infrastructure

