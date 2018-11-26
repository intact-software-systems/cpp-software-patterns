#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Export.h"

namespace Reactor
{

class ComputationStrategy;

/**
 * Notification policy on errors?
 * MaxLimit on number of attached commands?
 *
 * Policy on "Maximum number of concurrently executing commands in one controller".
 * Policy on "Maximum time to finish all commands before controller stops.".
 *
 * TODO: Support "long running asynchronous tasks without blocking rx threads".
 *
 * TODO: Control "throughput" by setting an Interval policy on the executor itself.
 *      - executor can control the rate of execution
 *      - command intervals are controlled by executor making it easier to control "batch of commands with identical intervals".
 *      - provides a flow-control mechanism
 *
 * TODO: Attempt controls "Minimum number of successful executions of Commands as a group, and maximum attempts before give up"
 * TODO: Interval "Time in-between execution of CommandGroup"
 * TODO: Introduce a CommandGroupPolicy?
 */
class DLL_STATE CommandControllerPolicy
{
public:
    CommandControllerPolicy();
    CommandControllerPolicy(
            Policy::Attempt lifetime,
            Policy::Interval interval,
            Policy::UndoRedo undoRedo,
            Policy::Computation computation,
            Policy::Composition composition,
            Policy::Ordering ordering);

    CommandControllerPolicy(Policy::Computation computation, Policy::Ordering ordering, Policy::Timeout timeout, Policy::Criterion criterion, Policy::MaxLimit<int> limit);
    virtual ~CommandControllerPolicy();

    Policy::Attempt      GetAttempt() const;
    Policy::Interval     GetInterval() const;
    Policy::UndoRedo     UndoRedo() const;
    Policy::Computation  Computation() const;
    Policy::Composition  Composition() const;
    Policy::Ordering     Ordering() const;
    Policy::Timeout      Timeout() const;


    Policy::MaxLimit<int>       MaxConcurrentCommands() const;

    ComputationStrategy* findReadyCommands();

    static CommandControllerPolicy Default();
    static CommandControllerPolicy SequentialAnd(Policy::Ordering ordering, Policy::Timeout timeout);
    static CommandControllerPolicy ParallelAnd(Policy::Ordering ordering, Policy::Timeout timeout);
    static CommandControllerPolicy ConcurrentAnd(Policy::Ordering ordering, Policy::Timeout timeout);


private:
    Policy::Attempt   lifetime_;
    Policy::Interval   interval_;
    Policy::UndoRedo   undoRedo_;
    Policy::Computation computation_;
    Policy::Composition composition_;
    Policy::Ordering   ordering_;

    Policy::Timeout         timeout_;
    Policy::Criterion       successCriterion_;
    Policy::MaxLimit<int>   maxConcurrentCommands_;
};

}
