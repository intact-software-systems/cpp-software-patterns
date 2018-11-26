#pragma once

#include <RxConcurrent/ThreadPool/ThreadPool.h>
#include <RxConcurrent/ThreadPool/ThreadPoolFactory.h>

#include "CommonDefines.h"
#include "FutureFactory.h"
#include "FlowController.h"
#include "FlowControllerPolicy.h"

namespace BaseLib { namespace Concurrent { namespace details {

// ----------------------------------------------------
// FlowControllerState
// - Queue - FIFO/Random/etc. If FIFO then share ringbuffer from subjects-group to all specific subject instances.
// ----------------------------------------------------

template <typename V>
class FlowControllerState
{
public:
    typedef Collection::RingBufferQueue<V> EventQueue;

public:
    FlowControllerState(
        int queueCapacity = 100,
        int historyDepth = 5,
        std::string threadPoolId = std::string(""))
        : threadPoolId_(threadPoolId)
        , scheduledTask_(std::make_shared<ScheduledFutureTaskNoOp<bool>>())
        , action_()
        , queue_(Collection::RingBufferQueuePolicy::LimitedTo(queueCapacity))
        , history_(Collection::RingBufferQueuePolicy::Overwrite(historyDepth))
    { }

    virtual ~FlowControllerState()
    { }

    bool IsInitialized() const
    {
        return scheduledTask_.IsAttached();
    }

    bool Initialize(ScheduledFutureTask<bool> scheduledTask)
    {
        scheduledTask_ = scheduledTask;
        return scheduledTask_.IsAttached();
    }

    ScheduledFutureTask<bool>& Task()
    {
        return scheduledTask_;
    }

    const ScheduledFutureTask<bool>& Task() const
    {
        return scheduledTask_;
    }

    std::shared_ptr<Templates::Action0<bool>> Action() const
    {
        return action_;
    }

    bool Set(std::shared_ptr<Templates::Action0<bool>> action)
    {
        action_ = action;
        return action_ != nullptr;
    }

    EventQueue& Queue()
    {
        return queue_;
    }

    const EventQueue& Queue() const
    {
        return queue_;
    }

    EventQueue& History()
    {
        return history_;
    }

    const EventQueue& History() const
    {
        return history_;
    }


private:
    std::string threadPoolId_;

    ScheduledFutureTask<bool> scheduledTask_;

    std::shared_ptr<Templates::Action0<bool>> action_;

