#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/Memento.h"
#include"RxConcurrent/MementoSharedType.h"

namespace BaseLib { namespace Templates
{

/**
* ContextObject is designed after the software design pattern "Context Object".
*
* Config           = configuration, policies, qos, strategies, etc
* State            = object state that develops in its lifetime, but not critical to persist
* CriticalState    = object and system state that is critical for the entire system
*
* TODO:
* data             = objects internal data vital to its operation, i.e., not state machine stuff.
*
* sm               = Access to state machine objects
* sm().config      = Access to state policies to be applied in state machine
* sm().status      = Object's status, i.e., count, time, etc, to be used in a state machine
* sm().state       = Object's state, i.e., current state in the state machine.
*                    Can be an implementation of an interface for all states possible in a state machine
* sm().subject     = All subject events originating from this
*
* sm().function    = All std::function or strategy objects that are configured for the API
*
* TODO: Introduce Factory/Builder for construction as a static
*  - Factory factory();
*  - Builder builder();
*
* TODO: Introduce Status which is the "object's Status object, i.e., state in a state machine"
* TODO: Instead of state() use data() for object's internals that are to be made accessible
*
* TODO: Policy for "Copy on write", "Copy on read", No copy,
* TODO: Possible to have policy on Write access, read access?
* TODO: Use memento to create a snapshot, and support setting a memento
* TODO: Introduce subject()?
*/
template <typename Config, typename Data, typename Status = NullState, typename State = NullState>
class MementoContextObjectShared
        : public UndoRedoBase
        , public InitializeMethods
{
public:
    MementoContextObjectShared(Config *config, Data *data, Status *status, State *state)
        : data_(new MementoSharedTypePtr<Data>(data))
        , config_(new MementoSharedTypePtr<Config>(config))
        , status_(new MementoSharedTypePtr<Status>(status))
        , state_(new MementoSharedTypePtr<State>(state))
    {
        Initialize();
    }
    MementoContextObjectShared(Config *config, Data *data, Status *status)
        : data_(new MementoSharedTypePtr<Data>(data))
        , config_(new MementoSharedTypePtr<Config>(config))
        , status_(new MementoSharedTypePtr<Status>(status))
        , state_(new MementoSharedTypePtr<State>(new State()))
    {
        Initialize();
    }
    MementoContextObjectShared(Config *config, Data *data)
        : data_(new MementoSharedTypePtr<Data>(data))
        , config_(new MementoSharedTypePtr<Config>(config))
        , status_(new MementoSharedTypePtr<Status>(new Status()))
        , state_(new MementoSharedTypePtr<State>(new State()))
    {
        Initialize();
    }
    virtual ~MementoContextObjectShared()
    {}


    const MementoSharedTypePtr<Data>& data() const
    {
        return data_.operator *();
    }

    MementoSharedTypePtr<Data>& data()
    {
        return data_.operator *();
    }


    const MementoSharedTypePtr<Config>& config() const
    {
        return config_.operator *();
    }

    MementoSharedTypePtr<Config>& config()
    {
        return config_.operator *();
    }


    const MementoSharedTypePtr<Status>& status() const
    {
        return status_.operator *();
    }

    MementoSharedTypePtr<Status>& status()
    {
        return status_.operator *();
    }


    const MementoSharedTypePtr<State>& state() const
    {
        return state_.operator *();
    }

    MementoSharedTypePtr<State>& state()
    {
        return state_.operator *();
    }

    // -------------------------------------
    // UndoRedoBase interface
    // -------------------------------------

    virtual bool CreateMemento() const
    {
        caretaker_.CreateMemento();
    }

    virtual bool Undo()
    {
        caretaker_.Undo();
    }

    virtual bool Redo()
    {
        caretaker_.Redo();
    }

    virtual void ClearUndo()
    {
        caretaker_.ClearUndo();
    }

    virtual void ClearRedo()
    {
        caretaker_.ClearRedo();
    }

    // -------------------------------------
    // InitializeMethods
    // -------------------------------------

    virtual bool Initalize()
    {
        caretaker_.push_back(data_);
        caretaker_.push_back(config_);
        caretaker_.push_back(status_);
        caretaker_.push_back(state_);

        return true;
    }

    virtual bool IsInitialized() const
    {
        return !caretaker_.empty();
    }

private:
    std::shared_ptr<MementoSharedTypePtr<Data>> data_;
    std::shared_ptr<MementoSharedTypePtr<Config>> config_;
    std::shared_ptr<MementoSharedTypePtr<Status>> status_;
    std::shared_ptr<MementoSharedTypePtr<State>> state_;

    Templates::Caretakers caretaker_;
};

/**
 * @brief The MementoContextDataShared class
 */
template <typename T>
class MementoContextDataShared
        : public UndoRedoBase
        , public InitializeMethods
{
public:
    MementoContextDataShared()
        : value_(new MementoSharedTypePtr<T>(new T()))
    {
        Initialize();
    }

    MementoContextDataShared(const T *value)
        : value_(new MementoSharedTypePtr<T>(value))
    {
        Initialize();
    }

    MementoContextDataShared(const std::shared_ptr<T> value)
        : value_(new MementoSharedTypePtr<T>(value))
    {
        Initialize();
    }

    virtual ~MementoContextDataShared()
    { }

    MementoSharedTypePtr<T>& context()
    {
        return value_.operator *();
    }

    const MementoSharedTypePtr<T>& context() const
    {
        return value_.operator *();
    }

    // -------------------------------------
    // UndoRedoBase interface
    // -------------------------------------

    virtual bool CreateMemento() const
    {
        caretaker_.CreateMemento();
    }

    virtual bool Undo()
    {
        caretaker_.Undo();
    }

    virtual bool Redo()
    {
        caretaker_.Redo();
    }

    virtual void ClearUndo()
    {
        caretaker_.ClearUndo();
    }

    virtual void ClearRedo()
    {
        caretaker_.ClearRedo();
    }

    // -------------------------------------
    // InitializeMethods
    // -------------------------------------

    virtual bool Initalize()
    {
        caretaker_.push_back(value_);
        return true;
    }

    virtual bool IsInitialized() const
    {
        return !caretaker_.empty();
    }

private:
    std::shared_ptr<MementoSharedTypePtr<T>> value_;
    Templates::Caretakers caretaker_;
};

}}
