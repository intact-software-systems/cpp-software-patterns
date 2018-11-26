#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/ReactorFactory.h"
#include"RxStreamer/DataAccessControllerPolicy.h"
#include"RxStreamer/DataAccessGroupChain.h"
#include"RxStreamer/DataAccessGroupChainIterator.h"

#include"RxStreamer/Export.h"

namespace Reactor {

class DataAccessController;

FORWARD_CLASS_TRAITS(DataAccessController)

// ---------------------------------------------------
// DataAccessControllerData
// ---------------------------------------------------

class DLL_STATE DataAccessControllerData
{
public:
    typedef Concurrent::Scheduler<RxThreadPool::Ptr, Templates::Schedulable<Duration>*> Scheduler;

public:
    DataAccessControllerData(DataAccessGroupChain chain, RxThreadPool::Ptr pool);
    virtual ~DataAccessControllerData();

    Templates::BooleanTrigger      & triggered();
    const Templates::BooleanTrigger& triggered() const;

    DataAccessControllerData::Scheduler& scheduler();

    DataAccessGroupChain      & chain();
    const DataAccessGroupChain& chain() const;

    const DataAccessGroupChainIterator& iterator() const;
    DataAccessGroupChainIterator      & iterator();

    bool Initialize(DataAccessController* controller);

    Mutex& schedulingMutex();

private:
    Templates::BooleanTrigger triggered_;
    RxThreadPool::Ptr         pool_;
    Scheduler                 scheduler_;

    DataAccessGroupChain         chain_;
    DataAccessGroupChainIterator iterator_;

    Mutex schedulingMutex_;
};

// ---------------------------------------------------
// DataAccessControllerBase
// ---------------------------------------------------

class DLL_STATE DataAccessControllerBase
    : public Templates::IsSuccessMethod
      , public Templates::CancelMethod
      , public Templates::SubscribeMethod<bool>
      , public Templates::UnsubscribeMethod<bool>
      , public Templates::IsSubscribedMethod
      , public Templates::IsCancelledMethod
      , public Templates::RefreshMethod<bool>
      , public Templates::WaitForMethod1<bool, int64>
      , public Templates::PolicyMethod<DataAccessControllerPolicy>
{
public:
    virtual ~DataAccessControllerBase()
    { }

    CLASS_TRAITS(DataAccessControllerBase)
};

// ---------------------------------------------------
// DataAccessController
// ---------------------------------------------------

class DLL_STATE DataAccessController
    : public DataAccessControllerBase
      , public RxObserverNew<DataAccessBase::Ptr>
      , public Templates::Schedulable<Duration>
      , public Templates::NotifyableShared<DataAccessController>
      , public Templates::ContextObjectShared<DataAccessControllerPolicy, DataAccessControllerData, Status::ExecutionStatus>
      , public ENABLE_SHARED_FROM_THIS(DataAccessController)
{
private:
    static const int64 ACQUIRE_LOCK_TIMEOUT_IN_MS = 5000;

public:
    DataAccessController(DataAccessControllerPolicy policy, DataAccessGroupChain chain, RxThreadPool::Ptr pool);
    virtual ~DataAccessController();

    CLASS_TRAITS(DataAccessController)

    DataAccessController::Ptr GetPtr();

    // ---------------------------------------------------
    // Schedulable interface
    // ---------------------------------------------------

    virtual void     run();
    virtual bool     HasNext() const;
    virtual Duration Next();

    // ---------------------------------------------------
    // DataAccessControllerBase interface
    // ---------------------------------------------------

    virtual DataAccessControllerPolicy Policy() const;

    virtual bool Cancel();
    virtual bool Refresh();
    virtual bool IsSubscribed() const;
    virtual bool IsCancelled() const;
    virtual bool Unsubscribe();
    virtual bool Subscribe();
    virtual bool IsSuccess() const;
    virtual bool WaitFor(int64 msecs = LONG_MAX) const;

    // ---------------------------------------------------
    // RxObserverNew<DataAccessBase::Ptr> interface
    // ---------------------------------------------------

    virtual bool OnComplete();
    virtual bool OnNext(DataAccessBase::Ptr dataAccess);
    virtual bool OnError(GeneralException exception);

private:

    bool scheduleFromRun();
    bool schedule();

    // -----------------------------------------------------------
    // process call-backs
    // -----------------------------------------------------------

    bool processOnNext(DataAccessBase::Ptr dataAccess);
    bool processOnError(BaseLib::GeneralException exception);

    // ---------------------------------------------------
    // private functions
    // ---------------------------------------------------

    bool execute(DataAccessGroupIterator group, DataAccessGroupPolicy policy);
    bool executeAll(DataAccessGroupIterator group);
    bool executeNext(DataAccessGroupIterator group);
    bool setupNext();

    bool executionStart();
    void executionFinished();
    void executionFailure(Exception exception);
    void executionCancelled(Exception exception);

    void connect(DataAccessGroup group);
    void disconnect(DataAccessGroup group);
    void unsubscribeAndDisconnect(DataAccessGroup group);

    bool isReady();
    bool isTimeout();

    bool isExecutionFailure();
};

}
