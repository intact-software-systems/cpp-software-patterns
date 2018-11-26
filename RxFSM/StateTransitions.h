#pragma once

#include"RxFSM/State.h"
#include"RxFSM/States.h"

namespace Reactor
{

// ----------------------------------------------------
// StateTransitionsPolicy
// ----------------------------------------------------

class StateTransitionsPolicy
{ };

// ----------------------------------------------------
// StateTransitionsData
// ----------------------------------------------------

template <typename StateType, typename Return, typename Input>
class StateTransitionsData
{
private:
    typedef ISet<StateType>                    StateSetList;
    typedef ISet<StateType>                    AdjacencyList;
    typedef IMap<StateType, AdjacencyList>     Table;
    typedef States<StateType, Return, Input>   StateMap;

public:
    StateTransitionsData(const StateMap &stateTable, const Table &table, const StateSetList &initialStates, const StateSetList &finalStates)
        : stateTable_(stateTable)
        , transitionTable_(table)
        , initialStates_(initialStates)
        , finalStates_(finalStates)
    {}
    virtual ~StateTransitionsData()
    {}

    const AdjacencyList& TransitionsFor(StateType state) const
    {
        return getAdjacencyList(state);
    }

    Reactor::State<StateType, Return, Input> HandlerFor(StateType state) const
    {
        //if(!stateTable_.containsKey(state)) return Reactor::StateNoOp<StateType, Return, Input>();
        ASSERT(stateTable_.containsKey(state));
        return stateTable_.at(state);
    }

    const StateSetList& InitialStates() const
    {
        return initialStates_;
    }

    const StateSetList& FinalStates() const
    {
        return finalStates_;
    }

    const StateSetList& AllStates() const
    {
        return stateTable_.keyISet();
    }

    bool IsFinal(StateType state) const
    {
        return finalStates_.contains(state);
    }

    bool IsInitial(StateType state) const
    {
        return initialStates_.contains(state);
    }

    size_t Degree(StateType state) const
    {
        return getAdjacencyList(state).size();
    }

private:
    const AdjacencyList& getAdjacencyList(StateType state) const
    {
        return transitionTable_.at(state);
    }

private:
    StateMap        stateTable_;
    Table           transitionTable_;
    StateSetList    initialStates_;
    StateSetList    finalStates_;
};

// ----------------------------------------------------
// StateTransitionTable
// ----------------------------------------------------

/**
Alternative 1:
- User defines nextState function
- State machine uses nextState function (no transition rules)

Alternative 2:
- User defines transition rules
- State machine uses predefined nextState function that applies the transition rules

Create a Builder for state transitions.

Non-deterministic finite machine: multiple next is possible, and, move to a different state when given no input
*/
template <typename StateType, typename Return, typename Input>
class StateTransitionTable
        : public Templates::ContextObjectShared<StateTransitionsPolicy, StateTransitionsData<StateType, Return, Input>>
{
private:
    typedef ISet<StateType>                    StateSetList;
    typedef ISet<StateType>                    AdjacencyList;
    typedef IMap<StateType, AdjacencyList>     Table;
    typedef States<StateType, Return, Input>   StateMap;

public:
    StateTransitionTable(const StateMap &stateTable, const Table &table, const StateSetList &initialStates, const StateSetList &finalStates)
        : Templates::ContextObjectShared<StateTransitionsPolicy, StateTransitionsData<StateType, Return, Input>>
          (
              new StateTransitionsPolicy(),
              new StateTransitionsData<StateType, Return, Input>(stateTable, table, initialStates, finalStates)
          )
    {}
    virtual ~StateTransitionTable()
    {}

    const AdjacencyList& TransitionsFor(StateType state) const
    {
        return this->data()->TransitionsFor(state);
    }

    Reactor::State<StateType, Return, Input> HandlerFor(StateType state) const
    {
        return this->data()->HandlerFor(state);
    }

    const StateSetList& InitialStates() const
    {
        return this->data()->InitialStates();
    }

    const StateSetList& FinalStates() const
    {
        return this->data()->FinalStates();
    }

    const StateSetList& AllStates() const
    {
        return this->data()->AllStates();
    }

    bool IsFinal(StateType state) const
    {
        return this->data()->IsFinal(state);
    }

    bool IsInitial(StateType state) const
    {
        return this->data()->IsInitial(state);
    }

    size_t Degree(StateType state) const
    {
        return this->data()->Degree(state);
    }
};

template <typename StateType, typename Return, typename Input>
class StateTransitionTableIterator
        : public BaseLib::Templates::Iterator<Reactor::State<StateType, Return, Input>>
{
public:
    StateTransitionTableIterator(StateTransitionTable<StateType, Return, Input> table)
        : table_(table)
        , current_()
    { }
    virtual ~StateTransitionTableIterator()
    { }

    virtual bool HasNext() const
    {
        return true;
    }

    virtual Reactor::State<StateType, Return, Input> Next()
    {

    }

    virtual void Remove()
    {

    }

    virtual void Rewind()
    {

    }

private:
    StateTransitionTable<StateType, Return, Input> table_;
    StateType current_;
};

// ----------------------------------------------------
// StateTransitionsBuilder
// ----------------------------------------------------

/**
_stateMachine.Configure(RfqState.Requesting)
                .OnEntry(LogTransition)
                .OnEntryFrom(_rfqEventUserRequests, OnEntryRequesting)
                .Permit(RfqEvent.ServerNewQuote, RfqState.Quoted)
                .Permit(RfqEvent.UserCancels, RfqState.Cancelling)
                .Permit(RfqEvent.InternalError, RfqState.Error);

private void OnEntryRequesting(IQuoteRequest quoteRequest)
{
    // here goes the code to send a quote request to the server
}

_stateMachine.OnUnhandledTrigger(OnUnhandledTrigger);

private void OnUnhandledTrigger(RfqState state, RfqEvent trigger)
{
    var message = string.Format("State machine received an invalid trigger '{0}' in state '{1}'", trigger, state);
    Console.WriteLine(message);

    _rfqUpdateSubject.OnError(new ApplicationException(message));
}
*/

template <typename StateType, typename Return, typename Input>
class StateTransitionsBuilder
{
private:
    typedef ISet<StateType>                    StateSetList;
    typedef ISet<StateType>                    AdjacencyList;
    typedef IMap<StateType, AdjacencyList>     Table;
    typedef States<StateType, Return, Input>   StateMap;

public:
    StateTransitionsBuilder() {}
    virtual ~StateTransitionsBuilder() {}

