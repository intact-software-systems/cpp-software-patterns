#pragma once

#include"BaseLib/Debug.h"
#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/BasePtrTypes.h"
#include"BaseLib/Templates/SharedType.h"
#include"BaseLib/Templates/Lifecycle.h"

namespace BaseLib { namespace Templates
{

class NullData
{
public:
    ~NullData() {}
};

class NullState
{
public:
    ~NullState() {}
};

class NullSubject
{
public:
    ~NullSubject() {}
};

class NullFunction
{
public:
    ~NullFunction() {}
};

class NullStatus
{
public:
    ~NullStatus() {}
};

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
template <typename Config, typename Data, typename Status = NullStatus, typename Subject = NullSubject, typename Function = NullFunction, typename State = NullState>
class ContextObjectShared
{
public:
    ContextObjectShared(Config *config, Data *data, Status *status, Subject *subject, Function* function, State *state)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(state)
        , subject_(subject)
        , function_(function)
    {}
    ContextObjectShared(Config *config, Data *data, Status *status, Subject *subject, Function* function)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(new State())
        , subject_(subject)
        , function_(function)
    {}
    ContextObjectShared(Config *config, Data *data, Status *status, Subject *subject, State *state)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(state)
        , subject_(subject)
        , function_(new Function())
    {}
    ContextObjectShared(Config *config, Data *data, Status *status, State *state)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(state)
        , subject_(new Subject())
        , function_(new Function())
    {}
    ContextObjectShared(Config *config, Data *data, Status *status, Subject *subject)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(new State())
        , subject_(subject)
        , function_(new Function())
    {}
    ContextObjectShared(Config *config, Data *data, Status *status, Function *function)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(new State())
        , subject_(new Subject())
        , function_(function)
    {}
    ContextObjectShared(Config *config, Data *data, Status *status)
        : data_(data)
        , config_(config)
        , status_(status)
        , state_(new State())
        , subject_(new Subject())
        , function_(new Function())
    {}
    ContextObjectShared(Config *config, Data *data)
        : data_(data)
        , config_(config)
        , status_(new Status())
        , state_(new State())
        , subject_(new Subject())
        , function_(new Function())
    {}
    ContextObjectShared(Config *config, std::shared_ptr<Data> data)
        : data_(data)
        , config_(config)
        , status_(new Status())
        , state_(new State())
        , subject_(new Subject())
        , function_(new Function())
    {}

    virtual ~ContextObjectShared()
    {}

    /**
     * See Qt's QSharedData that calls detach upon accessing write operatios.
     *
     * Apply in BaseTypePtr to support "Copy on write"
     */
    const SharedTypePtr<Data>& data() const
    {
        return data_;
    }

    SharedTypePtr<Data>& data()
    {
        return data_;
    }


    const SharedTypePtr<Config>& config() const
    {
        return config_;
    }

    SharedTypePtr<Config>& config()
    {
        return config_;
    }


    const SharedTypePtr<Status>& status() const
    {
        return status_;
    }

    SharedTypePtr<Status>& status()
    {
        return status_;
    }


    const SharedTypePtr<Subject>& subject() const
    {
        return subject_;
    }

    SharedTypePtr<Subject>& subject()
    {
        return subject_;
    }


    const SharedTypePtr<Function>& function() const
    {
        return function_;
    }

    SharedTypePtr<Function>& function()
    {
        return function_;
    }


    const SharedTypePtr<State>& state() const
    {
        return state_;
    }

    SharedTypePtr<State>& state()
    {
        return state_;
    }


private:
    SharedTypePtr<Data> data_;
    SharedTypePtr<Config> config_;
    SharedTypePtr<Status> status_;
    SharedTypePtr<State> state_;
    SharedTypePtr<Subject> subject_;
    SharedTypePtr<Function> function_;
};

/**
 * @brief The ContextDataShared class
 */
template <typename T>
class ContextDataShared
{
public:
    ContextDataShared()
        : value_(new T())
    {}

    ContextDataShared(T *value)
        : value_(value)
    {}

    ContextDataShared(std::shared_ptr<T> value)
        : value_(value)
    {}

    virtual ~ContextDataShared()
    { }
//
//    SharedTypePtr<T>& context()
//    {
//        return value_;
//    }
//
//    const SharedTypePtr<T>& context() const
//    {
//        return value_;
//    }
//

    std::shared_ptr<T>& context()
    {
        return value_;
    }

    const std::shared_ptr<T>& context() const
    {
        return value_;
    }

private:
    std::shared_ptr<T> value_;
};

}}
