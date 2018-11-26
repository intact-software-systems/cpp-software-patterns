#ifndef BaseLib_Templates_SchedulerMethods_h_IsIncluded
#define BaseLib_Templates_SchedulerMethods_h_IsIncluded

#include "BaseLib/Runnable.h"
#include "BaseLib/Singleton.h"

namespace BaseLib { namespace Templates
{

template <typename T>
class Schedulable : public BaseLib::Runnable
{
public:
    virtual ~Schedulable() {}

    virtual bool HasNext() const = 0;

    virtual T Next() = 0;
};


template <typename T>
class SchedulableNoOp
        : public Schedulable<T>
        , public StaticSingletonPtr<SchedulableNoOp<T>>
{
public:
    virtual ~SchedulableNoOp() {}

    virtual void run()
    {
        // NoOp
    }

    virtual bool HasNext() const
    {
        return false;
    }

    virtual T Next()
    {
        return T();
    }
};

}}

#endif

