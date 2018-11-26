#ifndef BaseLib_TriggerCondition_h_IsIncluded
#define BaseLib_TriggerCondition_h_IsIncluded

#include"BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

/**
 * @brief The TriggerCondition class is a representation of "a > b". Example read one datagram as "num-datagrams-read > min-number".
 *
 * TODO: Implement "WithinRangeTrigger", "ContainsTrigger". Set-based trigger conditions. "NotNullTrigger"?
 *
 * Use the triggers when doing "policy violation checks" and consequent "conditional triggering of alarms".
 */
template <typename T>
class TriggerCondition : public BaseLib::Templates::IsTriggeredMethod
{
public:
    virtual void UpdateTrigger(T) = 0;
};


template <typename T>
class TriggerConditionHolder
        : public TriggerCondition<T>
        //, public Templates::NoCopy<TriggerConditionHolder<T>>
{
public:
    TriggerConditionHolder(TriggerCondition<T> *condition)
        : condition_(condition)
    { }
    virtual ~TriggerConditionHolder()
    {
        delete condition_;
        condition_ = NULL;
    }

    CLASS_TRAITS(TriggerConditionHolder)

    virtual void UpdateTrigger(T val)
    {
        ASSERT(condition_);
        condition_->UpdateTrigger(val);
    }

    virtual bool IsTriggered() const
    {
        ASSERT(condition_);
        return condition_->IsTriggered();
    }

private:
    TriggerCondition<T> *condition_;
};

/**
 * @brief The AlwaysTrigger class
 */
template <typename T>
class AlwaysTrigger : public TriggerCondition<T>
{
public:
    AlwaysTrigger()
    {}
    virtual ~AlwaysTrigger()
    {}

    virtual void UpdateTrigger(T)
    {
        // Do nothing
    }

    virtual bool IsTriggered() const
    {
        return true;
    }
};


/**
 * @brief The NeverTrigger class
 */
template <typename T>
class NeverTrigger : public TriggerCondition<T>
{
public:
    NeverTrigger()
    {}
    virtual ~NeverTrigger()
    {}

    virtual void UpdateTrigger(T)
    {
        // Do nothing
    }

    virtual bool IsTriggered() const
    {
        return false;
    }
};

/**
 * @brief The BooleanTrigger class
 */
class BooleanTrigger
        : public TriggerCondition<bool>
        , public Templates::Lockable<Mutex>
{
public:
    BooleanTrigger(bool initialTrigger = false)
        : trigger_(initialTrigger)
    {}
    virtual ~BooleanTrigger()
    {}

    virtual void UpdateTrigger(bool flag)
    {
        trigger_ = flag;
    }

    virtual bool IsTriggered() const
    {
        return trigger_;
    }

private:
    bool trigger_;
};


/**
 * @brief The GreaterThanTrigger class
 */
template <typename T>
class GreaterThanTrigger : public TriggerCondition<T>
{
public:
    GreaterThanTrigger(T u)
        : u_(u)
    {}
    virtual ~GreaterThanTrigger()
    {}

    virtual void UpdateTrigger(T val)
    {
        t_ = val;
    }

    virtual bool IsTriggered() const
    {
        return t_ > u_;
    }

private:
    T t_;
    T u_;
};


template <typename T>
class LessThanTrigger : public TriggerCondition<T>
{
public:
    LessThanTrigger(T u)
        : u_(u)
    {}
    virtual ~LessThanTrigger()
    {}

    virtual void UpdateTrigger(T val)
    {
        t_ = val;
    }

    virtual bool IsTriggered() const
    {
        return t_ < u_;
    }

private:
    T t_;
    T u_;
};


/**
 * @brief The EqualityTrigger class
 */
template <typename T>
class EqualityTrigger : public TriggerCondition<T>
{
public:
    EqualityTrigger(T u)
        : u_(u)
    {}
    virtual ~EqualityTrigger()
    {}

    virtual void UpdateTrigger(T val)
    {
        t_ = val;
    }

    virtual bool IsTriggered() const
    {
        return t_ == u_;
    }

private:
    T t_;
    T u_;
};

/**
 * @brief The InequalityTrigger class
 */
template <typename T>
class InequalityTrigger : public TriggerCondition<T>
{
public:
    InequalityTrigger(T u)
        : u_(u)
    {}
    virtual ~InequalityTrigger()
    {}

    virtual void UpdateTrigger(T val)
    {
        t_ = val;
    }

    virtual bool IsTriggered() const
    {
        return t_ != u_;
    }

private:
    T t_;
    T u_;
};

}}

#endif
