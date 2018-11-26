/*
 * StatusCondition.cpp
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */
#include"DCPS/Status/StatusCondition.h"
//#include"DCPS/Infrastructure/Entity.h"

namespace DCPS { namespace Status
{

/**
 * @brief
 *
 * Creates a StatusCondition object.
 *
 * TODO: Should there be a functor constructor?
 *
 * @arg entity 	Entity owns the StatusCondition
 * @arg mask	StatusMask to determine which status the Entity listens to.
 */
StatusCondition::StatusCondition(DCPS::Infrastructure::Entity *entity, const Status::StatusMask &mask)
    : BaseLib::Concurrent::GuardCondition()
    , entity_(entity)
    , statusMask_(mask)
{

}

/**
 * @brief
 *
 * Overriding default copy constructor.
 *
 * @arg StatusCondition to copy
 */
StatusCondition::StatusCondition(const StatusCondition &condition)
    : BaseLib::Concurrent::GuardCondition()
    , entity_(condition.GetEntity())
    , statusMask_(condition.GetEnabledStatuses())
{

}

/**
 * @brief
 *
 * Empty destructor
 */
StatusCondition::~StatusCondition()
{

}

/**
* @brief
*
* This operation defines the list of communication statuses that are taken into account to determine the
* trigger_value of the StatusCondition. This operation may change the trigger_value of the StatusCondition.
* WaitSet objects behavior depend on the changes of the trigger_value of their attached conditions.
* Therefore, any WaitSet to which the StatusCondition is attached is potentially affected by this operation.
* If this function is not invoked, the default list of enabled statuses includes all the statuses.
*/
ReturnCode::Type StatusCondition::SetEnabledStatuses(const Status::StatusMask &mask)
{
    statusMask_ = mask;
    return ReturnCode::Ok;
}

/**
* @brief
*
* This operation retrieves the list of communication statuses that are taken into account to determine the
* trigger_value of the StatusCondition. This operation returns the statuses that were explicitly set on the
* last call to set_enabled_statuses or, if set_enabled_statuses was never called, the default list
* (see Section 7.1.2.1.9.1).
*/
Status::StatusMask StatusCondition::GetEnabledStatuses() const
{
    return statusMask_;
}

/**
* @brief
*
* This operation returns the Entity associated with the StatusCondition. Note that there is exactly one
* Entity associated with each StatusCondition.
*/
DCPS::Infrastructure::Entity* StatusCondition::GetEntity() const
{
    return entity_;
}

/**
 * @brief
 *
 * Gives ability to set trigger value true, which results in call-back if functor is set.
 */
void StatusCondition::SetTriggerValue(bool triggerValue)
{
    GuardCondition::SetTriggerValue(triggerValue);
}

}}
