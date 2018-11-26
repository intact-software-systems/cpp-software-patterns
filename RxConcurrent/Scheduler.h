#pragma once

#include"RxConcurrent/CommonDefines.h"

namespace BaseLib { namespace Concurrent {

template <typename Pool, typename S = std::shared_ptr<Templates::Schedulable<Duration>>>
class Scheduler
    : public BaseLib::Runnable
      , public Templates::FinalizeMethod
      , public Templates::ReactorMethods<bool, Duration, BaseLib::GeneralException>
      , public Templates::LockableType<Scheduler<Pool>>
{
    typedef MutexTypeLocker<Scheduler<Pool, S>> Locker;

public:
    Scheduler(Pool pool, S schedulable)
        : schedulable_(schedulable)
        , pool_(pool)
    { }

    virtual ~Scheduler()
    { }

    virtual void run()
    {
        S schedulable = getSchedulableSecurely();

        if(schedulable != nullptr)
        {
            schedule<Duration>(this, schedulable, pool_);
        }
        else
        {
            IINFO() << "Schedulable went out of scope. Shutting down....";
            pool_->Complete(this);
        }
    }

    virtual bool Next(Duration timeMs)
    {
        return pool_->Next(this, timeMs);
    }

    virtual bool Error(BaseLib::GeneralException exception)
    {
        return pool_->Error(this, exception);
    }

    virtual bool Complete()
    {
        return pool_->Complete(this);
    }

    virtual bool Finalize()
    {
        Locker lock(this);
        schedulable_ = nullptr;
        return true;
    }

private:
    // -----------------------------------------------------------
    // private implementation
    // -----------------------------------------------------------

    S getSchedulableSecurely() const
    {
        Locker lock(this);
        return schedulable_;
    }

    template <typename T>
    static void schedule(Scheduler* scheduler, S schedulable, Pool pool)
    {
        try
        {
            schedulable->run();

            if(schedulable->HasNext())
            {
                T waitTimeMs = schedulable->Next();
                pool->Next(scheduler, waitTimeMs); // == onNext(Pair<Runnable, Long>(this, waitTimeMs))
            }
            else
            {
                pool->Complete(scheduler);
            }
        }
        catch(BaseLib::GeneralException throwable)
        {
            IWARNING() << "Stopping schedulable. Caught unhandled exception " << throwable.msg();
            pool->Error(scheduler, throwable); // == onError(Pair<Runnable, Throwable>(this, throwable));
        }
    }

private:
    S    schedulable_;
    Pool pool_;
};

}}
