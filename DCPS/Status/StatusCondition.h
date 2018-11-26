/*
 * StatusCondition.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_StatusCondition_h_IsIncluded
#define DCPS_StatusCondition_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/Status/State.h"
#include"DCPS/Export.h"

namespace DCPS { namespace Infrastructure {

class Entity;

}}

namespace DCPS { namespace Status
{

/**
 * @brief
 *
 * A StatusCondition object is a specific Condition that is associated with each Entity.
 *
 * The trigger_value of the StatusCondition depends on the communication status of that entity (e.g., arrival of
 * data, loss of information, etc.), filtered by the set of enabled_statuses on the StatusCondition.
 * The enabled_statuses and its relation to Listener and WaitSet is detailed in Trigger State of the StatusCondition.
 */
class DLL_STATE StatusCondition : public BaseLib::Concurrent::GuardCondition
{
public:
    StatusCondition(DCPS::Infrastructure::Entity *entity, const Status::StatusMask &mask = Status::StatusMask::All());
    StatusCondition(const StatusCondition &condition);

    virtual ~StatusCondition();

public:

    /**
    * @brief
    *
    * This operation defines the list of communication statuses that are taken into account to determine the
    * trigger_value of the StatusCondition. This operation may change the trigger_value of the StatusCondition.
    * WaitSet objects behavior depend on the changes of the trigger_value of their attached conditions.
    * Therefore, any WaitSet to which the StatusCondition is attached is potentially affected by this operation.
    * If this function is not invoked, the default list of enabled statuses includes all the statuses.
    */
    ReturnCode::Type SetEnabledStatuses(const Status::StatusMask &mask);

    /**
    * @brief
    *
    * This operation retrieves the list of communication statuses that are taken into account to determine the
    * trigger_value of the StatusCondition. This operation returns the statuses that were explicitly set on the
    * last call to set_enabled_statuses or, if set_enabled_statuses was never called, the default list
    * (see Section 7.1.2.1.9.1).
    */
    Status::StatusMask GetEnabledStatuses() const;

    /**
    * @brief
    *
    * This operation returns the Entity associated with the StatusCondition. Note that there is exactly one
    * Entity associated with each StatusCondition.
    */
    DCPS::Infrastructure::Entity* GetEntity() const;

    /**
    * @brief
    *
    * The trigger_value of a StatusCondition is the Boolean OR of the ChangedStatusFlag of all the communication statuses
    * to which it is sensitive. That is, trigger_value==FALSE only if all the values of the ChangedStatusFlags are FALSE.
    * The sensitivity of the StatusCondition to a particular communication status is controlled by the list of enabled_statuses
    * set on the condition by means of the set_enabled_statuses operation.
    */
    virtual void SetTriggerValue(bool triggerValue);

private:
    DCPS::Infrastructure::Entity    *entity_;
    Status::StatusMask              statusMask_;
};

}}

#endif
