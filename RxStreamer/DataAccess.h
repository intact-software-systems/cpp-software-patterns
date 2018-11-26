#pragma once

#include"RxStreamer/IncludeExtLibs.h"
#include"RxStreamer/DataAccessPolicy.h"
#include"RxStreamer/Export.h"

namespace Reactor
{

class DataAccessStatus;

// ---------------------------------------------------
// DataAccessBase
// ---------------------------------------------------

class DataAccessBase
        : public Templates::SubscribeMethod<bool>
        , public Templates::IsSuccessMethod
        , public Templates::IsValidMethod<bool>
        , public Templates::UnsubscribeMethod<bool>
        , public Templates::IsSubscribedMethod
        , public Templates::RefreshMethod<bool>
        , public Templates::StatusConstMethod<DataAccessStatus>
{
public:
    virtual ~DataAccessBase()
    { }

    CLASS_TRAITS(DataAccessBase)

    virtual RxData::CacheDescription CacheId() const = 0;

    virtual RxObserverSubjectNew<DataAccessBase::Ptr> &dataAccessSubject() = 0;
};

// ---------------------------------------------------
// DataAccessStatus
// ---------------------------------------------------

class DataAccessStatus
{
public:
    DataAccessStatus()
    { }
    virtual ~DataAccessStatus()
    { }

    Status::ExecutionStatus &executionStatus()
    {
        return executionStatus_;
    }

    const Status::ExecutionStatus &executionStatus() const
    {
        return executionStatus_;
    }

    Status::AccessStatus &accessStatus()
    {
        return accessStatus_;
    }

    const Status::AccessStatus &accessStatus() const
    {
        return accessStatus_;
    }

private:
    Status::ExecutionStatus executionStatus_;
    Status::AccessStatus accessStatus_;
};

// ---------------------------------------------------
// DataAccessState
// ---------------------------------------------------

template<typename K, typename V>
class DataAccessState
{
public:
    typedef IMap<K, V> Return;
    typedef KeyValueRxEventsSubject<DataAccessBase *, Return> AccessSubject;

public:
    DataAccessState(RxData::CacheDescription description, RxThreadPool::Ptr pool)
            : description_(description)
            , commandController_()
            , accessSubject_()
            , dataAccessSubject_()
            , subscription_(CommandControllerSubscription<Return>::NoOp())
            , threadPool_(pool)
    { }
    virtual ~DataAccessState()
    { }

    RxData::ObjectAccessProxy<V, K> Access()
    {
        return RxData::ObjectAccessProxy<V, K>(cache(), GetTypeName());
    }

    typename RxData::ObjectHome<V, K>::Ptr Home()
    {
        return cache()->template getOrCreateHome<V, K>(GetTypeName());
    }

    typename CommandControllerKeyValue<K, V>::Ptr Executor() const
    {
        return commandController_;
    }

    bool SetController(typename CommandControllerKeyValue<K, V>::Ptr controller)
    {
        commandController_ = controller;
        return commandController_ != nullptr;
    }

    bool hasController() const
    {
        return commandController_ != nullptr;
    }

    AccessSubject &accessSubject()
    {
        return accessSubject_;
    }

    RxObserverSubjectNew<DataAccessBase::Ptr> &dataAccessSubject()
    {
        return dataAccessSubject_;
    }

    CommandControllerSubscription<Return> &subscription()
    {
        return subscription_;
    }

    const CommandControllerSubscription<Return> &subscription() const
    {
        return subscription_;
    }

    void SetSubscription(CommandControllerSubscription<Return> subscription)
    {
        subscription_ = subscription;
    }

    bool isSubscribed() const
    {
        return commandController_ != nullptr && commandController_->IsExecuting();
    }

    void SetControllerPool(RxThreadPool::Ptr threadPool)
    {
        threadPool_ = threadPool;
    }

    RxThreadPool::Ptr ControllerPool() const
    {
        return threadPool_;
    }

    RxData::CacheDescription CacheId() const
    {
        return description_;
    }

private:
    std::string GetTypeName() const
    {
        return BaseLib::Utility::GetTypeName<V>();
    }

    RxData::Cache::Ptr cache()
    {
        return RxData::CacheFactory::Instance().getOrCreateCache(description_);
    }

private:
    RxData::CacheDescription description_;
    typename CommandControllerKeyValue<K, V>::Ptr commandController_;

    AccessSubject accessSubject_;
    RxObserverSubjectNew<DataAccessBase::Ptr> dataAccessSubject_;

    CommandControllerSubscription<Return> subscription_;

