#pragma once

#include"RxFSM/CommonDefines.h"
#include"RxFSM/StateTransitions.h"
#include"RxFSM/State.h"
#include"RxFSM/StateAction.h"
#include"RxFSM/Export.h"

namespace Reactor
{

// ----------------------------------------------------
// StateMachineControllerBase
// ----------------------------------------------------

class DLL_STATE StateMachineControllerBase
    : public Templates::ExecuteMethod0<bool>
    , public Templates::ShutdownMethod
    , public Templates::TimeoutMethod
    , public Templates::BlockMethod
    , public Templates::UnblockMethod
    , public Templates::IsBlockedMethod
    , public Templates::ResumeMethod
    , public Templates::SuspendMethod
    , public Templates::TriggerMethod
    , public Templates::ResetMethod<bool>
    , public Templates::TimeoutInMethod<int>
    , public Templates::IsTimeoutMethod
    , public Templates::IsExecutingMethod
    , public Templates::IsSuspendedMethod
    , public Templates::IsPolicyViolatedMethod
{
public:
    virtual ~StateMachineControllerBase()
    { }

    CLASS_TRAITS(StateMachineControllerBase)
};

// ----------------------------------------------------
// StateMachineControllerPolicy
// ----------------------------------------------------
class StateMachineControllerStatus
{
public:
    StateMachineControllerStatus()
        : stateMachineStatus_(Duration::FromMinutes(5))
    {}

    virtual ~StateMachineControllerStatus()
    { }

    const Status::ExecutionStatus& executionStatus() const
    {
        return executionStatus_;
    }

    Status::ExecutionStatus& executionStatus()
    {
        return executionStatus_;
    }

    const Status::EventStatus& eventStatus() const
    {
        return eventStatus_;
    }

    Status::EventStatus& eventStatus()
    {
        return eventStatus_;
    }

    StateMachineStatus& machineStatus()
    {
        return stateMachineStatus_;
    }

    const StateMachineStatus& machineStatus() const
    {
        return stateMachineStatus_;
    }

private:
    Status::EventStatus     eventStatus_;
    Status::ExecutionStatus executionStatus_;
    StateMachineStatus      stateMachineStatus_;
};

// ----------------------------------------------------
// StateMachineControllerPolicy
// ----------------------------------------------------

class DLL_STATE StateMachineControllerPolicy
{
public:
    StateMachineControllerPolicy(CommandControllerPolicy policy = CommandControllerPolicy::Default(),
                                 FlowControllerPolicy flowControllerPolicy = FlowControllerPolicy::Default())
        : policy_(policy)
        , flowPolicy_(flowControllerPolicy)
    { }

    virtual ~StateMachineControllerPolicy()
    { }

    const CommandControllerPolicy& ControllerPolicy() const
    {
        return policy_;
    }

    const FlowControllerPolicy& FlowPolicy() const
    {
        return flowPolicy_;
    }

private:
    // Policy in scheduling and timing of scheduling of state actions after trigger.

    CommandControllerPolicy policy_;
    FlowControllerPolicy    flowPolicy_;
};

// ----------------------------------------------------
// StateMachineControllerData
// ----------------------------------------------------

template <typename StateType, typename Context, typename Return, typename Input>
class StateMachineControllerData
{
public:
    StateMachineControllerData(const Context& context, const StateTransitionTable<StateType, Return, Input>& table)
        : context_(context)
        , transitionTable_(table)
        , iterator_(table)
    { }

    virtual ~StateMachineControllerData()
    { }

    typename FlowController<CommandBase::Ptr>::Ptr& Queue() const
    {
        return triggerQueue_;
    }

    const StateTransitionTable<StateType, Return, Input>& Table() const
    {
        return transitionTable_;
    }

    const Context& context() const
    {
        return context_;
    }

    Context& context()
    {
        return context_;
    }

    StateMachineDescription Description() const
    {
        return description_;
    }

    StateTransitionTableIterator<StateType, Return, Input> const& iterator() const
    {
        return iterator_;
    }

    StateTransitionTableIterator<StateType, Return, Input>& iterator()
    {
        return iterator_;
    }

    StateType currentState() const
    {
        return StateType();
    }

private:
    Context context_;

    StateTransitionTable<StateType, Return, Input>         transitionTable_;
    StateTransitionTableIterator<StateType, Return, Input> iterator_;

    typename FlowController<CommandBase::Ptr>::Ptr triggerQueue_;

    StateMachineDescription description_;