    StateTransitionTable<StateType, Return, Input> Build()
    {
        return StateTransitionTable<StateType, Return, Input>(stateTable_, transitionTable_, initialStates_, finalStates_);
    }

    AdjacencyList& TransitionsFor(StateType state)
    {
        return transitionsFor(state);
    }

    bool HandlerFor(StateType state, Reactor::State<StateType, Return, Input> handler)
    {
        return stateTable_.put(state, handler);
    }

    Reactor::State<StateType, Return, Input> Configure(StateType state)
    {
        return stateTable_.at(state);
    }

    /*
    _stateMachine.Configure(RfqState.Input)
             .Permit(RfqEvent.UserRequests, RfqState.Requesting);
    _stateMachine.Configure(RfqState.Requesting)
             .Permit(RfqEvent.ServerNewQuote, RfqState.Quoted)
             .Permit(RfqEvent.UserCancels, RfqState.Cancelling)
             .Permit(RfqEvent.InternalError, RfqState.Error);
     */
    bool Permit(StateType from, StateType to, Input input)
    {
        typename StateMap::iterator it = stateTable_.find(from);
        if(it != stateTable_.end())
        {
            Reactor::State<StateType, Return, Input> handler = it->second;
            handler.InputActions()->SetCondition(new Templates::EqualityTrigger<Input>(input));

            transitionsFor(from).insert(to);
        }

        return false;
    }

    bool Permit(StateType from, StateType to, Templates::TriggerCondition<Input> *trigger)
    {
        typename StateMap::iterator it = stateTable_.find(from);
        if(it != stateTable_.end())
        {
            Reactor::State<StateType, Return, Input> handler = it->second;
            handler.InputActions()->SetCondition(trigger);

            transitionsFor(from).insert(to);
        }

        return false;
    }

    StateSetList& InitialStates()
    {
        return initialStates_;
    }

    StateSetList& FinalStates()
    {
        return finalStates_;
    }

private:
    const AdjacencyList& transitionsFor(StateType state) const
    {
        return transitionTable_.at(state);
    }

    AdjacencyList& transitionsFor(StateType state)
    {
        if(!transitionTable_.containsKey(state)) {
            transitionTable_.put(state, AdjacencyList());
        }

        return transitionTable_.at(state);
    }

private:
    StateMap       stateTable_;
    Table          transitionTable_;
    StateSetList   initialStates_;
    StateSetList   finalStates_;
};

}
