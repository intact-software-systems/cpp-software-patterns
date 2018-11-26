#include"RxConcurrent/ThreadPool/ThreadPool.h"
#include"RxConcurrent/Strategy/MonitorWorkers.h"

namespace BaseLib { namespace Concurrent {

// ------------------------------------------------------
// ThreadPoolConfig
// ------------------------------------------------------

ThreadPoolConfig::ThreadPoolConfig(ThreadPoolPolicy policy)
    : policy_(policy)
    , defaultWorkersPolicy_(WorkersPolicy::Default())
    , defaultWorkerPolicy_(WorkerPolicy::Default())
{ }

ThreadPoolConfig::~ThreadPoolConfig()
{ }

const ThreadPoolPolicy& ThreadPoolConfig::policy() const
{
    return policy_;
}

const WorkersPolicy& ThreadPoolConfig::DefaultWorkersPolicy() const
{
    return defaultWorkersPolicy_;
}

const WorkerPolicy& ThreadPoolConfig::DefaultWorkerPolicy() const
{
    return defaultWorkerPolicy_;
}

// ------------------------------------------------------
// ThreadPoolState
// ------------------------------------------------------

ThreadPoolState::ThreadPoolState(ThreadPoolState::PoolName poolName)
    : workers_(new Workers())
    , tasks_(new ScheduledTasks())
    , waitSet_(new WaitSet())
    , waitSetExecutorToPool_(new WaitSet())
    , taskScheduler_()
    , status_()
    , poolName_(poolName)
{ }

ThreadPoolState::~ThreadPoolState()
{ }

Workers::Ptr ThreadPoolState::Executors()
{
    return workers_->GetPtr();
}

ScheduledTasks::Ptr ThreadPoolState::Tasks()
{
    return tasks_->GetPtr();
}

WaitSet::Ptr ThreadPoolState::ExecutorWaitSet()
{
    return this->waitSet_->GetPtr();
}

WaitSet::Ptr ThreadPoolState::SchedulerWaitSet()
{
    return this->waitSetExecutorToPool_->GetPtr();
}

std::shared_ptr<TaskScheduler> ThreadPoolState::Scheduler()
{
    return this->taskScheduler_;
}

void ThreadPoolState::SetTaskScheduler(const std::shared_ptr<TaskScheduler>& scheduler)
{
    taskScheduler_ = scheduler;
}

const ThreadPoolState::PoolName& ThreadPoolState::Name() const
{
    return poolName_;
}

Status::ExecutionStatus& ThreadPoolState::Status()
{
    return status_;
}

const Status::ExecutionStatus& ThreadPoolState::Status() const
{
    return status_;
}

// ------------------------------------------------------
// ThreadPool
// ------------------------------------------------------

ThreadPool::ThreadPool(ThreadPoolBase::PoolName poolName, ThreadPoolPolicy policy)
    : Thread(poolName)
    , Templates::ContextObject<ThreadPoolConfig, ThreadPoolState>
        (
            ThreadPoolConfig(policy), ThreadPoolState(poolName)
        )
{ }

ThreadPool::~ThreadPool()
{
    ClearTasks();
    Stop();
}

ThreadPool::Ptr ThreadPool::GetPtr()
{
    return this->shared_from_this();
}

// ----------------------------------------------------
// Task management
// ----------------------------------------------------

ScheduledFutureTask<bool> ThreadPool::Trigger(BaseLib::Runnable* task)
{
    std::shared_ptr<FutureTask<bool>> future = FutureFactory::Create(task);

    Locker lock(this);
    return schedule<bool>(future, TaskPolicy::RunOnceImmediately(), Function::defaultScheduleFunction, false);
}

ScheduledFutureTask<bool> ThreadPool::Schedule(BaseLib::Runnable* task, Duration duration, bool createAsSuspended)
{
    std::shared_ptr<FutureTask<bool>> future = FutureFactory::Create(task);

    Locker lock(this);
    return schedule<bool>(future, TaskPolicy::RunOnceIn(duration), Function::defaultScheduleFunction, createAsSuspended);
}

ScheduledFutureTask<bool> ThreadPool::Schedule(BaseLib::Runnable* task, TaskPolicy taskPolicy, bool createAsSuspended)
{
    std::shared_ptr<FutureTask<bool>> future = FutureFactory::Create(task);

    Locker lock(this);
    return schedule<bool>(future, taskPolicy, Function::defaultScheduleFunction, createAsSuspended);
}

ScheduledFutureTask<bool> ThreadPool::Schedule(std::shared_ptr<BaseLib::Runnable> task, TaskPolicy taskPolicy, bool createAsSuspended)
{
    std::shared_ptr<FutureTask<bool>> future = FutureFactory::Create(task);

    Locker lock(this);
    return schedule<bool>(future, taskPolicy, Function::defaultScheduleFunction, createAsSuspended);
}

ScheduledRunnableTask::Ptr ThreadPool::Remove(std::shared_ptr<Templates::ScheduledRunnableFuture> task)
{
    Locker lock(this);
    return remove(task);
}

// ----------------------------------------------------
// Get list of tasks ready for execution
// ----------------------------------------------------

Duration ThreadPool::FindReady(ScheduledTasksState::ListTaskBase& readyTasks)
{
    return this->state().Tasks()->FindReady(readyTasks);
}

// ----------------------------------------------------
// Interface ThreadPoolBase
// ----------------------------------------------------

bool ThreadPool::Stop(int64 msecs)
{
    Locker lock(this);

    this->state().Status().Cancel();

    this->state().ExecutorWaitSet()->Stop();
    this->state().SchedulerWaitSet()->Stop();

    this->state().Executors()->Cancel();

    IDEBUG() << "Stopped " << this->state().Executors()->Size() << " threads working on " << this->state().Tasks()->Size() << " tasks";

    return Thread::wait(msecs);
}

void ThreadPool::Start()
{
    Locker lock(this);

    this->state().Status().Start();
    this->state().ExecutorWaitSet()->Start();
    this->state().SchedulerWaitSet()->Start();

    if(!Thread::isRunning())
    {
        Thread::start();
    }

    this->state().Executors()->Start();

    IDEBUG() << "Started " << this->state().Executors()->Size() << " threads working on " << this->state().Tasks()->Size() << " tasks";
}

bool ThreadPool::StopScheduler(int64 msecs)
{
    Locker lock(this);

    if(!Thread::isRunning()) return true;

    this->state().Status().Cancel();
    this->state().SchedulerWaitSet()->WakeAll();

    return Thread::wait(msecs);
}

bool ThreadPool::StartScheduler()
{
    Locker lock(this);

    if(Thread::isRunning()) return true;

    this->state().Status().Start();

    return Thread::start();
}

void ThreadPool::ClearTasks()
{
    Locker lock(this);

    IDEBUG() << "Clearing " << this->state().Executors()->Size() << " threads working on " << this->state().Tasks()->Size() << " tasks";

    this->state().Executors()->Clear(this->state().SchedulerWaitSet());
    this->state().Tasks()->Clear(this->state().ExecutorWaitSet());

    this->state().ExecutorWaitSet()->ClearTriggeredQueue();
    this->state().SchedulerWaitSet()->ClearTriggeredQueue();
}

const ThreadPoolBase::PoolName& ThreadPool::GetPoolName() const
{
    return this->state().Name();
}

void ThreadPool::SetTaskScheduler(std::shared_ptr<TaskScheduler> taskScheduler)
{
    Locker lock(this);
    this->state().SetTaskScheduler(taskScheduler);
}

void ThreadPool::UseDefaultTaskScheduler()
{
    Locker lock(this);
    this->state().SetTaskScheduler(std::shared_ptr<TaskScheduler>());
}

int ThreadPool::StartWorkers(int numToRestart)
{
    int numRestarted = this->state().Executors()->Restart(numToRestart);

    if(numRestarted < numToRestart)
    {
        numRestarted += createWorkers(numToRestart - numRestarted);
    }

    return numRestarted;
}

// ----------------------------------------------------
// Status
// ----------------------------------------------------

std::ostream& ThreadPool::Report(std::ostream& ostr)
{
    this->state().Tasks()->Report(ostr);
    this->state().Executors()->Report(ostr);

    ostr << "Pool(" << this->state().Name() << "): " << this->state().Status() << std::endl;

    return ostr;
}

// ----------------------------------------------------
// Callback from Worker through WaitSet and GuardCondition
// ----------------------------------------------------

/**
 * @brief operator () callback from all Worker::run() using WaitSet
 * After execution completed then ThreadPool::run() can reschedule it if applicable
 *
 * NOTE! Must be highly concurrent implementation to enable efficiency and scalability!
 *
 * @param task
 */
void ThreadPool::operator()(ScheduledRunnableTask::Ptr& task)
{
    // -- debug --
    ASSERT(task != nullptr);
    ASSERT(task->Task() != nullptr);
    // NOTE! Resume/Suspend/Trigger might change flags before actually executed
    //ASSERT(task->Task()->StatusConst().IsStarting());
    ASSERT(!task->Task()->StatusConst().IsStarted());
    // -- debug --

    Entry(task);

    Status::ExecutionStatus status;
    Strategy::DefaultRunnableExecution::Perform(task.get(), status);

    Exit(task, status);
}

void ThreadPool::Entry(ScheduledRunnableTask::Ptr&)
{
    // Callback to observers subject_.OnEntry(task);
}

void ThreadPool::Exit(ScheduledRunnableTask::Ptr&, Status::ExecutionStatus& status)
{
    if(status.IsFailed())
    {
        // TODO: Feedback to ThreadPool?
    }

    // Callback to observers subject_.OnExit(task);
}

// ----------------------------------------------------
// Thread running
// TODO: execute the run from a "controller" ThreadPool that controls several thread pools.
// TOOD: Introduce ThreadPoolStatus (schedule miss, hit, tasks, etc)
// ----------------------------------------------------

void ThreadPool::run()
{
    IINFO() << "Task executor started for " << this->state().Name();

    while(!this->state().Status().IsCancelled())
    {
        manageWorkers();

        schedule();

        //Report(std::cout);

        manageTasks();
    }

    IINFO() << "Task executor stopped for " << this->state().Name();
}

void ThreadPool::schedule()
{
    if(isDefaultScheduler())
    {
        int      triggeredNumber  = this->state().Tasks()->TriggerReady();
        Duration nextScheduleTime = this->state().Tasks()->EarliestReadyIn();

        if(nextScheduleTime == Duration::zero() && triggeredNumber == 0)
        {
            IWARNING() << "Found nothing to schedule backing off 1000 msecs!";
            nextScheduleTime = Duration::FromMilliseconds(1000);
        }

        {
            //ScopedTimer a(IINFO());
            this->state().SchedulerWaitSet()->WaitDispatch(nextScheduleTime.InMillis());
        }
    }
    else
    {
        TaskScheduler::Ptr scheduler = ThreadPool::getExternalScheduler(this);
        if(scheduler != nullptr)
        {
            scheduler->Schedule(1000);
        }
    }
}

void ThreadPool::manageTasks()
{
    // TODO: Remove finished tasks when lifespan expired.
    std::list<ScheduledRunnableTask::Ptr> doneTasks = this->state().Tasks()->RemoveDone();

    this->state().Tasks()->InterruptTimeoutTasks();

//    if(!doneTasks.empty())
//    {
//        IDEBUG() << "Removed " << doneTasks.size() << " tasks that are done.";

//        for(ScheduledRunnableTask::Ptr future : doneTasks)
//        {
//            IDEBUG() << "Removed future: " << future->StatusConst();
//        }
//    }

//    int policyViolationCounter = this->state().Tasks()->NumTasksPolicyViolated();
//    int numTasksNeverExecuted = this->state().Tasks()->NumTasksNeverStarted();

//    if(policyViolationCounter > 0 || numTasksNeverExecuted > 0)
//        IWARNING() << "Total: " << this->state().Tasks()->Size() << ". Tasks whose policy violated " << policyViolationCounter << ". Tasks never started " << numTasksNeverExecuted;
}

void ThreadPool::manageWorkers()
{
    Strategy::LoadBalanceWorkers::Perform(this);

    // Use centralized workers external to ThreadPool, this makes it possible to detect starvation, thread pool problems, etc.
    // Use ScheduledTask with Strategy and trigger run through Workers if time to manage workers
    // Can I do the same with scheduling?
    // Use similar approaches for all managment tasks that can be implemented in a strategy.
    // For example, adding and removing tasks. Just trigger future. How to organize the strategies? Just separate instances of ScheduledTasks, I guess.
}

bool ThreadPool::isDefaultScheduler()
{
    return !this->state().Scheduler();
}

TaskScheduler::Ptr ThreadPool::getExternalScheduler(ThreadPool* pool)
{
    if(!pool->state().Scheduler()) return TaskScheduler::Ptr();

    Locker lock(pool);
    return pool->state().Scheduler();
}

// ----------------------------------------------------
// Task managment
// ----------------------------------------------------

ScheduledRunnableTask::Ptr ThreadPool::remove(Templates::ScheduledRunnableFuture::Ptr task)
{
    ScheduledRunnableTask::Ptr future = this->state().Tasks()->Remove(task);
    if(!future) return ScheduledRunnableTask::Ptr();

    this->state().ExecutorWaitSet()->Detach(future->state().Cond());

    return future;
}

// ----------------------------------------------------
// Worker managment
// ----------------------------------------------------

int ThreadPool::createWorkers(int numWorkersToCreate)
{
    int numCreated = 0;

    for(int i = 0; i < numWorkersToCreate; i++)
    {
        Worker::Ptr worker = this->createWorker();
        if(!worker) break;

        worker->Start();
        ++numCreated;
    }
    return numCreated;
}

/**
 * Create an executor thread if QoS allows
 */
Worker::Ptr ThreadPool::createWorker()
{
    Worker::Ptr worker = this->state().Executors()->Create(
        this->state().Name(),
        this->state().ExecutorWaitSet(),
        this->config().DefaultWorkerPolicy()
    );

    if(!worker) return Worker::Ptr();

    this->state().SchedulerWaitSet()->Attach(worker->state().Guard());

    return worker->GetPtr();
}

}}
