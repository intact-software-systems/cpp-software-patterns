#pragma once

#include"RxCommand/IncludeExtLibs.h"

#include"RxCommand/Strategy/CompositionStrategy.h"
#include"RxCommand/Strategy/AggregateCompositionStrategy.h"
#include"RxCommand/Strategy/IndividualCompositionStrategy.h"

#include"RxCommand/Export.h"

namespace Reactor {

class DLL_STATE CommandPolicy
{
public:
    CommandPolicy();
    CommandPolicy(TaskPolicy policy, Policy::Composition composition, Policy::Criterion successCriterion);
    virtual ~CommandPolicy();

    Policy::Criterion SuccessCriterion() const;

    TaskPolicy GetTaskPolicy() const;

    template <typename Return>
    CompositionStrategy<Return>* GetCompositionStrategy()
    {
        switch(composition_.Kind())
        {
            case Policy::CompositionKind::AGGREGATE:
                return AggregateCompositionStrategy<Return>::InstancePtr();
            case Policy::CompositionKind::INDIVIDUAL:
            case Policy::CompositionKind::MAXIMUM:
            case Policy::CompositionKind::MINIMUM:
            case Policy::CompositionKind::NO:
            default:
                return IndividualCompositionStrategy<Return>::InstancePtr();
        }
    }

    static CommandPolicy Default();
    static CommandPolicy OnceAggregate(Policy::Interval retryInterval, Policy::Timeout timeout, int maxNumberAttempts);
    static CommandPolicy OnceIndividual(Policy::Interval retryInterval, Policy::Timeout timeout, int maxNumberAttempts);
    static CommandPolicy OnceImmediatelyIndividual(Policy::Timeout timeout, int maxNumberAttempts);
    static CommandPolicy ForeverIndividual(Policy::Interval interval, Policy::Interval retryInterval, Policy::Timeout timeout);

private:
    Concurrent::TaskPolicy policy_;

    Policy::UndoRedo    undoRedo_;
    Policy::Criterion   successCriterion_;
    Policy::Composition composition_;
};

}
