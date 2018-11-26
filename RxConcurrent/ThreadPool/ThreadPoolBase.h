#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/WaitSet.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

/**
 * @brief The TaskScheduler class can be attached to a ThreadPool and service as a task scheduler for it.
 *
 * Note! This class must hold a ThreadPool<TASK>::Ptr to the ThreadPool it is scheduling tasks for.
 */
class DLL_STATE TaskScheduler
        : public Templates::ScheduleMethod<bool>
{
public:
    virtual ~TaskScheduler() {}

    CLASS_TRAITS(TaskScheduler)
};

/**
 * @brief The ThreadFactory class is base interface for creating new threads.
 */
class DLL_STATE ThreadFactory
        : public Templates::CreateMethod1<Thread*, const std::string& >
{
public:
    virtual ~ThreadFactory() {}

    CLASS_TRAITS(ThreadFactory)
};

// ----------------------------------------------------
// Base for ScheduledTasks
// ----------------------------------------------------

class DLL_STATE ScheduledTasksBase
        : public Templates::Action
        , public Templates::TriggerMethod
        , public Templates::IsEmptyMethod
        , public Templates::IsSuspendedMethod
        , public Templates::ResumeMethod
        , public Templates::SuspendMethod
{
public:
    virtual ~ScheduledTasksBase() {}

    CLASS_TRAITS(ScheduledTasksBase)

    virtual size_t Size() const = 0;

    virtual void Clear(WaitSet::Ptr executorWaitSet) = 0;

    virtual Duration FindReady(std::list<Templates::ScheduledRunnableFuture::Ptr> &readyTasks) = 0;
    virtual int TriggerReady() = 0;

    virtual int NumTasksExecuting() const = 0;
    virtual int NumTasksPolicyViolated() const = 0;
    virtual int NumTasksNeverStarted() const = 0;

    virtual Duration EarliestReadyIn() const = 0;
};

// ----------------------------------------------------
// Base for ThreadPool
// ----------------------------------------------------

class DLL_STATE ThreadPoolBase
        : public Templates::StartMethod
        , public Templates::StopAndWaitMethod<bool>
{
public:
    virtual ~ThreadPoolBase() {}

    CLASS_TRAITS(ThreadPoolBase)

    typedef std::string PoolName;

    virtual Duration FindReady(std::list<Templates::ScheduledRunnableFuture::Ptr> &readyTasks) = 0;

    virtual void ClearTasks() = 0;

    virtual bool StopScheduler(int64 msecs) = 0;
    virtual bool StartScheduler() = 0;

    virtual const PoolName& GetPoolName() const = 0;

    virtual void SetTaskScheduler(TaskScheduler::Ptr) = 0;
    virtual void UseDefaultTaskScheduler() = 0;
};

}}