    // TODO: Current state if applicable
    // if no current state is stored, then a transition has to contain information from - to
    // if current state is stored, then a transition only requires information of state to
};

// ----------------------------------------------------
// StateMachineController
// ----------------------------------------------------

/*
One instance of state machine controller can represent a "Key" in my selector example.
I need a Next(Return value), alternative, Next(StateType startState, Return value)

See AsyncScheduler for initial implementation.

Identify Start state(s) and Final state(s). Final state(s) may be accepting, rejecting.

Alternative 1:
    - User defines nextState function
    - State machine uses nextState function (no transition rules)
Alternative 2:
    - User defines transition rules
    - State machine uses predifined nextState function that applies the transition rules

State machines depends on input:
    - Define an input source for the entire state machine
    - Define input sources for every action
    - Use return values from every state

    typename State, typename Input, typename Output

    StateCommand<State, Output> cmd;
    Action1<Output, Input> action;

    State = StateType
    Output = Return
    Input = Event (Arg1)

State machines stores "current state".

Sequential: Use current to find next, execute next, set next as current.
Parallel:
TODO: What is logical for parallel? Execute from start-state?

Add StateMachineStatus

One execution may for example be [StartState -> FinalState] then write output to cache.

TODO: How to add and execute "input actions"?
TODO: Focus on supporting sequential. Parallel can come later and is perhaps solved otherwise.

TODO: Input == Event
TODO: FlowController for Input events. Input may be called events.
TODO: FlowController<Event>, see Observer implementation.
TODO: StateMachineController for input events can be similar to a subject

TODO: FlowController for Return. Are these events? They are used in transition condition to transition between states without input event.

TODO: What is logical for parallel? Execute from start-state?

Sequential machine execution:
- Execute any one state with input only in sequence (not in parallel)
- Execute individual states with input in sequence

Parallel machine execution (many current):
- Execute any one state with input only in sequence (not in parallel)
- Execute individual states with input in parallel

Embarrassingly parallel machine execution (many current, also duplicates of current):
- Execute any one state with input in parallel
- Execute individual states with input in parallel
*/
template <typename StateType, typename Return, typename Input, typename Context>
class StateMachineController
    : public StateMachineControllerBase
    , public Templates::Action0<bool>
    //, public Templates::IsNextMethod1<bool, StateType>
    , public RxObserverNew<Input>
    , public Templates::ContextObjectShared<StateMachineControllerPolicy, StateMachineControllerData<StateType, Context, Return, Input>, StateMachineControllerStatus, StateType>
    , public std::enable_shared_from_this<StateMachineController<StateType, Return, Input, Context>>
{
public:
    typedef ISet<StateType>                          AdjacencyList;
    typedef Reactor::State<StateType, Return, Input> StateHandler;

public:
    StateMachineController(const Context& context, const StateTransitionTable<StateType, Return, Input>& table)
        : Templates::ContextObjectShared<StateMachineControllerPolicy, StateMachineControllerData<StateType, Context, Return, Input>, StateMachineControllerStatus, StateType>
        (
            new StateMachineControllerPolicy(),
            new StateMachineControllerData<StateType, Context, Return, Input>(context, table),
            new StateMachineControllerStatus(),
            new StateType()
        )
    { }
    virtual ~StateMachineController()
    { }

    virtual bool Execute()
    {
        // ----------------------------------------------------
        // Execute from start state
        //  - given only one start state and not executing
        // ----------------------------------------------------
        Reactor::State<StateType, Return, Input> state = this->data()->iterator().Next();

        typename StateCommandController<Return>::Ptr controller = state.EntryActions();
        controller->RxSubject().state().Subscribe(this->shared_from_this());
        controller->Execute();

        this->status()->executionStatus().Start();

        return true;
    }

    // ---------------------------------------------------------
    // Action0 interface
    // TODO: Split subject base functionality and this action, which is basically a reactor
    // ---------------------------------------------------------

    virtual bool Invoke()
    {
//        if(this->IsBlocked()) return false;
//
//        Events events = this->data()->Queue()->Pull();
//        if(events.empty()) return false;
//
//        StateType state = this->data()->currentState();
//        AdjacencyList nextForCurr = this->data()->Table().TransitionsFor(state);
//        return Submit(events);

        return true;
    }

    virtual bool Cancel()
    {
        return true;
    }

    virtual bool IsDone() const
    {
        return true;
    }

    virtual bool IsSuccess() const
    {
        return true;
    }

    // ----------------------------------------------------
    // Reactor functions (key,value) - input from sources
    // ----------------------------------------------------

    // Similar to Subject1::Next(Source source, Arg1 arg1)
    // This is a dispatch function (play events to "observers", where observers are neighbors and only conditionally gets executed.
    virtual IList<StateType> Next(StateType state, Input value)
    {
        AdjacencyList nextForCurr = this->data()->Table().TransitionsFor(state);

        for(StateType neighbor : nextForCurr)
        {
            StateHandler handler = this->data()->Table().HandlerFor(neighbor);

            handler.InputActions()->Condition()->UpdateTrigger(value);

            if(handler.InputActions()->Condition()->IsTriggered())
            {
                handler.InputActions()->Execute();
            }
        }

        return CollectionUtils::EmptyIList<StateType>();
    }

    // ---------------------------------------------------------
    // Next event functions like subject1
    // TODO: Inherit from Subject instead and copy Subject1 functions?
    // ---------------------------------------------------------

    // Similar to Subject1::Next(Source source, Arg1 arg1)
    template <typename Source>
    Concurrent::Event::Ptr Next(StateType state, Input value, Source* source)
    {
        typename Concurrent::Event1<Input>::Ptr event1(new Concurrent::Event1<Input>(state, value));
        Concurrent::Event::Ptr                  event(new Concurrent::Event(event1, state, source));

        this->data()->Queue()->Next(event);

        return event;
    }

    // ----------------------------------------------------
    // Reactor functions (value) - input from sources
    // ----------------------------------------------------

    virtual IList<StateType> Next(Input value)
    {
        // TODO: Use current state of state machine to find and execute next.
        AdjacencyList nextForCurr = this->data()->Table().TransitionsFor(value);

        //nextForCurr.Condition().IsTriggered(value);

        return CollectionUtils::EmptyIList<StateType>();
    }

    virtual void Complete()
    {

    }

    virtual void Error(BaseLib::Exception )
    {

    }

    // ----------------------------------------------------
    // Check status functions
    // ----------------------------------------------------

    virtual bool IsNext(StateType ) const
    {
        return false;
    }

    virtual bool Block()
    {
        return true;
    }

    virtual bool Unblock()
    {
        return true;
    }

    virtual bool IsBlocked() const
    {
        return true;
    }

    // -----------------------------------------------------------
    // Handler monitoring
    // -----------------------------------------------------------

//    virtual bool OnEntry(StateMachineAction *handler)
//    {
//        // callback from handlers
//        //handler->SetContext(machineContext);
//        return true;
//    }
//
//    virtual bool OnExit(StateMachineAction *handler)
//    {
//        // callback from handlers
//        //handler->UnsetContext();
//        return true;
//    }

    // -----------------------------------------------------------
    // Interface CommandObserver <T>
    // -----------------------------------------------------------

//    virtual bool OnComplete(Command<Return> *command)
//    {
//        return true;
//    }

//    virtual bool OnNext(Command<Return> *command, Return value)
//    {
//        // command represents the state, value represents input
//        // StateTransition<state, Rules<Return, State>>

//        // Use value to identify next state(s) to execute, or use context object?
//        // identify next handler in HandlerChain
//        // execute next handler with value as input
//        //      - create a command with handler and value as input
//        //      - feed to trigger queue of commands

//        return true;
//    }

//    virtual bool OnError(Command<Return> *command, BaseLib::GeneralException exception)
//    {
//        return true;
//    }

    // -----------------------------------------------------------
    // Interface RxObserver - Values from machine input source(s)
    // -----------------------------------------------------------

    virtual bool OnComplete()
    {
        return true;
    }

    virtual bool OnNext(Input value)
    {
        IINFO() << "Next value " << value;

        // State machines depends on input:
        //    - Define an input source for the entire state machine
        //    - Define input sources for every action
        //    - Use return values from every state

        // command represents the state, value represents input
        // StateTransition<state, Rules<Return, State>>

        // Use value to identify next state(s) to execute, or use context object?
        // identify next handler in HandlerChain
        // execute next handler with value as input
        //      - create a command with handler and value as input
        //      - feed to trigger queue of commands

        return true;
    }

    virtual bool OnError(BaseLib::GeneralException )
    {
        return true;
    }

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    void Initialize()
    {
//        bool isAttached = triggerQueue_->Set(this->shared_from_this());
//        if(isAttached)
//        {
//            status_.Activate();
//        }
    }

    bool IsInitialized() const
    {
        return false; //this->status()->IsActive();
    }

private:

    void executeManyStartStates()
    {
        for(auto startState : this->data()->Table().InitialStates())
        {
            // ----------------------------------------------------
            // Execute start state input actions
            // ----------------------------------------------------
            // AdjacencyList nextForCurr = this->data()->Table().TransitionsFor(startState);
            Reactor::State<StateType, Return, Input> state = this->data()->iterator().Next();

            typename StateCommandController<Return>::Ptr controller = state.EntryActions();
            controller->RxSubject().state().Subscribe(this->shared_from_this());
            controller->Execute();
        }
    }
};

}