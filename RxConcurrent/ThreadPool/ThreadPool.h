#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/GuardCondition.h"
#include"RxConcurrent/WaitSet.h"
#include"RxConcurrent/FutureFactory.h"
#include"RxConcurrent/ScheduledFutureTask.h"

#include"RxConcurrent/ThreadPool/ThreadPoolPolicy.h"
#include"RxConcurrent/ThreadPool/ThreadPoolBase.h"
#include"RxConcurrent/ThreadPool/Workers.h"
#include"RxConcurrent/ThreadPool/ScheduledTasks.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

// ------------------------------------------------------
// ThreadPoolConfig
// ------------------------------------------------------

class DLL_STATE ThreadPoolConfig
{
public:
    explicit ThreadPoolConfig(ThreadPoolPolicy policy = ThreadPoolPolicy::Default());
    virtual ~ThreadPoolConfig();

    const ThreadPoolPolicy& policy() const;

    const WorkersPolicy &DefaultWorkersPolicy() const;
    const WorkerPolicy  &DefaultWorkerPolicy() const;

private:
    ThreadPoolPolicy policy_;
    WorkersPolicy defaultWorkersPolicy_;
    WorkerPolicy defaultWorkerPolicy_;
};

// ------------------------------------------------------
// ThreadPoolState
// ------------------------------------------------------

class DLL_STATE ThreadPoolState
{
public:
    typedef std::string PoolName;

public:
    explicit ThreadPoolState(PoolName poolName = PoolName("Default"));
    virtual ~ThreadPoolState();

    Workers::Ptr        Executors();
    ScheduledTasks::Ptr Tasks();

    WaitSet::Ptr ExecutorWaitSet();
    WaitSet::Ptr SchedulerWaitSet();

    std::shared_ptr<TaskScheduler> Scheduler();

    void SetTaskScheduler(const std::shared_ptr<TaskScheduler> &scheduler);

    const PoolName &Name() const;

    Status::ExecutionStatus       &Status();
    const Status::ExecutionStatus &Status() const;

private:
    /**
     * @brief tasks_ collection of all task executors (Runnable), no state related to individual TASK instances
     */
    Workers::Ptr workers_;

    /**
     * @brief taskWrappers_ collection of all TASK instances which are triggered by task scheduler using WaitSet/Condition
     */
    ScheduledTasks::Ptr tasks_;

    /**
     * @brief waitSet_ contains all the conditions to execute tasks, and is used by TaskExecutors
     */
    WaitSet::Ptr waitSet_;

    /**
     * @brief waitSetExecutorToPool_ used by TaskExecutors to wake up ThreadPool:run()
     */
    WaitSet::Ptr waitSetExecutorToPool_;

    /**
     * @brief taskScheduler_ optional parameter which (if set) overrides default task scheduler, run by internal Thread
     */
    std::shared_ptr<TaskScheduler> taskScheduler_;

    Status::ExecutionStatus status_;

    /**
     * @brief poolName_
     * TODO: Use as ThreadPoolId?
     */
    PoolName poolName_;
};

// ------------------------------------------------------
// ThreadPool
// ------------------------------------------------------

/**
 * ThreadPool is the scheduler and notifyer, TaskExecutors perform the work, calling TASK->run()
 *
 * Issues:
 * - DONE: Stop and start task executor threads based on load = number-of-workers and performance in terms of QoSFrequency.
 * - TODO: Enable the threadPool.run() to detect whether there are long running tasks that starves the rest of the tasks.
 * - TODO: QosPool's priority and frequency are not used....
 * - TODO: Is it possible to use a min-heap for the scheduling of tasks?
 * - TODO: Enable listeners to the thread-pool to get the state of their TASK? State of ThreadPool?
 * - TODO: Control "cancelling of tasks".
 * - TODO: Reject tasks if all threads are tied up in cancellation "dead-lock" or "stale-mate".
 * - TODO: User defined thread factory
 *
 * - DONE: Exception handling, fault-tolerance for TaskExecutors that execute tasks that throw exceptions,
 *      currently these Threads die leaving them in the thread-pool's map!
 * - DONE: When adding tasks, let caller receive a handle that lets it remove/suspend/trigger the task without
 *          getting the actual ThreadPool
 *
 * - DONE: Create factory for ThreadPool: Stores all ThreadPools in one factory object, etc.
 * - DONE: Use Thread::idealThreadCount() to determine the ideal size of ThreadPool?
 * - DONE: Support for "One-Off" functions? Execute once and then automatically remove from ThreadPool
 * - DONE: Make scheduleNextTask more efficient by scheduling more than one task at any time....
 * - DONE: Enable an external task-scheduler to override the QosTask policies for any task. For example, the QosFrequency
 * - DONE: Optional to start internal thread-pool scheduler, with new TriggerTask, it is possible to do this from outside!
 * - DONE: Support for setting internal scheduler that overrides the current built-in scheduler
 * - DONE: Functions StartScheduler, StopScheduler (thread), SetScheduler(TaskScheduler)
 * - DONE: Support Resume and Suspend
 */
