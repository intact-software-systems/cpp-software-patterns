#pragma once

#include"RxFSM/IncludeExtLibs.h"

namespace Reactor
{

/**
 * One action associated with every command
 *  - A command may hold "entry actions", "exit actions", "input action", "transition actions"
 *
 * An InputCommand is essentially the same as a StateCommand, use different typename Input vs Output
 */
template <typename Return>
class StateCommandController : public CommandControllerAction<Return>
{
public:
    StateCommandController(CommandControllerPolicy policy, RxThreadPool::Ptr threadPool)
        : CommandControllerAction<Return>(policy, threadPool)
        , condition_(std::make_shared<Templates::AlwaysTrigger<Return>>(Templates::AlwaysTrigger<Return>()))
    { }

    StateCommandController(Templates::TriggerCondition<Return>* condition,
                           CommandControllerPolicy policy,
                           RxThreadPool::Ptr threadPool)
        : CommandControllerAction<Return>(policy, threadPool)
        , condition_(condition)
    { }

    virtual ~StateCommandController()
    { }

    CLASS_TRAITS(StateCommandController)

    std::shared_ptr<Templates::TriggerCondition<Return>> Condition() const
    {
        return condition_;
    }

    void SetCondition(Templates::TriggerCondition<Return>* condition)
    {
        condition_ = std::make_shared<Templates::TriggerConditionHolder<Return>>(Templates::TriggerConditionHolder<Return>(condition));
    }

private:
    std::shared_ptr<Templates::TriggerCondition<Return>> condition_;
};

}