    RxThreadPool::Ptr threadPool_;
};

// ---------------------------------------------------
// DataAccess
// ---------------------------------------------------

template<typename K, typename V>
class DataAccess
      : public DataAccessBase
      , public Templates::ResultMethod<CommandControllerSubscription<IMap<K, V>>>
      , public RxObserverNew<IMap<K, V>>
      , public RxData::ObjectObserver<V>
      , public Templates::LockableType<DataAccess<K, V>>
      , public Templates::ContextObjectShared<DataAccessPolicy, DataAccessState<K, V>, DataAccessStatus>
      , public ENABLE_SHARED_FROM_THIS_T2(DataAccess, K, V)
{
public:
    typedef IMap<K, V> Return;
    typedef MutexTypeLocker<DataAccess<K, V>> Locker;

public:
    DataAccess(RxData::CacheDescription description, DataAccessPolicy policy = DataAccessPolicy::Default(), RxThreadPool::Ptr pool = RxThreadPoolFactory::ControllerPool())
            : RxObserverNew<IMap<K, V>>()
            , Templates::ContextObjectShared<DataAccessPolicy, DataAccessState<K, V>, DataAccessStatus>
                    (
                            new DataAccessPolicy(policy),
                            new DataAccessState<K, V>(description, pool),
                            new DataAccessStatus()
                    )
    { }

    virtual ~DataAccess()
    {
        this->data()->Home()->Disconnect(this);
        if(this->data()->hasController())
        {
            this->data()->Executor()->RxSubject().state().Unsubscribe(this->shared_from_this());
        }
    }

    CLASS_TRAITS(DataAccess)

    typename DataAccess<K, V>::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    // ---------------------------------------------------------------
    // Interface DataAccessBase
    // ---------------------------------------------------------------

    virtual bool Subscribe()
    {
        Locker locker(this);
        return this->doSubscribe();
    }

    virtual bool Refresh()
    {
        Locker locker(this);
        if(!this->data()->hasController()) return false;

        this->data()->Executor()->Trigger();
        return true;
    }

    virtual bool Unsubscribe()
    {
        Locker locker(this);
        return this->doUnsubscribe();
    }

    virtual bool IsSubscribed() const
    {
        Locker locker(this);
        return this->data()->isSubscribed();
    }

    virtual bool IsSuccess() const
    {
        Locker locker(this);
        if(!this->data()->hasController()) return false;

        return this->data()->Executor()->IsSuccess();
    }

    virtual bool IsValid() const
    {
        return true;
    }

    virtual CommandControllerSubscription<Return> Result() const
    {
        return this->data()->subscription();
    }

    virtual const DataAccessStatus& StatusConst() const
    {
        return this->status().delegate();
    }

    // ---------------------------------------------------------------
    // Add command
    // ---------------------------------------------------------------

    bool Add(typename CommandActions<Return>::Ptr action)
    {
        Locker locker(this);
        return this->getOrCreateController()->Add(action);
    }

    bool SetCommands(CommandsGroup commands)
    {
        Locker locker(this);
        return this->getOrCreateController()->SetCommands(commands);
    }

    void SetControllerPool(RxThreadPool::Ptr threadPool)
    {
        Locker locker(this);
        this->data()->SetControllerPool(threadPool);
    }

    virtual RxObserverSubjectNew<DataAccessBase::Ptr> &dataAccessSubject()
    {
        return this->data()->dataAccessSubject();
    }

    // ------------------------------------------------------
    // Access the cache and executor
    // ------------------------------------------------------

    RxData::ObjectAccessProxy<V, K> Data()
    {
        return this->data()->Access();
    }

    CommandController::Ptr Executor() const
    {
        return this->data()->Executor();
    }

    virtual RxData::CacheDescription CacheId() const
    {
        return this->data()->CacheId();
    }

    // ------------------------------------------------------
    // Access to the subject
    // ------------------------------------------------------

    typename DataAccessState<K, V>::AccessSubject &Subject()
    {
        return this->data()->accessSubject();
    }

    // ------------------------------------------------------
    // interface RxObserver
    // ------------------------------------------------------

    virtual bool OnComplete()
    {
        IINFO() << "Completed commands";
        Locker locker(this);
        processOnComplete();
        return true;
    }

    virtual bool OnNext(Return values)
    {
        IINFO() << "Next value " << values;

        if(this->status()->executionStatus().IsFailed())
        {
            IINFO() << "Execution failed, ignoring next value";
            return true;
        }

        Locker locker(this);
        processOnNext(values);
        return true;
    }

    virtual bool OnError(BaseLib::GeneralException exception)
    {
        IINFO() << "Error command" << exception.what();

        Locker locker(this);
        processOnError(exception);
        return true;
    }

    // --------------------------------------------------------
    // Interface ObjectObserver<K>
    // --------------------------------------------------------

    virtual bool OnObjectCreated(V )
    {
        if(this->config()->Reload().IsOnCreate())
        {
            if(isDonePrivate())
            {
                IINFO() << "DataAccess is done. Ignoring reload.";
                return true;
            }

            doSubscribe();
        }

        return true;
    }

    virtual bool OnObjectDeleted(V )
    {
        if(this->config()->Reload().IsOnDelete())
        {
            if(isDonePrivate())
            {
                IINFO() << "DataAccess is done. Ignoring reload.";
                return true;
            }

            doSubscribe();
        }

        return true;
    }

    virtual bool OnObjectModified(V )
    {
        if(this->config()->Reload().IsOnModify())
        {
            if(isDonePrivate())
            {
                IINFO() << "DataAccess is done. Ignoring reload.";
                return true;
            }

            doSubscribe();
        }

        return true;
    }

private:

    // -----------------------------------------------------------
    // private functions
    // -----------------------------------------------------------

    typename CommandControllerKeyValue<K, V>::Ptr getOrCreateController()
    {
        if(!this->data()->hasController())
        {
            typename CommandControllerKeyValue<K, V>::Ptr commandController =
                    CommandFactory::Instance().CreateCommandControllerKeyValue<K, V>(
                            this->config()->ControllerPolicy(),
                            this->data()->ControllerPool()
                    );

            this->data()->SetController(commandController);
            this->status()->accessStatus().Modify();
        }

        return this->data()->Executor();
    }

    bool doSubscribe()
    {
        if(this->data()->isSubscribed())
        {
            IINFO() << "DataAccess is already subscribed";
            return false;
        }

        this->data()->Home()->Connect(this);
        getOrCreateController()->RxSubject().state().Subscribe(this->shared_from_this());
        //this->event().Subscribe(this->shared_from_this());

        CommandControllerSubscription<Return> result = getOrCreateController()->Execute();
        this->data()->SetSubscription(result);

        this->status()->executionStatus().Start();

        return true;
    }

    bool doUnsubscribe()
    {
        if(!this->data()->hasController()) return false;

        // TODO: Wait for shutdown? Review and move to controller reset?
        bool unsubscribe = getOrCreateController()->Shutdown();
        getOrCreateController()->RxSubject().state().Unsubscribe();
        bool isReset = getOrCreateController()->Reset();
        ASSERT(isReset);

        this->data()->Home()->Disconnect(this);
        this->data()->accessSubject().state().Unsubscribe();

        this->data()->subscription().Cancel();

        return unsubscribe;
    }

    bool isDonePrivate() const
    {
        if(!this->data()->hasController())
        {
            IINFO() << "Not done";
            return false;
        }

//        if(this->status()->executionStatus().IsExecuting()) {
//            IINFO() << "Not done";
//            return false;
//        }
        if(this->data()->Executor()->IsExecuting())
        {
            IINFO() << "Not done";
            return false;
        }

        bool inAttempt = Strategy::IsInAttempt::Perform(
                this->status()->executionStatus(),
                this->config()->attempt()
        );
        bool inLifespan = Strategy::IsInLifetime::Perform(
                this->status()->accessStatus(),
                this->config()->lifetime()
        );

        IINFO() << "Is done? " << (!inAttempt && !inLifespan) << " || " << !inLifespan;

        return (!inAttempt && !inLifespan) || !inLifespan;
    }

    // -----------------------------------------------------------
    // private functions to process callbacks
    // -----------------------------------------------------------

    void processOnNext(const IMap<K, V> &values)
    {
        this->data()->Access().WriteAll(values);
        this->data()->accessSubject().event().Next(this, values);
        this->data()->dataAccessSubject().event().Next(this->GetPtr());

        if(this->config()->leasePlan().IsRenewOnWrite() || this->config()->leasePlan().IsRenewOnAccess())
        {
            this->status()->accessStatus().RenewLease();
        }
    }

    void processOnError(BaseLib::GeneralException exception)
    {
        this->status()->executionStatus().Failure();

        this->data()->accessSubject().event().Error(this, exception);
        this->data()->dataAccessSubject().event().Error(exception);
    }

    void processOnComplete()
    {
        this->status()->executionStatus().Success();

        this->data()->accessSubject().event().Complete(this);
        this->data()->dataAccessSubject().event().Complete();
    }
};

}
