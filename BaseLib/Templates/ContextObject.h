#ifndef BaseLib_Templates_ContextObject_h_Included
#define BaseLib_Templates_ContextObject_h_Included

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/BasePtrTypes.h"

namespace BaseLib { namespace Templates
{

// --------------------------------------------------------
// Default values to ContextObject can be these "NoOp" classes
// --------------------------------------------------------

class NullTransientState
{
public:
    NullTransientState() {}
    ~NullTransientState() {}
};

class NullCriticalState
{
public:
    NullCriticalState() {}
    ~NullCriticalState() {}
};

class NullConfig
{
public:
    NullConfig() {}
    ~NullConfig() {}
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
 * sn().state       = Object's state, i.e., current state in the state machine.
 *                    Can be an implementation of an interface for all states possible in a state machine
 *
 * TODO: Introduce Factory/Builder for construction as a static
 *  - Factory factory();
 *  - Builder builder();
 *
 * TODO: Introduce Status which is the "object's Status object, i.e., state in a state machine"
 * TODO: Instead of state() use data() for object's internals that are to be made accessible
 *
 *
 */
template <typename Config, typename State, typename CriticalState = NullCriticalState>
class ContextObject
{
public:
    ContextObject(const Config &config, const State &transientState, const CriticalState &criticalState)
        : config_(config)
        , state_(transientState)
        , criticalState_(criticalState)
    {}

    ContextObject(const Config &config, const State &transientState)
        : config_(config)
        , state_(transientState)
        , criticalState_()
    {}

    ContextObject(const Config &config)
        : config_(config)
        , state_()
        , criticalState_()
    {}

    ContextObject()
        : config_()
        , state_()
        , criticalState_()
    {}

    virtual ~ContextObject()
    {}

    // ---------------------------------------------
    // Config getter and setter
    // ---------------------------------------------
    Config& config()
    {
        return config_.delegate();
    }

    const Config& config() const
    {
        return config_.delegate();
    }

    void setConfig(const Config &config)
    {
        config_.Set(config);
    }

    // ---------------------------------------------
    // CriticalState getter and setter
    // ---------------------------------------------
    CriticalState& critical()
    {
        return criticalState_.delegate();
    }

    const CriticalState& critical() const
    {
        return criticalState_.delegate();
    }

    void setCritical(const CriticalState &criticalState)
    {
        criticalState_.Set(criticalState);
    }

    // ---------------------------------------------
    // TransientState getter and setter
    // ---------------------------------------------
    State& state()
    {
        return state_.delegate();
    }

    const State& state() const
    {
        return state_.delegate();
    }

    void setState(const State &state)
    {
        state_.Set(state);
    }

private:
    /**
     * @brief config_
     */
    ComparableMutableType<Config> config_;

    /**
     * @brief transientState_
     */
    ComparableMutableType<State> state_;

    /**
     * @brief criticalState_
     */
    ComparableMutableType<CriticalState> criticalState_;
};

/**
 * ContextObjectPtr is designed after the software design pattern "Context Object".
 *
 * Config           = configuration, policies, qos, strategies, etc
 * TransientState   = object state that develops in its lifetime, but not critical to persist
 * CriticalState    = object and system state that is critical for the entire system
 */
template <typename Config, typename State, typename CriticalState = NullCriticalState>
class ContextObjectPtr
{
public:
    ContextObjectPtr(Config *config, State *transientState, CriticalState *criticalState)
        : config_(config)
        , state_(transientState)
        , criticalState_(criticalState)
    {}

    ContextObjectPtr(Config *config, State *transientState)
        : config_(config)
        , state_(transientState)
        , criticalState_(new NullCriticalState())
    {}

//    ContextObjectPtr(Config *config)
//        : config_(config)
//        , state_()
//        , criticalState_(new NullCriticalState())
//    {}

//    ContextObjectPtr()
//        : config_(new NullConfig())
//        , state_(new NullTransientState())
//        , criticalState_(new NullCriticalState())
//    {}

    virtual ~ContextObjectPtr()
    {
        delete config_.Clone();
        delete state_.Clone();
        delete criticalState_.Clone();

//        delete config_;
//        delete state_;
//        delete criticalState_;

//        config_ = nullptr;
//        state_ = nullptr;
//        criticalState_ = nullptr;
    }

    // ---------------------------------------------
    // Config getter and setter
    // ---------------------------------------------
//    Config& config()
//    {
//        return config_.delegate();
//    }

    const Config& config() const
    {
        return config_.delegate();
    }

//    void setConfig(const Config &config)
//    {
//        config_.Set(config);
//    }

    // ---------------------------------------------
    // CriticalState getter and setter
    // ---------------------------------------------
    CriticalState& critical()
    {
        return criticalState_.delegate();
    }

    const CriticalState& critical() const
    {
        return criticalState_.delegate();
    }

    void setCritical(CriticalState *criticalState)
    {
        criticalState_.Set(criticalState);
    }

    // ---------------------------------------------
    // TransientState getter and setter
    // ---------------------------------------------
    State& state()
    {
        return state_.delegate();
    }

    const State& state() const
    {
        return state_.delegate();
    }

    void setState(State *state)
    {
        state_.Set(state);
    }

private:
    /**
     * @brief config_
     */
    ImmutableTypePtr<Config> config_;

    /**
     * @brief transientState_
     */
    MutableTypePtr<State> state_;

    /**
     * @brief criticalState_
     */
    MutableTypePtr<CriticalState> criticalState_;
};


/**
 * @brief The ContextData class
 */
template <typename T>
class ContextData
{
public:
    ContextData()
        : value_()
    {}

    ContextData(const T &value)
        : value_(value)
    {}

    // ---------------------------------------------
    // Data getter and setter
    // ---------------------------------------------

    T& context()
    {
        return value_.delegate();
    }

    const T& context() const
    {
        return value_.delegate();
    }

    void setContext(const T& value)
    {
        value_.Set(value);
    }

private:
    /**
     * @brief value_
     */
    ComparableMutableType<T> value_;
};

/**
 * @brief The ContextPolicy class
 *
 * TODO: Check out the DDS qos with specialized policy functions.
 */
template <typename T>
class ContextPolicy
        : protected ComparableImmutableType<T>
{
public:
    ContextPolicy(T policy)
        : ComparableImmutableType<T>(policy)
    {}
    virtual ~ContextPolicy()
    {}

    const T& policy() const
    {
        return this->delegate();
    }
};

}}

#endif
