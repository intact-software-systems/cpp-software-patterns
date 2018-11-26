#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/StateMachineMethods.h"

namespace BaseLib { namespace Concurrent
{

/**
 * @brief The StateVertex class is a vertex with an adjacency list.
 *
 * Note StateType can be any type.
 */
template <typename StateType, typename T>
class StateVertex
{
public:
    typedef Collection::ISet<StateType> AdjacencyList;

public:
    StateVertex(StateType key, T value)
        : vertex_(key)
        , value_(value)
    {}
    virtual ~StateVertex()
    {}

    virtual bool AddNeighbor(StateType state)
    {
        if(state == vertex_) return false;

        return neighbors_.insert(state).second;
    }

    virtual bool RemoveNeighbor(StateType state)
    {
        return neighbors_.erase(state) > 0;
    }

    unsigned int Degree() const
    {
        return neighbors_.size();
    }

    AdjacencyList& Neighbors()
    {
        return neighbors_;
    }

    /**
     * @brief Key returns the key of the vertex.
     */
    const StateType& Key() const
    {
        return vertex_;
    }

    /**
     * @brief Value returns the state associated to the vertex.
     */
    const T& Value() const
    {
        return value_;
    }

    /**
     * @brief Value returns the state associated to the vertex.
     * @return
     */
    T& Value()
    {
        return value_;
    }

    /**
     * @brief operator ==
     * @param that
     * @return
     */
    bool operator==(const StateVertex<StateType, T> &that)
    {
        return this->vertex_ == that.vertex_;
    }

private:
    StateType vertex_;
    T value_;
    AdjacencyList neighbors_;
};

/**
 * @brief The StateTransition class represents a state transition (from, to).
 *
 * The individual StateType represents a vertex and the state transition (from, to) an edge.
 */
template <typename StateType>
class Transition
{
public:
    Transition(StateType from, StateType to)
        : from_(from)
        , to_(to)
    {}
    virtual ~Transition()
    {}

    const StateType& From() const
    {
        return from_;
    }

    const StateType& To() const
    {
        return to_;
    }

    bool operator==(const Transition &that)
    {
        return this->from_ == that.from_ && this->to_ == that.to_;
    }

    bool operator!=(const Transition &that)
    {
        return !(*this == that);
    }

    bool operator<(const Transition &that)
    {
        return this->from_ < that.from_;
    }


private:
    StateType from_;
    StateType to_;
};

/**
 * TODO: Starting to require graph functionality.
 */
template <typename StateType>
class TransitionPath : public Collection::IMultiMap<StateType, Transition<StateType> >
{
public:
    TransitionPath(StateType origin)
        : origin_(origin)
    {}
    virtual ~TransitionPath()
    {}

    const StateType& Origin() const
    {
        return origin_;
    }

    void AddTransition(StateType from, StateType to)
    {
        this->put(from, Transition<StateType>(from, to));
    }

    void RemoveState(StateType state)
    {
        this->erase(state);
    }