    EventQueue queue_;
    EventQueue history_;
};

// ----------------------------------------------------
// FlowControllerAction
// ----------------------------------------------------

/**
 * Acts like a flow controller for producers and consumers.
 *
 * Responsibility:
 * - The controller takes into account the user defined action's policy
 * - Store last n events based on policy (History)
 * - run attached action based on interval policy
 * - TODO: report deadline violations?
 */
template <typename V>
class FlowControllerAction
    : public BaseLib::Concurrent::FlowController<V>
      , public Templates::LockableType<FlowControllerAction<V> >
      , protected Templates::ContextObjectShared<FlowControllerPolicy, FlowControllerState<V>, FlowControllerStatus>
{
public:
    FlowControllerAction(FlowControllerPolicy policy = FlowControllerPolicy::Default())
        : Templates::ContextObjectShared<FlowControllerPolicy, FlowControllerState<V>, FlowControllerStatus>
        (
            new FlowControllerPolicy(policy),
            new FlowControllerState<V>(policy.MaxQueueSize().Limit(), policy.History().Depth()),
            new FlowControllerStatus(Duration::FromMinutes(5), Count::Create(1000))
        )
    { }

    virtual ~FlowControllerAction()
    { }

    CLASS_TRAITS(FlowControllerAction)

    friend class Templates::DoubleCheckedLocking;

    // ---------------------------------------------------------
    // Set active action
    // ---------------------------------------------------------

    // TODO: Instead of setting action, just set a std::function?
    virtual bool Set(std::shared_ptr<Templates::Action0<bool>> action)
    {
        return this->data()->Set(action);
    }

    bool IsActionSet() const
    {
        return this->data()->Action() != nullptr;
    }

    int AvailableCapacity() const
    {
        return this->config()->MaxQueueSize().Limit() - this->data()->Queue().Length();
    }

    // ---------------------------------------------------------
    // Flow controller get and update status
    // ---------------------------------------------------------

    virtual void Trigger()
    {
        this->data()->Task().Trigger();
    }

    virtual void Resume()
    {
        ASSERT(!IsEmpty());
        this->data()->Task().Resume();
    }

    virtual void Suspend()
    {
        this->data()->Task().Suspend();
    }

    virtual bool IsResumable() const
    {
        return !IsEmpty() && IsSuspended();
    }

    virtual bool IsSuspendable() const
    {
        return IsEmpty() && !IsSuspended();
    }

    virtual bool IsResumed() const
    {
        return !this->data()->Task().IsSuspended();
    }

    virtual bool IsSuspended() const
    {
        return this->data()->Task().IsSuspended();
    }

    virtual bool IsEmpty() const
    {
        return this->data()->Queue().IsEmpty();
    }

    virtual const FlowControllerStatus& Status() const
    {
        return this->status().delegate();
    }

    // ---------------------------------------------------------
    // Access to events
    // ---------------------------------------------------------

    virtual std::list<V> Pull()
    {
        int numElements = INT_MAX; //Strategy::ComputeNumPacketsToDequeue::Perform(this->status().delegate(), this->config()->Throughput());

        std::list<V> events = this->data()->Queue().template DequeueUpto<std::list<V>>(numElements);

        //IINFO() << "Queue size " << this->data()->Queue().Length() << " Dequeue max: " << numElements << ". Dequeued " << events.size();

        this->status()->NextState(FlowStatusKind::DEQUEUED, events.size());

        return events;
    }

    virtual std::list<V> History() const
    {
        return this->data()->History().template PeekAll<std::list<V>>();
    }

    // ----------------------------------------------------
    // scheduled to run periodically based on subject's interval policy
    // triggered by subject to notify observer
    // One FlowController for each subject/observer
    //
    // handler = identify_handler (event);
    // handler.handle_event (event);
    // ----------------------------------------------------
    // TODO: Instead of hard-wiring an action implementation, attach a scheduler/call-back action

    virtual bool Invoke()
    {
        if(isDone())
        {
            IINFO() << "Flow controller is done!";
            return false;
        }

        // ---------------------------------------------------
        // TODO: Store dispatched events? Clean up received events
        // Check if window allows more events to be sent out
        // Check if event queue is empty
        // TODO: Make as policy: "Only execute action when events in queue"
        // ---------------------------------------------------

        if(!this->data()->Queue().IsEmpty())
        {
            Status::ExecutionStatus status;
            Strategy::DefaultActionExecution<bool>::Perform(this->data()->Action().operator*(), status);

            if(!status.IsSuccess())
            {
                ICRITICAL() << "Failed to execute action from flow controller";
                return false;
            }
        }

        // ----------------------------------------
        // TODO: Make as policy: "Suspend this task if queue is empty"
        // ----------------------------------------
        //Templates::DoubleCheckedLocking::ConditionalSuspend<FlowControllerAction<V> >(this);

        return true;
    }

    virtual bool Cancel()
    {
        return this->data()->Task().Cancel();
    }

    virtual bool IsDone() const
    {
        return this->data()->Task().IsDone();
    }

    virtual bool IsSuccess() const
    {
        return this->data()->Task().IsSuccess();
    }

    // ---------------------------------------------------------
    // Reactor methods
    // ---------------------------------------------------------

    virtual bool Next(V event)
    {
        bool initialize = Templates::DoubleCheckedLocking::Initialize<FlowControllerAction<V>>(this);
        ASSERT(initialize);

        if(isDone())
        {
            IINFO() << "Flow controller is done!";
            return false;
        }

        // -------------------------
        // cache event
        // -------------------------
        bool isPushed = this->data()->Queue().Enqueue(event, 1000);

        if(isPushed)
        {
            bool isAdded = this->data()->History().TryEnqueue(event);
            ASSERT(isAdded);

            this->status()->NextState(FlowStatusKind::ENQUEUED, 1);

            //IINFO() << "Pushed event! Queue size " << this->data()->Queue().Length() << " history " << this->data()->History().Length();

            //Templates::DoubleCheckedLocking::ConditionalResume<FlowControllerAction<V>>(this);
            this->Resume();

            return true;
        }
        else
        {
            ICRITICAL() << "Failed to push event! Queue size " << this->data()->Queue().Length() << " history " << this->data()->History().Length();
            //ASSERT(isPushed);

            this->status()->NextState(FlowStatusKind::REJECTED, 1);

            return false;
        }
    }

    virtual bool Complete()
    {
        return this->data()->Task().Cancel();
    }

    virtual bool Error(BaseLib::Exception)
    {
        return this->data()->Task().Cancel();
    }

    virtual Duration NextSchedule(const Status::ExecutionStatus&, const TaskPolicy&)
    {
        if(!this->data()->Queue().IsEmpty() && !this->data()->Task().IsExecuting())
        {
            return Duration::Zero();
        }

        return Duration::FromMilliseconds(1000);
    }

protected:

    // ---------------------------------------------------------
    // Initialize methods
    // ---------------------------------------------------------

    virtual bool Initialize()
    {
        auto action = this->shared_from_this();
        ASSERT(action);

        bool suspended = false;

        ScheduledFutureTask<bool> scheduledTask = ThreadPoolFactory::Instance().GetDefault()->template Schedule<bool>(
            [action]() -> bool { return action->Invoke(); },
            [action](const Status::ExecutionStatus& status, const TaskPolicy& policy) -> Duration { return action->NextSchedule(status, policy); },
            this->config()->Task(),
            suspended
        );

        return this->data()->Initialize(scheduledTask);
    }

    virtual bool IsInitialized() const
    {
        return this->data()->IsInitialized();
    }

private:

    bool isDone() const
    {
        return isFlowControllerDone() && isActionDone();
    }

    bool isFlowControllerDone() const
    {
        return this->data()->Task().IsCancelled() || this->data()->Task().IsDone();
    }

    bool isActionDone() const
    {
        ASSERT(this->data()->Action());
        return this->data()->Action()->IsDone();
    }
};

}}}
