#pragma once

#include"RxConcurrent/ThreadPool/ThreadPool.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

/**
 * @brief The RxThreadPool class executes the reactor handlers.
 */
class DLL_STATE RxThreadPool
    : public BaseLib::Concurrent::ThreadPool
    //, public Observer::RxScheduleObserver<Runnable*>
{
public:
    RxThreadPool(ThreadPoolBase::PoolName name, ThreadPoolPolicy policy = ThreadPoolPolicy::Default());
    virtual ~RxThreadPool();

    CLASS_TRAITS(RxThreadPool)

    // -----------------------------------------------------------
    // Reactor functions
    // -----------------------------------------------------------

    virtual bool Next(Runnable* value, Duration duration);
    virtual bool Complete(Runnable* runnable);
    virtual bool Error(Runnable*, BaseLib::GeneralException&);

    // -----------------------------------------------------------
    // Callback functions
    // -----------------------------------------------------------

    virtual void OnInit(Runnable*);
    virtual void OnSchedule(Runnable* runnable, Duration duration);
    virtual void OnReady(Runnable* runnable);
    virtual void OnDispose(Runnable* runnable);
    virtual void OnAbort(Runnable*, GeneralException);

    // -----------------------------------------------------------
    // Factory functions
    // -----------------------------------------------------------

    static RxThreadPool::Ptr Create(ThreadPoolPolicy rxThreadPoolPolicy, ThreadPoolBase::PoolName id);
};

}}
