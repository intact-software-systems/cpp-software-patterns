#pragma once

#include"DataReactor/IncludeExtLibs.h"
#include"DataReactor/Handler/RxHandler.h"

namespace Reactor
{

//template <typename StateType>
//class HandlerTuple
//{
//public:
//    HandlerTuple(StateType state, typename RxHandler<StateType>::Ptr handler)
//        : state_(state)
//        , handler_(handler)
//    {}
//    virtual ~HandlerTuple()
//    {}
//    const StateType& State() const
//    {
//        return state_;
//    }
//    typename RxHandler<StateType>::Ptr Handler()
//    {
//        return handler_;
//    }
//private:
//    StateType state_;
//    typename RxHandler<StateType>::Ptr handler_;
//};


//template <typename StateType>
//class StateMachineChain
//        : public Templates::CopyMethod0< SHARED_PTR_T1(StateMachineChain, StateType) >
//{
//public:
//    typedef Collection::IMap<StateType, State<StateType> > StateList;

//public:
//    StateMachineChain()
//    {}
//    virtual ~StateMachineChain()
//    {}

//    CLASS_TRAITS(StateMachineChain)

//    bool AddState(const State<StateType> &state)
//    {
//        return states_.put(state.Vertex(), state);
//    }

//    bool RemoveState(const State<StateType> &state)
//    {
//        return states_.erase(state.Vertex()) == 1;
//    }

//    bool RemoveState(StateType stateType)
//    {
//        return states_.erase(stateType) == 1;
//    }

//    StateList& States()
//    {
//        return states_;
//    }

//    virtual typename StateMachineChain<StateType>::Ptr Copy() const
//    {
//        // TODO: copy the state machine
//        ASSERT(0);

//        return StateMachineChain<StateType>::Ptr();
//    }

//private:
//    StateList states_;
//};


/**
 * @brief The RxHandlerChain class
 *
 * Pass the events through OnNext -> OnNext -> OnNext
 */
template <typename StateType>
class RxHandlerChain
        : public Templates::CopyMethod0<std::shared_ptr<RxHandlerChain<StateType>>>
{
public:
    typedef StateVertex<StateType, typename TransitionHandler<StateType>::Ptr >  HandlerVertex;
    typedef typename Collection::IMap<StateType, HandlerVertex> HandlerMap;

public:
    RxHandlerChain()
    {}
    virtual ~RxHandlerChain()
    {}

    CLASS_TRAITS(RxHandlerChain)

    bool AddNeighbor(StateType vertex, StateType neighbor)
    {
        if(handlerChain_.containsKey(vertex)) return false;

        typename HandlerMap::iterator it = handlerChain_.find(vertex);

        return (it->second).AddNeighbor(neighbor);
    }

    bool AddHandler(StateType state, typename TransitionHandler<StateType>::Ptr handler)
    {
        if(handlerChain_.containsKey(state)) return false;

        return handlerChain_.put(state, HandlerVertex(state, handler));
    }

    bool RemoveHandler(StateType state)
    {
        return handlerChain_.erase(state) == 1;
    }

    ISet<StateType> Neighbors(StateType state)
    {
        if(handlerChain_.containsKey(state))
        {
            return handlerChain_.get(state);
        }

        return ISet<StateType>();
    }

    typename TransitionHandler<StateType>::Ptr Handler(StateType state)
    {
        if(handlerChain_.containsKey(state))
        {
            return handlerChain_.get(state).Value();
        }

        return typename TransitionHandlerNoOp<StateType>::Ptr(new TransitionHandlerNoOp<StateType>(state) );
    }

    virtual typename RxHandlerChain<StateType>::Ptr Copy() const
    {
        // TODO: copy the handler
        ASSERT(0);

        return RxHandlerChain<StateType>::Ptr();
    }

    HandlerMap& Handlers()
    {
        return handlerChain_;
    }

private:
    HandlerMap handlerChain_;
};

}
