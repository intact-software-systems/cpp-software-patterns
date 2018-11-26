#pragma once

#include"RxConcurrent/CommonDefines.h"
#include"RxConcurrent/FutureTask.h"
#include"RxConcurrent/FutureFactory.h"

#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent
{

/**
 * @brief The FutureTasks class
 *
 * TODO:
 * - Use FutureTask::Ptr
 * - GetHandles based on Return type: GetHandles<Return>
 * - GetHandles based on Class: GetHandles<Class>
 * - GetHandles based on Function: GetHandles<Function>
 * - Add possibility of a listener
 * - Add possibility to execute individual futuretasks attached, based on instancehandle, etc
 *
 * - NB! Make thread-safe - use thread-safe container for the future-tasks
 */
class DLL_STATE FutureTasks
        : public Templates::RunnableFuture
        //, public Templates::NoCopy<FutureTasks>
        , public ENABLE_SHARED_FROM_THIS(FutureTasks)
{
public:
    FutureTasks();

    virtual ~FutureTasks();

    CLASS_TRAITS(FutureTasks)

    FutureTasks::Ptr GetPtr();

    /**
     * @brief run
     *
     * TODO: Refactor to computation strategy!
     *
     * Use FutureTask in GuardCondition?
     * Investigate the DDS policies to be used in subjects.
     */
    virtual void run();

    void RunTasks(const InstanceHandleSet &handles);

    virtual bool Interrupt();
    virtual bool IsInterrupted() const;
    virtual bool Cancel();
    virtual bool IsCancelled() const;
    virtual bool IsDone() const;

//    virtual bool TryAgain()
//    {
//        MutexLocker lock(&mutex_);
//        return tryFinishedTasksAgain();
//    }

    virtual bool WaitFor(int64 msecs = LONG_MAX) const;
    virtual bool IsSuccess() const;

    virtual const Status::ExecutionStatus   &StatusConst() const;
    virtual Status::ExecutionStatus         &Status();

    void Clear();

public:
    bool Cancel(InstanceHandle handle);
    bool IsDone(InstanceHandle handle) const;

    InstanceHandleSet GetHandles() const;
    InstanceHandleSet GetNHandlesFIFO(unsigned int number) const;

    InstanceHandleSet Merge(FutureTasks::Ptr futureTasks);
    InstanceHandleSet Merge(FutureTasks::Ptr futureTasks, const InstanceHandleSet &handles);

public:
    template <typename Return>
    InstanceHandleSet GetHandlesWithReturnType() const
    {
        InstanceHandleSet handleSet;
        for(ListFutureTasks::const_iterator it = futureTasks_.begin(), it_end = futureTasks_.end(); it != it_end; ++it)
        {
            if(this->getFutureTask<Return>(it->first))
                handleSet.insert(it->first);
        }

        return handleSet;
    }

public:
    InstanceHandle AddTask(Templates::RunnableFuture::Ptr futureTask);

    template <typename Return, typename Class>
    InstanceHandle AddTask(Class* action, Return (Class::*member)() )
    {
        typename FutureTask<Return>::Ptr futureTask = FutureFactory::Create<Return, Class>(action, member);

        MutexLocker lock(&mutex_);
        return insertFutureTask(futureTask->GetPtr());
    }

    template <typename Return, typename Class, typename Arg1>
    InstanceHandle AddTask(Class* action, Return (Class::*member)(Arg1), Arg1 arg1)
    {
        typename FutureTask<Return>::Ptr futureTask = FutureFactory::Create<Return, Class, Arg1>(action, member, arg1);

        MutexLocker lock(&mutex_);
        return insertFutureTask(futureTask->GetPtr());
    }

    template <typename Return, typename Class, typename Arg1, typename Arg2>
    InstanceHandle AddTask(Class* action, Return (Class::*member)(Arg1, Arg2), Arg1 arg1, Arg2 arg2)
    {
        typename FutureTask<Return>::Ptr futureTask = FutureFactory::Create<Return, Class, Arg1, Arg2>(action, member, arg1, arg2);

        MutexLocker lock(&mutex_);
        return insertFutureTask(futureTask->GetPtr());
    }

public:
    bool RemoveTask(InstanceHandle handle);

    size_t RemoveTasks(InstanceHandleSet handles);

    bool RemoveFinishedTasks();

public:
    template <typename Return>
    typename FutureTask<Return>::Ptr GetTask(InstanceHandle handle) const
    {
        return getFutureTask<Return>(handle);
    }

    Templates::RunnableFuture::Ptr GetTaskBase(InstanceHandle handle) const;

    template <typename Return>
    typename FutureTask<Return>::ResultHolder Result(InstanceHandle handle) const
    {
        typename FutureTask<Return>::Ptr futureTask = getFutureTask<Return>(handle);
        if(!futureTask) return typename FutureTask<Return>::ResultHolder(Status::FutureStatus::Failed(), Return());

        return futureTask->Result();
    }

    template <typename Return>
    typename FutureTask<Return>::ResultHolder WaitForResult(InstanceHandle handle, int64 msecs = LONG_MAX) const
    {
        typename FutureTask<Return>::Ptr futureTask = getFutureTask<Return>(handle);
        if(!futureTask) return typename FutureTask<Return>::ResultHolder(Status::FutureStatus::Failed(), Return());

        return futureTask->Result(msecs);
    }

private:
    InstanceHandle insertFutureTask(Templates::RunnableFuture::Ptr futureTask);

    bool removeFutureTask(InstanceHandle handle);
    bool removeFinishedTasks();

    size_t removeFutureTasks(const InstanceHandleSet &tasks);

    Templates::RunnableFuture::Ptr getRunnableFuture(InstanceHandle handle) const;

    template <typename Return>
    typename FutureTask<Return>::Ptr getFutureTask(InstanceHandle handle) const
    {
        Templates::RunnableFuture::Ptr futureBase = getRunnableFuture(handle);
        if(!futureBase) return typename FutureTask<Return>::Ptr();

        typename FutureTask<Return>::Ptr futureTask = std::dynamic_pointer_cast<FutureTask<Return>>(futureBase);
        if(!futureTask) return typename FutureTask<Return>::Ptr();

        return futureTask->GetPtr();
    }

//    bool tryFinishedTasksAgain()
//    {
//        bool foundTasksToRetry = false;
//        for(ListFutureTasks::iterator it = futureTasks_.begin(), it_end = futureTasks_.end(); it != it_end; ++it)
//        {
//            RunnableFuture::Ptr future = it->second;

//            if(!future)
//            {
//                // -- debug --
//                IWARNING() << "Future is NULL!";
//                ASSERT(future);
//                // -- debug --
//                continue;
//            }

//            if(future->IsCancelled() || future->IsDone())
//            {
//                future->TryAgain();
//                foundTasksToRetry = true;
//            }
//        }

//        return foundTasksToRetry;
//    }

    void wakeupWaiters() const;

private:
    typedef Collection::IMap<InstanceHandle, Templates::RunnableFuture::Ptr >    ListFutureTasks;

    ListFutureTasks futureTasks_;
    InstanceHandle  currentHandle_;

    Status::ExecutionStatus status_;

    Templates::ProtectedBool isDone_;
    Templates::ProtectedBool isCancelled_;

    mutable WaitCondition waitCondition_;
    mutable Mutex mutex_;
};

}}
