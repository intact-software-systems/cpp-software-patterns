#include "BaseLib/InterruptibleWaitCondition.h"
#include "BaseLib/Thread.h"
#include "BaseLib/ThreadManager.h"
#include "BaseLib/Exception.h"

namespace BaseLib {

InterruptibleWaitCondition::InterruptibleWaitCondition()
    : mutex_(MutexPolicy::No())
{ }

InterruptibleWaitCondition::~InterruptibleWaitCondition()
{ }

bool InterruptibleWaitCondition::wait(Mutex* mutex, int64 milliseconds)
{
    Thread* thread = Thread::currentThread();
    if(thread == nullptr)
    {
        throw CriticalException("Current thread not found!");
    }

    insertWaiting();

    try
    {
        thread->interruptible().wait(mutex, milliseconds);

        removeWaiting();
    }
    catch(InterruptedException ex)
    {
        removeWaiting();
        throw ex;
    }
    catch(...)
    {
        removeWaiting();
    }

    return false;
}

bool InterruptibleWaitCondition::wait(ReadWriteLock* mutex, int64 milliseconds)
{
    Thread* thread = Thread::currentThread();
    if(thread == nullptr)
    {
        throw CriticalException("Current thread not found!");
    }
    insertWaiting();

    try
    {
        thread->interruptibleReadWrite().wait(mutex, milliseconds);

        removeWaiting();
    }
    catch(InterruptedException ex)
    {
        removeWaiting();
        throw ex;
    }
    catch(...)
    {
        removeWaiting();
    }

    return false;

}


void InterruptibleWaitCondition::wakeAll()
{
    MutexLocker lock(&mutex_);

    for(auto threadId : waiting_)
    {
        Thread* thread = ThreadManager::GetInstance().GetThread(threadId);
        if(thread != nullptr)
        {
            thread->interruptible().wakeAll();
        }
    }

    waiting_.clear();
}

void InterruptibleWaitCondition::wakeOne()
{
    MutexLocker lock(&mutex_);

    if(!waiting_.empty())
    {
        const std::unordered_set<int64>::const_iterator& it = waiting_.cbegin();

        Thread* thread = ThreadManager::GetInstance().GetThread(it.operator*());
        if(thread != nullptr)
        {
            thread->interruptible().wakeAll();
        }

        waiting_.erase(it);
    }
}

size_t InterruptibleWaitCondition::numWaiters() const
{
    MutexLocker lock(&mutex_);

    return waiting_.size();
}

void InterruptibleWaitCondition::removeWaiting()
{
    MutexLocker lock(&mutex_);
    this->waiting_.erase(Thread::currentThreadId());
}

void InterruptibleWaitCondition::insertWaiting()
{
    MutexLocker lock(&mutex_);
    this->waiting_.insert(Thread::currentThreadId());
}

}
