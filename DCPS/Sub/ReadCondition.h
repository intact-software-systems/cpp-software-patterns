/*
 * ReadCondition.h
 *
 *  Created on: 29. juni 2012
 *      Author: KVik
 */

#ifndef DCPS_ReadCondition_h_IsIncluded
#define DCPS_ReadCondition_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"
#include"DCPS/Status/DataStatus.h"
#include"DCPS/Sub/SubFwd.h"

namespace DCPS { namespace Subscription
{

/**
 * @brief
 *
 * ReadCondition objects are conditions specifically dedicated to read operations and
 * attached to one DataReader.
 *
 * ReadCondition objects allow an application to specify the data samples it is interested
 * in (by specifying the desired sample-states, view-states, and instance-states). See
 * the parameter definitions for DataReaders read/take operations.) This allows the middleware
 * to enable the condition only when suitable information is available24. They are to be used
 * in conjunction with a WaitSet as normal conditions. More than one ReadCondition may be
 * attached to the same DataReader.
 *
 * TODO: This and QueryCondition are only used by the Subscriber/DataReader. Move there?
 */
template <typename DATA>
class ReadCondition : public BaseLib::Concurrent::GuardCondition
{
public:
    // -------------------------------------------------------
    // Basic constructor without call-back functor
    // -------------------------------------------------------
    ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status);

    // -------------------------------------------------------
    // Sets Functor and argument to be invoked (if any)
    // -------------------------------------------------------
    template <typename FUN>
    ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status, const FUN& functor);

    template <typename FUN, typename ARG>
    ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status, const FUN& functor, const ARG &arg);

    virtual ~ReadCondition()
    { }

    CLASS_TRAITS(ReadCondition)

public:
    /**
     * @brief
     *
     * This operation returns the DataReader associated with the ReadCondition.
     * Note that there is exactly one DataReader asssociated with each ReadCondition.
     */
    typename DataReader<DATA>::Ptr GetDataReader() const;

    /**
     * @brief
     *
     * This operation returns the set of sample-states that are taken into account to determine
     * the trigger_value of the ReadCondition. These are the sample-states specified when the
     * ReadCondition was created.
     */
    Status::SampleState GetSampleStateMask() const;

    /**
     * @brief
     *
     * This operation returns the set of view-states that are taken into account to determine the
     * trigger_value of the ReadCondition. These are the view-states specified when the
     * ReadCondition was created.
     */
    Status::ViewState GetViewStateMask() const;

    /**
     * @brief
     *
     * This operation returns the set of instance-states that are taken into account to determine
     * the trigger_value of the ReadCondition. These are the instance-states specified when the
     * ReadCondition was created.
     */
    Status::InstanceState GetInstanceStateMask() const;

    /**
     * @brief
     *
     * This operation returns the container class for SampleState, ViewState and InstanceState.
     */
    Status::DataState	GetDataStateMask() const;

    /**
     * @brief
     *
     * Similar to the StatusCondition, a ReadCondition also has a trigger_value that determines whether the attached WaitSet is
     * BLOCKED or UNBLOCKED. However, unlike the StatusCondition, the trigger_value of the ReadCondition is tied to
     * the presence of at least a sample managed by the Service with SampleState, ViewState, and InstanceState matching those
     * of the ReadCondition. Furthermore, for the QueryCondition to have a trigger_value==TRUE, the data associated with
     * the sample must be such that the query_expression evaluates to TRUE.
     *
     * The fact that the trigger_value of a ReadCondition is dependent on the presence of samples on the associated DataReader
     * implies that a single take operation can potentially change the trigger_value of several ReadCondition or
     * QueryCondition conditions. For example, if all samples are taken, any ReadCondition and QueryCondition conditions
     * associated with the DataReader that had their trigger_value==TRUE before will see the trigger_value change to FALSE.
     * Note that this does not guarantee that WaitSet objects that were separately attached to those conditions will not be woken
     * up. Once we have trigger_value==TRUE on a condition it may wake up the attached WaitSet, the condition transitioning
     * to trigger_value==FALSE does not necessarily unwakeup the WaitSet as unwakening may not be possible in general.
     * The consequence is that an application blocked on a WaitSet may return from the wait with a list of conditions some of
     * which are no longer active. This is unavoidable if multiple threads are concurrently waiting on separate WaitSet objects
     * and taking data associated with the same DataReader entity.
     *
     * To elaborate further, consider the following example: A ReadCondition that has a sample_state_mask = {NOT_READ}
     * will have trigger_value of TRUE whenever a new sample arrives and will transition to FALSE as soon as all the newlyarrived
     * samples are either read (so their status changes to READ) or taken (so they are no longer managed by the
     * Service). However if the same ReadCondition had a sample_state_mask = {READ, NOT_READ}, then the trigger_value
     * would only become FALSE once all the newly-arrived samples are taken (it is not sufficient to read them as that would
     * only change the SampleState to READ which overlaps the mask on the ReadCondition.
     */
     virtual void SetTriggerValue(bool triggerValue);

