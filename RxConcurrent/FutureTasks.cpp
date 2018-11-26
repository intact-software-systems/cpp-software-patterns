#include"RxConcurrent/FutureTasks.h"

namespace BaseLib { namespace Concurrent {

FutureTasks::FutureTasks()
    : isDone_(false)
    , isCancelled_(false)
{ }

FutureTasks::~FutureTasks()
{ }

FutureTasks::Ptr FutureTasks::GetPtr()
{
    return this->shared_from_this();
}

/**
 * @brief run
 *
 * TODO: Refactor to computation strategy!
 *
 * Use FutureTask in GuardCondition?
 * Investigate the DDS policies to be used in subjects.
 */
void FutureTasks::run()
{
    InstanceHandle iterator;

    for( ; iterator < currentHandle_; ++iterator)
    {
        if(IsCancelled()) break;

        Templates::RunnableFuture::Ptr future;

        {
            MutexLocker lock(&mutex_);
            future = this->getRunnableFuture(iterator);
        }

        if(!future) continue;
        else if(future->IsCancelled() || future->IsDone()) continue;

        future->run();
    }

    if(!IsCancelled())
    {
        isDone_ = true;
    }

    wakeupWaiters();
}

void FutureTasks::RunTasks(const InstanceHandleSet &handles)
{
    for(InstanceHandleSet::const_iterator it = handles.begin(), it_end = handles.end(); it != it_end; ++it)
    {
        if(IsCancelled()) break;

        Templates::RunnableFuture::Ptr future;

        {
            MutexLocker lock(&mutex_);
            future = getRunnableFuture(*it);
        }

        if(!future) continue;

        future->run();
    }
}

bool FutureTasks::Interrupt()
{
    // TODO: Implement function
    return false;
}

bool FutureTasks::IsInterrupted() const
{
    // TODO: Implement function
    return false;
}

bool FutureTasks::Cancel()
{
    if(IsDone()) return false;

    isCancelled_ = true;

    return true;
}

bool FutureTasks::IsCancelled() const
{
    return isCancelled_.get();
}

bool FutureTasks::IsDone() const
{
    return isDone_.get();
}

bool FutureTasks::WaitFor(int64 msecs) const
{
    MutexLocker lock(&mutex_);

    if(IsDone() || IsCancelled())
        return true;

    waitCondition_.wait(&mutex_, msecs);

    return IsDone() || IsCancelled();
}

bool FutureTasks::IsSuccess() const
{
    return isDone_.get();
}

const Status::ExecutionStatus &FutureTasks::StatusConst() const
{
    return status_;
}

Status::ExecutionStatus &FutureTasks::Status()
{
    return status_;
}

void FutureTasks::Clear()
{
    MutexLocker lock(&mutex_);

    isCancelled_= true;
    isDone_ = true;

    futureTasks_.clear();

    currentHandle_ = 0;
}

bool FutureTasks::Cancel(InstanceHandle handle)
{
    Templates::RunnableFuture::Ptr future = getRunnableFuture(handle);
    if(!future) return false;

    return future->Cancel();
}

bool FutureTasks::IsDone(InstanceHandle handle) const
{
    Templates::RunnableFuture::Ptr future = getRunnableFuture(handle);
    if(!future) return false;

    return future->IsDone();
}

InstanceHandleSet FutureTasks::GetHandles() const
{
    return futureTasks_.keySet();
}

InstanceHandleSet FutureTasks::GetNHandlesFIFO(unsigned int number) const
{
    InstanceHandleSet handleSet;
    for(ListFutureTasks::const_iterator it = futureTasks_.begin(), it_end = futureTasks_.end(); it != it_end && number > 0; ++it, --number)
    {
        handleSet.insert(it->first);
    }

    return handleSet;
}

InstanceHandleSet FutureTasks::Merge(FutureTasks::Ptr futureTasks)
{
    InstanceHandleSet newHandles;

    for(ListFutureTasks::iterator it = futureTasks->futureTasks_.begin(), it_end = futureTasks->futureTasks_.end(); it != it_end; ++it)
    {
        InstanceHandle handle = currentHandle_;

        newHandles.insert(handle);
        futureTasks_.insert(std::pair<InstanceHandle, Templates::RunnableFuture::Ptr>(handle, it->second));

        ++currentHandle_;
    }

    return newHandles;
}

InstanceHandleSet FutureTasks::Merge(FutureTasks::Ptr futureTasks, const InstanceHandleSet &handles)
{
    InstanceHandleSet newHandles;

    for(InstanceHandleSet::const_iterator it = handles.begin(), it_end = handles.end(); it != it_end; ++it)
    {
        Templates::RunnableFuture::Ptr future = futureTasks->getRunnableFuture(*it);

        if(future)
        {
            InstanceHandle handle = currentHandle_;

            newHandles.insert(handle);
            futureTasks_.insert(std::pair<InstanceHandle, Templates::RunnableFuture::Ptr>(handle, future));

            ++currentHandle_;
        }
    }

    return newHandles;
}

InstanceHandle FutureTasks::AddTask(Templates::RunnableFuture::Ptr futureTask)
{
    MutexLocker lock(&mutex_);
    return insertFutureTask(futureTask);
}

bool FutureTasks::RemoveTask(InstanceHandle handle)
{
    MutexLocker lock(&mutex_);
    return removeFutureTask(handle);
}

size_t FutureTasks::RemoveTasks(InstanceHandleSet handles)
{
    MutexLocker lock(&mutex_);
    return removeFutureTasks(handles);
}

bool FutureTasks::RemoveFinishedTasks()
{
    MutexLocker lock(&mutex_);
    return removeFinishedTasks();
}

Templates::RunnableFuture::Ptr FutureTasks::GetTaskBase(InstanceHandle handle) const
{
    return getRunnableFuture(handle);
}

InstanceHandle FutureTasks::insertFutureTask(Templates::RunnableFuture::Ptr futureTask)
{
    InstanceHandle handle = currentHandle_;

    futureTasks_.insert(std::pair<InstanceHandle, Templates::RunnableFuture::Ptr>(handle, futureTask));

    ++currentHandle_;

    return handle;
}

bool FutureTasks::removeFutureTask(InstanceHandle handle)
{
    ListFutureTasks::iterator pos = futureTasks_.find(handle);
    if(pos == futureTasks_.end())
        return false;

    futureTasks_.erase(pos);

    return true;
}

bool FutureTasks::removeFinishedTasks()
{
    InstanceHandleSet finishedTasks;

    for(ListFutureTasks::iterator it = futureTasks_.begin(), it_end = futureTasks_.end(); it != it_end; ++it)
    {
        Templates::RunnableFuture::Ptr future = it->second;
        if(!future)
            finishedTasks.insert(it->first);
        else if(future->IsDone() || future->IsCancelled())
            finishedTasks.insert(it->first);
    }

    this->removeFutureTasks(finishedTasks);

    return !finishedTasks.empty();
}

size_t FutureTasks::removeFutureTasks(const InstanceHandleSet &tasks)
{
    size_t numRemoved = 0;
    for(InstanceHandleSet::const_iterator it = tasks.begin(), it_end = tasks.end(); it != it_end; ++it)
    {
        bool isRemoved = this->removeFutureTask(*it);

        ASSERT(isRemoved);

        if(isRemoved) ++numRemoved;
    }
    return numRemoved;
}

Templates::RunnableFuture::Ptr FutureTasks::getRunnableFuture(InstanceHandle handle) const
{
    ListFutureTasks::const_iterator it = futureTasks_.find(handle);

    if(it == futureTasks_.end())
        return Templates::RunnableFuture::Ptr();

    return it->second;
}

void FutureTasks::wakeupWaiters() const
{
    MutexLocker lock(&mutex_);
    waitCondition_.wakeAll();
}

}}

