#pragma once

namespace Reactor {

/**
 * @brief The StateTransition class represents a state transition (from, to).
 *
 * The individual StateType represents a vertex and the state transition (from, to) an edge.
 * http://msdn.microsoft.com/en-us/library/system.activities.statements.transition(v=vs.110).aspx
 * TODO: I don't need FROM state because I can use this transition in class State, which stores from.
 */
//template <typename StateType>
//class Transition
//{
//public:
//    Transition(StateType to,
//               typename TransitionHandler<StateType>::Ptr handler,
//               IsTriggeredMethod::Ptr trigger = IsTriggeredMethod::Ptr(new AlwaysTrigger()))
//        : to_(to)
//        , condition_()
//        , action_(handler)
//        , trigger_(trigger)
//    {}
//    virtual ~Transition()
//    {}
//
//    // --------------------------------------
//    // Getters
//    // --------------------------------------
//
//    /**
//     * Gets or sets the next state in the state machine to execute after the transition is completed.
//     */
//    const StateType& To() const
//    {
//        return to_;
//    }
//
//    const TransitionCondition& Condition() const
//    {
//        return condition_;
//    }
//
//    typename TransitionHandler<StateType>::Ptr Action() const
//    {
//        return action_;
//    }
//
//    IsTriggeredMethod::Ptr Trigger() const
//    {
//        return trigger_;
//    }
//
//    // --------------------------------------
//    // operator comparisons
//    // --------------------------------------
//
//    bool operator==(const Transition &that)
//    {
//        return this->to_ == that.to_;
//    }
//
//    bool operator!=(const Transition &that)
//    {
//        return !(*this == that);
//    }
//
//    bool operator<(const Transition &that)
//    {
//        return this->to_ < that.to_;
//    }
//
//private:
//    StateType to_;
//    TransitionCondition condition_;
//
//    typename TransitionHandler<StateType>::Ptr action_;
//    IsTriggeredMethod::Ptr trigger_;
//};

}

#endif
