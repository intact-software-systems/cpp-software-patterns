#pragma once

#include"RxConcurrent/CommonDefines.h"

namespace BaseLib {
namespace Concurrent {


template <typename Policy, typename Status>
struct DecisionPair
{
    const Policy                  policy;
    const std::shared_ptr<Status> status;
};


template <typename Result>
struct Checker : public Templates::OperatorMethod0<Result>
{
    Checker()
    { }

    template <typename Policy, typename Status>
    Checker(std::function<Result(DecisionPair<Policy, Status>)> check, DecisionPair<Policy, Status> decision)
        : invoker(
        [check, decision]()
        {
            return check(decision);
        }
    )
    { }

    virtual Result operator()()
    {
        return invoker();
    }

    const std::function<Result()> invoker;
};


template <typename Return>
struct ActOn : public Templates::OperatorMethod0<Return>
{
    template <typename Decision, typename Data>
    ActOn(std::function<Return(Decision, Data&)> action, Checker<Decision> decision, Data& data)
        : invoker(
        [action, decision, &data]()
        {
            Decision result = decision();
            return action(result, data);
        }
    )
    { }

    virtual Return operator()()
    {
        return invoker();
    }

    const std::function<Return()> invoker;
};

struct ActOnAll
{
    template <typename Decision, typename Data>
    ActOnAll(std::function<void(Decision, Data&)> action, std::list<Checker<Decision>> decisions, Data& data)
        : invoker(
        [action, decisions, &data]()
        {
            for(auto& decision : decisions)
            {
                action(decision(), data);
            }
        }
    )
    { }

    void operator()()
    {
        invoker();
    }

    const std::function<void()> invoker;
};

/**
 * Based on notion:
 *
 * Policy + Status = decision (ruling, choice, finding)
 *
 * Decision + Data = action
 */
template <typename Decision, typename Policy, typename Status>
class OOFuncBuilder
{
public:
    OOFuncBuilder()
    { }

    virtual ~OOFuncBuilder()
    { }

    OOFuncBuilder& PairUp(Policy policy, std::shared_ptr<Status> status)
    {
        pair_ = DecisionPair<Policy,Status>{policy, status};
        return *this;
    }

    OOFuncBuilder& AndCheck(std::function<Decision(DecisionPair<Policy, Status>)> check)
    {
        checkers_.push_back(Checker<Decision>(check, pair_));
        return *this;
    }

    template <typename Data>
    ActOnAll AndAct(std::function<void(Decision, Data&)> action, Data& data)
    {
        return ActOnAll(action, checkers_, data);
    }

    template <typename Return, typename Data>
    ActOn<Return> AndActOnFront(std::function<Return(Decision, Data&)> action, Data& data)
    {
        return ActOn<Return>(action, checkers_.front(), data);
    }

private:
    DecisionPair<Policy, Status> pair_;
    std::list<Checker<Decision>> checkers_;
};

//Concurrent::OOFuncBuilder<bool, MessageWriterPolicy, MessageWriterStatus> builder;
//
//ActOnAll act = builder
//    .PairUp(this->config().delegate(), this->status().SharedPtr())
//    .AndCheck(MessageWriterAlgorithms::Check)
//    .template AndAct<MessageWriterData<T>>(MessageWriterAlgorithms::Action, this->data().delegate());
//
//act();
//


// ----------------------------------------------------------
// These are test functions
// ----------------------------------------------------------
//
//template <typename T>
//static void Action(bool doTheAction, MessageWriterData<T> &data)
//{
//    if(doTheAction) IINFO() << "I am here";
//    // return success/failure -> used to update status accordingly
//}
//
//static bool Check(MessageWriterPolicy policy, MessageWriterStatusPtr status)
//{
//    // Event type as enum, used as input to Action
//    return true;
//}
//
//static void Update(bool actionType, bool success, MessageWriterStatusPtr status)
//{
//    // switch on actionType and update status
//}

/**
 * Example:
 * - Use in a class with multiple (Status, TaskPolicy) pairs
 * - Use ScheduledTask which exposes (Status, Policy)?
 *
 * This is a Schedulables class with pairs of (Policy, Status) to find a decision T
 */
template <typename T, typename Task>
class Schedulables
    : public Templates::Schedulable<T>
    , public Collection::details::StdListCollectionType<Task>
{
public:
    Schedulables(
        std::function<T(const std::list<Task>&)> next,
        std::function<bool(const std::list<Task>&)> hasNext
    )
        : next_(next)
        , hasNext_(hasNext)
    { }

    virtual ~Schedulables()
    { }

    virtual void run() {}

    virtual bool HasNext() const
    {
        return hasNext_(this->collection());
    }

    virtual T Next()
    {
        return next_(this->collection());
    }

private:
    std::function<T(const std::list<Task>&)>    next_;
    std::function<bool(const std::list<Task>&)> hasNext_;
};

// ---------------------------------------------------------------
// TODO: Use a holder for Pair(TaskPolicy, ExecutionStatus), create a list of these and use Schedulables<int, Pair>
// DecisionPair can also be (TaskPolicy, ScheduledFuture (containing status and updated by thread pool))
// ---------------------------------------------------------------
//Schedulables<int, DecisionPair<TaskPolicy, Status::ExecutionStatus>> schedulables(Strategy::PolicyChecker::ReadyInMs, Strategy::PolicyChecker::IsDone);
//schedulables.Add(DecisionPair<TaskPolicy, Status::ExecutionStatus>{TaskPolicy::Default(), std::make_shared<Status::ExecutionStatus>()});

}
}
