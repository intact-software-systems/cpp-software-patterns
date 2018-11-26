#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/GuardCondition.h"
#include"RxConcurrent/WaitSet.h"
#include"RxConcurrent/FutureFactory.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

// ------------------------------------------------------
// WorkerPolicy class
// ------------------------------------------------------

class DLL_STATE WorkerPolicy
{
public:
    WorkerPolicy();

    WorkerPolicy(
            Policy::Lifespan lifespan,
            Policy::LeasePlan lease,
            Policy::Timeout timeout,
            Policy::Timeout idleTimeout);
    virtual ~WorkerPolicy();

    const Policy::Lifespan&     Lifespan() const;
    const Policy::LeasePlan&    Lease() const;
    const Policy::Timeout&      Timeout() const;
    const Policy::Timeout&      IdleTimeout() const;

    static WorkerPolicy Default()
    {
        return WorkerPolicy(Policy::Lifespan::Minutes(1),
                            Policy::LeasePlan::RenewOnAccess(),
                            Policy::Timeout::FromMinutes(5),
                            Policy::Timeout::FromMilliseconds(60000));
    }

private:
    Policy::Lifespan    lifespan_;
    Policy::LeasePlan   lease_;
    Policy::Timeout     timeout_;
    Policy::Timeout     idleTimeout_;
};

// ------------------------------------------------------
// WorkerConfig class
// ------------------------------------------------------

class DLL_STATE WorkerConfig : protected Templates::ImmutableType<WorkerPolicy>
{
public:
    WorkerConfig(WorkerPolicy policy = WorkerPolicy::Default());
    virtual ~WorkerConfig();

    const WorkerPolicy& policy() const;
};

// ------------------------------------------------------
// WorkerState class
// ------------------------------------------------------

class DLL_STATE WorkerState
{
public:
    WorkerState() {}
    WorkerState(WaitSet::Ptr waitSet, AbstractCondition::Ptr executionGuard);
    virtual ~WorkerState();

    WaitSet::Ptr Wait() const;

    Status::ExecutionStatus&        Status();
    const Status::ExecutionStatus&  Status() const;

    // Shared with external scheduler
    AbstractCondition::Ptr Guard() const;

private:
    WaitSet::Ptr waitSet_;
    AbstractCondition::Ptr executionGuard_;
    Status::ExecutionStatus executionStatus_;
};

// ------------------------------------------------------
// Worker class
// ------------------------------------------------------

/**
 * @brief The Worker class executes ready tasks. The executor has no dependency/state to any particular task.
 */
class DLL_STATE Worker
        : public Thread
        , public Templates::Action
        , public Templates::StartMethod
        , public Templates::StopAndWaitMethod<bool>
        , public Templates::IsIdleMethod
        , public Templates::ContextObject<WorkerConfig, WorkerState>
        , public ENABLE_SHARED_FROM_THIS(Worker)
{
public:
    Worker(const std::string &workerName, WaitSet::Ptr waitSet, AbstractCondition::Ptr executionGuard, const WorkerPolicy &policy);
    virtual ~Worker();

    CLASS_TRAITS(Worker)

    Worker::Ptr GetPtr();

    /**
     * Worker has no dependency/state to any particular task
     */
    virtual void run();

    AbstractCondition::Ptr GetExecutionGuard() const;

    // ------------------------------------------------------
    // Using interfaces: Called when execution starts and stops
    // ------------------------------------------------------

    virtual bool Cancel();
    virtual void Start();
    virtual bool Stop(int64 msecs = 200);

    // ------------------------------------------------------
    // Checking ExecutionStatus and policies
    // ------------------------------------------------------

    virtual bool IsTimeout() const;
    virtual bool IsDone() const;
    virtual bool IsSuccess() const;
    virtual bool IsIdle() const;

    Duration TimeSinceLastExecutionTimeMs() const;

    bool IsInLifespan() const;

private:
    // ------------------------------------------------------
    // private implementation
    // ------------------------------------------------------

    void startWorker();
    bool stopWorker(int64 msecs = 200);
    bool isStopped() const;
};

}}
