#include "RxStreamer/DataAccessController.h"

namespace Reactor {

// ---------------------------------------------------
// DataAccessControllerData
// ---------------------------------------------------

DataAccessControllerData::DataAccessControllerData(DataAccessGroupChain chain, RxThreadPool::Ptr pool)
    : triggered_(false)
    , pool_(pool)
    , scheduler_(pool, Templates::SchedulableNoOp<Duration>::InstancePtr().get())
    , chain_(chain)
    , iterator_(chain)
{ }

DataAccessControllerData::~DataAccessControllerData()
{ }

Templates::BooleanTrigger& DataAccessControllerData::triggered()
{
    return triggered_;
}

const Templates::BooleanTrigger& DataAccessControllerData::triggered() const
{
    return triggered_;
}

DataAccessControllerData::Scheduler& DataAccessControllerData::scheduler()
{
    return scheduler_;
}

DataAccessGroupChain& DataAccessControllerData::chain()
{
    return chain_;
}

const DataAccessGroupChain& DataAccessControllerData::chain() const
{
    return chain_;
}

const DataAccessGroupChainIterator& DataAccessControllerData::iterator() const
{
    return iterator_;
}

DataAccessGroupChainIterator& DataAccessControllerData::iterator()
{
    return iterator_;
}

bool DataAccessControllerData::Initialize(DataAccessController* controller)
{
    this->scheduler_ = DataAccessControllerData::Scheduler(pool_, controller);
    return true;
}

Mutex& DataAccessControllerData::schedulingMutex()
{
    return schedulingMutex_;
}

// ---------------------------------------------------
// DataAccessController
// ---------------------------------------------------

DataAccessController::DataAccessController(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxThreadPool::Ptr pool)
    : Templates::ContextObjectShared<DataAccessControllerPolicy, DataAccessControllerData, Status::ExecutionStatus>
    (
        new DataAccessControllerPolicy(policy),
        new DataAccessControllerData(chain, pool),
        new Status::ExecutionStatus()
    )
{
    this->data()->Initialize(this);
}

DataAccessController::~DataAccessController()
{
    this->data()->scheduler().Finalize();
    // TODO: Stop and wait until shut down safely
}

DataAccessController::Ptr DataAccessController::GetPtr()
{
    return shared_from_this();
}

// ---------------------------------------------------
// Schedulable interface
// ---------------------------------------------------

void DataAccessController::run()
{
    bool scheduled = scheduleFromRun();
    if(!scheduled)
    {
        // TODO: False start?
        IINFO() << "Scheduler found nothing to do";
    }
}

Duration DataAccessController::Next()
{
    return BaseLib::Strategy::ComputeEarliestReadyIn::Perform(
        this->status().delegate(),
        this->config()->attempt(),
        this->config()->interval(),
        this->config()->retryInterval(),
        this->config()->timeout()
    );
}

bool DataAccessController::HasNext() const
{
    return BaseLib::Strategy::IsInAttempt::Perform(
        this->status().delegate(),
        this->config()->attempt()
    );
}

// ---------------------------------------------------
// DataAccessControllerBase interface
// ---------------------------------------------------

bool DataAccessController::Subscribe()
{
    Locker lock(this);

    // TODO: execution status should reflect "all executions" not single like today
    if(this->status()->IsExecuting())
    {
        return true;
    }
    else if(this->data()->chain().Group().empty())
    {
        return false;
    }

    this->status()->Starting();
    this->data()->scheduler().Next(0);
    return true;
}

bool DataAccessController::Unsubscribe()
{
    Locker lock(this);

    this->data()->scheduler().Complete();
    unsubscribeAndDisconnect(this->data()->iterator().Current());
    return true;
}

bool DataAccessController::Refresh()
{
    Locker lock(this);

    if(!this->data()->triggered().IsTriggered() && !this->status()->IsExecuting())
    {
        this->data()->triggered().UpdateTrigger(true);
        this->data()->scheduler().Next(0);
    }
    return true;
}

bool DataAccessController::Cancel()
{
    executionCancelled(Exception("Execution cancelled"));

    {
        Locker lock(this);
        this->wakeAll();
    }
    return false;
}

bool DataAccessController::IsSubscribed() const
{
    // TODO: Check different status. It should be a SubscriptionStatus, not ExecutionStatus
    return this->status()->IsExecuting();
}

bool DataAccessController::IsCancelled() const
{
    return this->status()->IsCancelled();
}

bool DataAccessController::IsSuccess() const
{
    return this->status()->IsSuccess();
}

bool DataAccessController::WaitFor(int64 msecs) const
{
    Locker lock(this);
    if(!this->status()->IsExecuting())
    {
        return false;
    }

    return this->waitForDone<Status::ExecutionStatus>(this->status().operator->(), msecs);
}

DataAccessControllerPolicy DataAccessController::Policy() const
{
    return this->config().Clone();
}

// ---------------------------------------------------
// RxObserverNew interface
// ---------------------------------------------------

bool DataAccessController::OnComplete()
{
    return true;
}

bool DataAccessController::OnNext(DataAccessBase::Ptr dataAccess)
{
    return processOnNext(dataAccess);
}

bool DataAccessController::OnError(GeneralException exception)
{
    return processOnError(exception);
}

// -----------------------------------------------------------
// scheduler function
// -----------------------------------------------------------

/**
 * Tries to acquire a scheduling mutex, gives up within a give threshold.
 */
bool DataAccessController::scheduleFromRun()
{
    ScopedTimer t(IINFO());

    TryMutexTypeLocker<Mutex> lock(&(this->data()->schedulingMutex()), ACQUIRE_LOCK_TIMEOUT_IN_MS);

    if(!lock.isLocked())
    {
        IWARNING() << "Could not acquire scheduling lock within " << ACQUIRE_LOCK_TIMEOUT_IN_MS << " ms";
        return false;
    }

    return schedule();
}

bool DataAccessController::schedule()
{
    if(isExecutionFailure() || isTimeout())
    {
        // TODO: currently the controller is "fail-fast" on any error
        executionFailure(Exception("Controller failed"));

        IWARNING() << "DataAccessController failed";

        {
            Locker lock(this);
            this->wakeAll();
        }

        return true;
    }
    else if(this->status()->IsCancelled())
    {
        IINFO() << "Controller is cancelled";
        return false;
    }
    else if(isReady())
    {
        bool started = executionStart();
        if(started)
        {
            bool executed = execute(this->data()->iterator().GroupIterator(), this->data()->iterator().Current().config().delegate());
            if(!executed)
            {
                IWARNING() << "Unable to trigger group!";
            }
            return executed;
        }
        else
        {
            // TODO: support exponential backoff when execution cannot be done.
            //iWarning("Unable to start execution!");
            return false;
        }
    }
    else if(!this->data()->iterator().GroupIterator().IsStarted())
    {
        bool executed = execute(this->data()->iterator().GroupIterator(), this->data()->iterator().Current().config().delegate());
        if(!executed)
        {
            IWARNING() << "Unable to trigger group!";
        }
        return executed;
    }

    return false;
}

// -----------------------------------------------------------
// process call-backs
// -----------------------------------------------------------

bool DataAccessController::processOnNext(DataAccessBase::Ptr dataAccess)
{
    // -----------------------------------------------------------------
    // Q: Do I need to call anything if is cancelled?
    // A: No, isCancelled is only possible if this.cancel() is called, which sets ExecutionStatus.
    // -----------------------------------------------------------------
    if(this->status()->IsCancelled())
    {
        return true;
    }

    // Add DataAccess to current group execution status (similar to TCP acknowledgments, "Execution control protocol")
    DataAccessGroup group = this->data()->iterator().Current();
    group.Next(dataAccess);

    DataAccessGroupIterator groupIterator = this->data()->iterator().GroupIterator();

    // ----------------------------------------
    // Double checked locking for isDone and scheduling execution of next group
    // ----------------------------------------
    if(group.IsDone())
    {
        MutexTypeLocker<DataAccessGroup> lock(&group);

        if(this->data()->iterator().Current().IsDone())
        {
            bool next = setupNext();
            if(next)
            {
                IINFO() << "Set up next group execution!";
                data()->scheduler().Next(0L);
            }
            else
            {
                IINFO() << "Time elapsed to finish: " << status()->Time().TimeSinceLastExecutionTime();
                executionFinished();

                {
                    Locker lock(this);
                    this->wakeAll();
                }
            }
        }
    }
    else if(group.IsExecuting())
    {
        switch(group.config()->Computation().Kind())
        {
            case Policy::ComputationKind::PARALLEL:
            case Policy::ComputationKind::CONCURRENT:
                IINFO() << "Ignoring executing next because Parallel";
                break;
            case Policy::ComputationKind::SEQUENTIAL:
            default:
                return executeNext(groupIterator);
        }
    }

    return true;
}

bool DataAccessController::processOnError(BaseLib::GeneralException exception)
{
    data()->iterator().Current().Error(exception);

    if(this->status()->IsExecuting())
    {
        // ------------------------------------------------------------
        // Note: Trigger error handling only if not handled previously
        // ------------------------------------------------------------
        MutexTypeLocker<Templates::BooleanTrigger> lock(&this->data()->triggered());
        if(!this->data()->triggered().IsTriggered() && this->status()->IsExecuting())
        {
            this->data()->triggered().UpdateTrigger(true);
            this->data()->scheduler().Next(0L);
        }
    }
    return true;
}

// ---------------------------------------------------
// private implementation
// ---------------------------------------------------

bool DataAccessController::execute(DataAccessGroupIterator group, DataAccessGroupPolicy policy)
{
    switch(policy.Computation().Kind())
    {
        case Policy::ComputationKind::PARALLEL:
        case Policy::ComputationKind::CONCURRENT:
            return executeAll(group);
        case Policy::ComputationKind::SEQUENTIAL:
        default:
            return executeNext(group);
    }
}

bool DataAccessController::executeAll(DataAccessGroupIterator group)
{
    while(group.HasNext() && !status()->IsCancelled())
    {
        bool executed = executeNext(group);
        if(!executed)
        {
            IWARNING() << "Failed to execute next in group!";
            return false;
        }
    }

    return true;
}

bool DataAccessController::executeNext(DataAccessGroupIterator group)
{
    if(status()->IsCancelled() || !group.HasNext())
    {
        return false;
    }

    DataAccessBase::Ptr access = group.Next();
    if(access == nullptr)
    {
        IWARNING() << "Next was null because is done == " << data()->iterator().Current().IsDone();
        return false;
    }

    return access->Subscribe();
}

bool DataAccessController::setupNext()
{
    if(!this->data()->iterator().Current().IsDone())
    {
        return false;
    }

    disconnect(this->data()->iterator().Current());

    if(!this->data()->iterator().HasNext()) // chain has no more groups to execute
    {
        IINFO() << "No actions left to execute!";
        return false;
    }
    else // chain has more groups to execute
    {
        DataAccessGroup nextGroup = this->data()->iterator().Next();
        nextGroup.Start();

        connect(nextGroup);

        IINFO() << "Setup new action group.";
    }

    return true;
}

bool DataAccessController::executionStart()
{
    this->data()->triggered().UpdateTrigger(false);
    this->data()->iterator().Rewind();

    if(!this->data()->iterator().HasNext())
    {
        return false;
    }
    else
    {
        DataAccessGroup group = this->data()->iterator().Next();
        connect(group);

        group.Start();
        this->status()->Start();

        return true;
    }
}

void DataAccessController::executionFinished()
{
    DataAccessGroup currentGroup = this->data()->iterator().Current();
    disconnect(currentGroup);

    this->status()->Success();
    this->data()->scheduler().Complete();
}

void DataAccessController::executionFailure(Exception exception)
{
    DataAccessGroup currentGroup = this->data()->iterator().Current();
    unsubscribeAndDisconnect(currentGroup);

    this->status()->Failure();
    this->data()->scheduler().Error(exception);
}

void DataAccessController::executionCancelled(Exception exception)
{
    DataAccessGroup currentGroup = this->data()->iterator().Current();
    unsubscribeAndDisconnect(currentGroup);

    this->status()->Cancel();
    this->data()->scheduler().Error(exception);
}

// -----------------------------------------------------------
// Subscription handling, i.e., observers, etc.
// -----------------------------------------------------------

void DataAccessController::connect(DataAccessGroup group)
{
    for(DataAccessBase::Ptr access : group.getList())
    {
        access->dataAccessSubject().state().Subscribe(this->GetPtr());
    }
}

void DataAccessController::disconnect(DataAccessGroup group)
{
    for(DataAccessBase::Ptr access : group.getList())
    {
        access->dataAccessSubject().state().Unsubscribe(this->GetPtr());
    }
}

void DataAccessController::unsubscribeAndDisconnect(DataAccessGroup group)
{
    for(DataAccessBase::Ptr access : group.getList())
    {
        access->Unsubscribe();
        access->dataAccessSubject().state().Unsubscribe(this->GetPtr());
    }
}

bool DataAccessController::isReady()
{
    if(this->status()->IsStarting()) return true;

    return !this->status()->IsExecuting() &&
           (this->data()->triggered().IsTriggered() ||
            BaseLib::Strategy::IsReadyToExecute::Perform(
                this->status().delegate(),
                this->config()->attempt(),
                this->config()->interval(),
                this->config()->retryInterval()
            ));
}

bool DataAccessController::isTimeout()
{
    return BaseLib::Strategy::IsTimeout::Perform(
        this->status().delegate(),
        this->config()->timeout()
    );
}

bool DataAccessController::isExecutionFailure()
{
    return this->data()->iterator().Current().IsFailure() && this->data()->iterator().Current().IsExecuting();
}

}
