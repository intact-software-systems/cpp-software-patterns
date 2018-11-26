#include"RxConcurrent/ThreadPool/Workers.h"
#include"RxConcurrent/Strategy/ExecutionStrategy.h"

namespace BaseLib { namespace Concurrent
{

// ------------------------------------------------------
// WorkerPolicy class
// ------------------------------------------------------

WorkerPolicy::WorkerPolicy()
    : lifespan_(Policy::Lifespan::Minutes(2))
    , lease_(Policy::LeasePlan::RenewOnAccess())
    , timeout_(Policy::Timeout::FromMilliseconds(10000))
    , idleTimeout_(Policy::Timeout::FromMilliseconds(60000))
{}

WorkerPolicy::WorkerPolicy(
        Policy::Lifespan lifespan,
        Policy::LeasePlan lease,
        Policy::Timeout timeout,
        Policy::Timeout idleTimeout)
    : lifespan_(lifespan)
    , lease_(lease)
    , timeout_(timeout)
    , idleTimeout_(idleTimeout)
{}

WorkerPolicy::~WorkerPolicy()
{}

const Policy::Lifespan &WorkerPolicy::Lifespan() const
{
    return lifespan_;
}

const Policy::LeasePlan &WorkerPolicy::Lease() const
{
    return lease_;
}

const Policy::Timeout &WorkerPolicy::Timeout() const
{
    return timeout_;
}

const Policy::Timeout &WorkerPolicy::IdleTimeout() const
{
    return idleTimeout_;
}

// ------------------------------------------------------
// WorkerConfig class
// ------------------------------------------------------

WorkerConfig::WorkerConfig(WorkerPolicy policy)
    : Templates::ImmutableType<WorkerPolicy>(policy)
{}

WorkerConfig::~WorkerConfig()
{}

const WorkerPolicy &WorkerConfig::policy() const
{
    return this->delegate();
}

// ------------------------------------------------------
// WorkerState class
// ------------------------------------------------------

WorkerState::WorkerState(BaseLib::Concurrent::WaitSet::Ptr waitSet, AbstractCondition::Ptr executionGuard)
    : waitSet_(waitSet)
    , executionGuard_(executionGuard->GetPtr())
{ }

WorkerState::~WorkerState()
{ }

WaitSet::Ptr WorkerState::Wait() const
{
    return waitSet_;
}

Status::ExecutionStatus &WorkerState::Status()
{
    return executionStatus_;
}

const Status::ExecutionStatus &WorkerState::Status() const
{
    return executionStatus_;
}

AbstractCondition::Ptr WorkerState::Guard() const
{
    return executionGuard_;
}

// ------------------------------------------------------
// Worker class
// ------------------------------------------------------

Worker::Worker(
        const std::string &workerName,
        WaitSet::Ptr waitSet,
        AbstractCondition::Ptr executionGuard,
        const WorkerPolicy &policy)
    : Thread(workerName)
    , Templates::ContextObject<WorkerConfig, WorkerState>
      (
          WorkerConfig(policy),
          WorkerState(waitSet, executionGuard)
      )
{}

Worker::~Worker()
{
    Stop(1000);
}

Worker::Ptr Worker::GetPtr()
{
    return this->shared_from_this();
}

/**
 * Worker has no dependency/state to any particular task
 *
 * TODO: Use RunnableStrategyInvoker as input to executor, use WaitSetExecutionStrategy
 */
void Worker::run()
{
    while(!isStopped())
    {
        bool isDispatched = Strategy::WaitSetExecutionStrategy::Perform(*this->state().Wait().get(), this->state().Status());

        Thread::unsetInterrupt();

        if(isDispatched)
        {
            this->state().Guard()->SetTriggerValue(isDispatched);     // wakes up thread-pool run()
        }
    }

    IDEBUG() << "Stopped thread " << Thread::currentThread();
}

AbstractCondition::Ptr Worker::GetExecutionGuard() const
{
    return this->state().Guard()->GetPtr();
}

// ------------------------------------------------------
// Using interfaces: Called when execution starts and stops
// ------------------------------------------------------

bool Worker::Cancel()
{
    return this->stopWorker(0);
}

void Worker::Start()
{
    startWorker();
}

bool Worker::Stop(int64 msecs)
{
    return this->stopWorker(msecs);
}

// ------------------------------------------------------
// Checking ExecutionStatus and policies
// ------------------------------------------------------

bool Worker::IsTimeout() const
{
    return BaseLib::Strategy::IsTimeout::Perform(
                this->state().Status(),
                this->config().policy().Timeout());
}

bool Worker::IsDone() const
{
    return !Thread::isRunning();
}

bool Worker::IsSuccess() const
{
    return this->state().Status().IsSuccess();
}

bool Worker::IsIdle() const
{
    return Strategy::IsIdle::Perform(
                this->state().Status(),
                this->config().policy().IdleTimeout());
}

Duration Worker::TimeSinceLastExecutionTimeMs() const
{
    return this->state().Status().Time().TimeSinceLastExecutionTime();
}

bool Worker::IsInLifespan() const
{
    return Strategy::IsInLifespan::Perform(
                this->state().Status(),
                this->config().policy().Lifespan());
}

// ------------------------------------------------------
// private implementation
// ------------------------------------------------------

void Worker::startWorker()
{
    this->state().Wait()->Start();
    if(!Thread::isRunning())
        Thread::start();
}

bool Worker::stopWorker(int64 msecs)
{
    this->state().Wait()->Stop();
    this->state().Wait()->WakeAll();

    return Thread::wait(msecs);
}

bool Worker::isStopped() const
{
    return !this->state().Wait()->IsValid();
}

}}