class DLL_STATE ThreadPool
        : public ThreadPoolBase
        , public Templates::EntryMethod1<ScheduledRunnableTask::Ptr>
        , public Templates::ExitMethod2<ScheduledRunnableTask::Ptr, Status::ExecutionStatus>
        , public Thread
        , public Templates::LockableType<ThreadPool, Mutex>
        , public Templates::NoCopy<ThreadPool>
        , public Templates::ContextObject<ThreadPoolConfig, ThreadPoolState>
        , public ENABLE_SHARED_FROM_THIS(ThreadPool)
{
public:
    explicit ThreadPool(PoolName poolName, ThreadPoolPolicy policy = ThreadPoolPolicy::Default());

    ~ThreadPool() override;

    CLASS_TRAITS(ThreadPool)

    ThreadPool::Ptr GetPtr();

    // ----------------------------------------------------
    // Task management
    // ----------------------------------------------------

    ScheduledFutureTask<bool> Trigger(BaseLib::Runnable *task);

    ScheduledFutureTask<bool> Schedule(BaseLib::Runnable* task, Duration duration, bool createAsSuspended = false);
    ScheduledFutureTask<bool> Schedule(BaseLib::Runnable *task, TaskPolicy taskPolicy, bool createAsSuspended = false);
    ScheduledFutureTask<bool> Schedule(std::shared_ptr<BaseLib::Runnable> task, TaskPolicy taskPolicy, bool createAsSuspended = false);

    ScheduledRunnableTask::Ptr Remove(std::shared_ptr<Templates::ScheduledRunnableFuture> task);

    // ----------------------------------------------------
    // Templated task scheduling with return type
    // ----------------------------------------------------

    template <typename Return>
    ScheduledFutureTask<Return> Schedule(std::shared_ptr<FutureTask<Return>> future, TaskPolicy taskPolicy, bool createAsSuspended = false)
    {
        Locker lock(this);
        return schedule<Return>(future, taskPolicy, Function::defaultScheduleFunction, createAsSuspended);
    }

    template <typename Return>
    ScheduledFutureTask<Return> Schedule(std::function<Return()> func, TaskPolicy taskPolicy, bool createAsSuspended = false)
    {
        std::shared_ptr<FutureTask<Return>> future = FutureFactory::Create<Return>(func);

        Locker lock(this);
        return schedule<Return>(future, taskPolicy, Function::defaultScheduleFunction, createAsSuspended);
    }

    template <typename Return>
    ScheduledFutureTask<Return> Schedule(std::function<Return()> func, ScheduleFunction scheduler, TaskPolicy taskPolicy, bool createAsSuspended = false)
    {
        std::shared_ptr<FutureTask<Return>> future = FutureFactory::Create<Return>(func);

        Locker lock(this);
        return schedule<Return>(future, taskPolicy, scheduler, createAsSuspended);
    }

    // ----------------------------------------------------
    // Get list of tasks ready for execution
    // ----------------------------------------------------

    Duration FindReady(ScheduledTasksState::ListTaskBase &readyTasks) override;

    // ----------------------------------------------------
    // Interface ThreadPoolBase
    // ----------------------------------------------------

    bool Stop(int64 msecs = LONG_MAX) override;
    void Start() override;

    bool StopScheduler(int64 msecs = LONG_MAX) override;
    bool StartScheduler() override;

    void ClearTasks() override;

    const PoolName &GetPoolName() const override;

    void SetTaskScheduler(TaskScheduler::Ptr taskScheduler) override;
    void UseDefaultTaskScheduler() override;

    int StartWorkers(int numToRestart);

    // ----------------------------------------------------
    // Callback from Worker through WaitSet and GuardCondition
    // ----------------------------------------------------

    /**
     * @brief operator () callback from all Worker::run() using WaitSet
     *
     * NOTE! Must be highly concurrent implementation to enable efficiency and scalability!
     */
    void operator()(ScheduledRunnableTask::Ptr &task);

    void Entry(ScheduledRunnableTask::Ptr &) override;
    void Exit(ScheduledRunnableTask::Ptr &, Status::ExecutionStatus &status) override;

    // ----------------------------------------------------
    // Status reporting
    // ----------------------------------------------------

    std::ostream &Report(std::ostream &ostr);

private:
    // ----------------------------------------------------
    // Thread running
    // ----------------------------------------------------

    void run() override;

    void schedule();
    void manageTasks();
    void manageWorkers();

    bool isDefaultScheduler();

    static TaskScheduler::Ptr getExternalScheduler(ThreadPool *pool);

    // ----------------------------------------------------
    // Task managment
    // ----------------------------------------------------

    ScheduledRunnableTask::Ptr remove(Templates::ScheduledRunnableFuture::Ptr task);

    // ----------------------------------------------------
    // Templated task scheduling with return type
    // ----------------------------------------------------

    template <typename Return>
    ScheduledFutureTask<Return> schedule(std::shared_ptr<FutureTask<Return>> task, TaskPolicy taskPolicy, const ScheduleFunction& scheduleFunction, bool createAsSuspended)
    {
        ASSERT(task);

        // ------------------------------
        // Create and add Task
        // ------------------------------
        std::shared_ptr<ScheduledRunnableTask> future = this->state().Tasks()->Add(task->GetPtr(), taskPolicy, scheduleFunction);

        // --------------------------------------
        // Initialize Guard condition and WaitSet
        // --------------------------------------
        future->state().Cond()-> template SetHandler<ThreadPool, std::shared_ptr<ScheduledRunnableTask>>(this, future->GetPtr());
        this->state().ExecutorWaitSet()->Attach(future->state().Cond()->GetPtr());

        if(createAsSuspended)
        {
            future->Suspend();
        }

        this->state().SchedulerWaitSet()->Attach(future->state().SchedulerCond()->GetPtr());
        this->state().SchedulerWaitSet()->WakeAll();

        std::shared_ptr<ScheduledTask<Return>> scheduledTask( new ScheduledTask<Return>(task->GetPtr(), future->GetPtr(), taskPolicy));

        return ScheduledFutureTask<Return>(scheduledTask->GetPtr());
    }

    // ----------------------------------------------------
    // Worker managment
    // ----------------------------------------------------

    int createWorkers(int numWorkersToCreate);

    Worker::Ptr createWorker();
};

}}