private:
    typename DataReader<DATA>::WeakPtr	dataReader_;
    Status::DataState	                dataStatus_;
};

/*----------------------------------------------------
 ReadCondition template implementations
----------------------------------------------------*/
template <typename DATA>
ReadCondition<DATA>::ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status)
    : BaseLib::Concurrent::GuardCondition()
    , dataReader_(dataReader)
    , dataStatus_(status)
{

}

template <typename DATA>
template <typename FUN>
ReadCondition<DATA>::ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status, const FUN& functor)
    : BaseLib::Concurrent::GuardCondition(functor)
    , dataReader_(dataReader)
    , dataStatus_(status)
{

}

template <typename DATA>
template <typename FUN, typename ARG>
ReadCondition<DATA>::ReadCondition(const DataReader<DATA>& dataReader, const Status::DataState& status, const FUN& functor, const ARG &arg)
    : BaseLib::Concurrent::GuardCondition(functor, arg)
    , dataReader_(dataReader)
    , dataStatus_(status)
{

}

/**
 * @brief
 *
 * This operation returns the DataReader associated with the ReadCondition.
 * Note that there is exactly one DataReader asssociated with each ReadCondition.
 */
template <typename DATA>
typename DataReader<DATA>::Ptr ReadCondition<DATA>::GetDataReader() const
{
    return this->dataReader_.lock();
}

/**
 * @brief
 *
 * This operation returns the set of sample-states that are taken into account to determine
 * the trigger_value of the ReadCondition. These are the sample-states specified when the
 * ReadCondition was created.
 */
template <typename DATA>
Status::SampleState ReadCondition<DATA>::GetSampleStateMask() const
{
    return this->dataStatus_.GetSampleState();
}

/**
 * @brief
 *
 * This operation returns the set of view-states that are taken into account to determine the
 * trigger_value of the ReadCondition. These are the view-states specified when the
 * ReadCondition was created.
 */
template <typename DATA>
Status::ViewState ReadCondition<DATA>::GetViewStateMask() const
{
    return this->dataStatus_.GetViewState();
}

/**
 * @brief
 *
 * This operation returns the set of instance-states that are taken into account to determine
 * the trigger_value of the ReadCondition. These are the instance-states specified when the
 * ReadCondition was created.
 */
template <typename DATA>
Status::InstanceState ReadCondition<DATA>::GetInstanceStateMask() const
{
    return this->dataStatus_.GetInstanceState();
}

/**
 * @brief
 *
 * This operation returns the container class for SampleState, ViewState and InstanceState.
 */
template <typename DATA>
Status::DataState ReadCondition<DATA>::GetDataStateMask() const
{
    return this->dataStatus_;
}

/**
 * @brief
 *
 * Gives ability to set trigger value true, which results in call-back if functor is set.
 */
template <typename DATA>
void ReadCondition<DATA>::SetTriggerValue(bool triggerValue)
{
    GuardCondition::SetTriggerValue(triggerValue);
}

}}

#endif