    bool RemoveTransition(const Transition<StateType> &transition)
    {
        typename Collection::IMultiMap<StateType, Transition<StateType> >::iterator it = this->find(transition.From());
        for( ; it != this->end() && it->first == transition.From(); it++)
        {
            if(transition == it->second)
            {
                return erase(it) == 1;
            }
        }

        return false;
    }

private:
    StateType origin_;
};

/**
 * @brief The StateCollection class is used by StateMachineCollection to store objects that are in a given StateType.
 *
 * Key = StateType
 * Value = T
 */
template <typename StateType, typename T>
class StateCollection
        : public Collection::IMap<InstanceHandle, std::shared_ptr<T>>
        , public Runnable
        , public Templates::NoOpMethod
        , public ENABLE_SHARED_FROM_THIS_T2(StateCollection, StateType, T)
{
private:
    typedef Collection::IMap<InstanceHandle, std::shared_ptr<T>> Map;

public:
    StateCollection(StateType stateType)
        : stateType_(stateType)
    {}
    virtual ~StateCollection()
    {}

    CLASS_TRAITS(StateCollection)

    virtual void run()
    {
        futureTasks_.run();
    }

    /**
     * @brief RunTasks Execute N futuretasks in FIFO order
     */
    InstanceHandleSet RunTasks(unsigned int number)
    {
        InstanceHandleSet handles = futureTasks_.GetNHandlesFIFO(number);
        futureTasks_.RunTasks(handles);
        return handles;
    }

    void Merge(typename StateCollection<StateType, T>::Ptr fromCollection)
    {
        for(typename StateCollection<StateType, T>::iterator it = fromCollection->begin(), it_end = fromCollection->end(); it != it_end; ++it)
        {
            Templates::RunnableFuture::Ptr future = fromCollection->Actions().GetTaskBase(it->first);

            ASSERT(future);

            InstanceHandle key = futureTasks_.AddTask(future);
            Map::put(key, it->second);
        }
    }

    void Merge(typename StateCollection<StateType, T>::Ptr fromCollection, const InstanceHandleSet &handles)
    {
        for(InstanceHandleSet::const_iterator it = handles.begin(), it_end = handles.end(); it != it_end; ++it)
        {
            InstanceHandle handle = *it;

            Templates::RunnableFuture::Ptr future = fromCollection->Actions().GetTaskBase(handle);

            ASSERT(fromCollection->containsKey(handle));
            ASSERT(future);

            InstanceHandle key = futureTasks_.AddTask(future);

            std::shared_ptr<T> t = fromCollection->at(handle);

            Map::put(key, t);
        }
    }

    /**
     * @return number of objects in collection actually removed
     */
    size_t Remove(const InstanceHandleSet &handles)
    {
        for(InstanceHandleSet::const_iterator it = handles.begin(), it_end = handles.end(); it != it_end; ++it)
        {
            InstanceHandle handle = *it;

            bool isRemoved = futureTasks_.RemoveTask(handle);

            ASSERT(isRemoved);
        }

        return Map::eraseKeys(handles);
    }

    void ClearAll()
    {
        Map::clear();
        futureTasks_.Clear();
    }

    bool IsEmpty() const
    {
        return Map::empty();
    }

    size_t Size() const
    {
        return Map::size();
    }

    bool AddObject(std::shared_ptr<T> object)
    {
        InstanceHandle key = futureTasks_.AddTask<void, T>(object.get(), &Templates::NoOpMethod::NoOp);
        return Map::put(key, object);
    }

    template <typename Return>
    bool AddAction(std::shared_ptr<T> object, Return (T::*member)())
    {
        InstanceHandle key = futureTasks_.AddTask<Return, T>(object.get(), member);
        return Map::put(key, object);
    }

    template <typename Return, typename Arg1>
    bool AddAction(std::shared_ptr<T> object, Return (T::*member)(Arg1), Arg1 arg1)
    {
        InstanceHandle key = futureTasks_.AddTask<Return, T, Arg1>(object.get(), member, arg1);
        return Map::put(key, object);
    }

//    bool RemoveObject(const T &object)
//    {
//        InstanceHandleSet handles = futureTasks_.GetHandlesFromClass<void, T>(&object, &Templates::NoOpMethod::NoOp);
//        return removeFromCollection(handles);
//    }

//    template <typename Return>
//    bool RemoveAction(const T &object, Return (T::*member)())
//    {
//        InstanceHandleSet handles = futureTasks_.GetHandlesFromClass<Return, T>(&object, member);
//        return removeFromCollection(handles);
//    }

//    template <typename Return, typename Arg1>
//    bool RemoveAction(const T &object, Return (T::*member)(Arg1))
//    {
//        InstanceHandleSet handles = futureTasks_.GetHandlesFromClass<Return, T, Arg1>(&object, member);
//        return removeFromCollection(handles);
//    }

    StateType GetStateType() const
    {
        return stateType_;
    }

    FutureTasks& Actions()
    {
        return futureTasks_;
    }

    const FutureTasks& Actions() const
    {
        return futureTasks_;
    }

    bool operator==(const StateCollection &other) const
    {
        return stateType_ == other.stateType_;
    }

    bool operator!=(const StateCollection &other) const
    {
        return stateType_ != other.stateType_;
    }

private:

    bool removeFromCollection(const InstanceHandleSet &handles)
    {
        size_t numRemoved = futureTasks_.RemoveTasks(handles);

        return Map::eraseKeys(handles) == numRemoved;
    }

private:
    FutureTasks futureTasks_;
    StateType   stateType_;
};

/**
 * @brief The StateMachineCollections class is an implementation of the "Collections for states" software design pattern.
 *
 * A Key Value state machine where StateType = Key, T = Value.
 */
template <typename StateType, typename T>
class StateMachineCollections
        : public Templates::Lockable<Mutex>
        , public ENABLE_SHARED_FROM_THIS_T2(StateMachineCollections, StateType, T)
{
public:
    StateMachineCollections()
    {}
    virtual ~StateMachineCollections()
    {}

    CLASS_TRAITS(StateMachineCollections)

    typename StateMachineCollections::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    size_t ExecuteObjects(StateType state)
    {
        typename StateCollection<StateType,T>::Ptr fromCollection = getOrCreateCollection(state);
        fromCollection->run();

        return fromCollection->Size();
    }

    size_t ExecuteNObjects(int number, StateType state)
    {
        typename StateCollection<StateType,T>::Ptr fromCollection = getOrCreateCollection(state);
        InstanceHandleSet handles = fromCollection->RunTasks(number);

        return handles.size();
    }

    bool ExecuteObjectsAndMove(StateType from, StateType to)
    {
        // --------------------------------
        // Execute actions in from state
        // --------------------------------
        typename StateCollection<StateType,T>::Ptr fromCollection = getOrCreateCollection(from);
        fromCollection->run();

        // --------------------------------
        // merge from collection into to
        // --------------------------------
        {
            typename StateCollection<StateType,T>::Ptr toCollection = getOrCreateCollection(to);
            toCollection->Merge(fromCollection);

            fromCollection->ClearAll();
        }

        return fromCollection->IsEmpty();
    }

    bool ExecuteNObjectsAndMove(unsigned int number, StateType from, StateType to)
    {
        // --------------------------------
        // Execute N actions in from state
        // --------------------------------
        typename StateCollection<StateType,T>::Ptr fromCollection = getOrCreateCollection(from);
        InstanceHandleSet handles = fromCollection->RunTasks(number);

        // --------------------------------
        // merge N objects from collection into to
        // --------------------------------
        {
            typename StateCollection<StateType,T>::Ptr toCollection = getOrCreateCollection(to);
            toCollection->Merge(fromCollection, handles);

            fromCollection->Remove(handles);
        }

        return true;
    }

    bool ClearCollection(StateType type)
    {
        if(!states_.containsKey(type)) return false;

        typename StateCollection<StateType,T>::Ptr collection = getOrCreateCollection(type);
        collection->ClearAll();

        return collection->IsEmpty();
    }

    typename StateCollection<StateType,T>::Ptr GetCollection(StateType type)
    {
        return getOrCreateCollection(type);
    }

//    bool CreateStateTransition(StateType from, StateType to)
//    {
//        createCollection(from);
//        createCollection(to);
//        return addTransition(from, to);
//    }

private:
    typename StateCollection<StateType,T>::Ptr getOrCreateCollection(StateType state)
    {
        if(!states_.containsKey(state))
        {
            typename StateCollection<StateType, T>::Ptr collection(new StateCollection<StateType, T>(state));

            states_.put(state, collection);
        }

        return states_.at(state);
    }

//    bool addTransition(StateType from, StateType to)
//    {
//        return transitions_.put(from, to);
//    }

private:
    Collection::IMap<StateType, typename StateCollection<StateType, T>::Ptr > states_;
    //Collection::IMap<StateType, StateType> transitions_;
};

}}
