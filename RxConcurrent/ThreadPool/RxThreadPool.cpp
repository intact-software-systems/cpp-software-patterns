#include"RxConcurrent/ThreadPool/RxThreadPool.h"

namespace BaseLib { namespace Concurrent {

RxThreadPool::RxThreadPool(ThreadPoolBase::PoolName name, ThreadPoolPolicy policy)
    : ThreadPool(name, policy)
{ }

RxThreadPool::~RxThreadPool()
{ }

// -----------------------------------------------------------
// Reactor functions
// -----------------------------------------------------------

bool RxThreadPool::Next(Runnable* runnable, Duration duration)
{
    ThreadPool::Schedule(runnable, TaskPolicy::RunOnceIn(duration));
    return true;
}

bool RxThreadPool::Complete(Runnable*)
{
    ICRITICAL() << "TODO implement";
    return true;
}

bool RxThreadPool::Error(Runnable*, BaseLib::GeneralException&)
{
    ICRITICAL() << "TODO implement";
    return true;
}

// -----------------------------------------------------------
// Callback functions
// -----------------------------------------------------------

void RxThreadPool::OnInit(Runnable*)
{
    //this->Add(value, QosTask::WaitUntilTrigger());
}

void RxThreadPool::OnSchedule(Runnable* runnable, Duration duration)
{
    ThreadPool::Schedule(runnable, TaskPolicy::RunOnceIn(duration));
}

void RxThreadPool::OnDispose(Runnable*)
{
    IDEBUG() << "Not implemented";
    //this->Remove(runnable);
}

void RxThreadPool::OnReady(Runnable* runnable)
{
    IDEBUG() << "Triggering " << runnable->GetName();
    this->Trigger(runnable);
}

void RxThreadPool::OnAbort(Runnable*, GeneralException)
{
    ICRITICAL() << "Not implemented";
    // Match runnable with scheduled future and call remove.
    //this->Remove(runnable);
}

// -----------------------------------------------------------
// Factory functions
// -----------------------------------------------------------

RxThreadPool::Ptr RxThreadPool::Create(ThreadPoolPolicy policy, ThreadPoolBase::PoolName id)
{
    return RxThreadPool::Ptr(new RxThreadPool(id, policy));
    //threadPool->setRejectedExecutionHandler(threadPool);
}

}}
