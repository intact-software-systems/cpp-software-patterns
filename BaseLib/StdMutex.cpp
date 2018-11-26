#include "StdMutex.h"

namespace BaseLib {

StdMutex::StdMutex(MutexPolicy )
    : mutex_(std::make_shared<std::timed_mutex>())
{ }

StdMutex::StdMutex(const StdMutex& other)
    : mutex_(other.mutex_)
{ }

StdMutex::~StdMutex()
{ }

StdMutex& StdMutex::operator=(const StdMutex& )
{
    return *this;
}

void StdMutex::lock()
{
    mutex_->lock();
}

void StdMutex::unlock()
{
    mutex_->unlock();
}

bool StdMutex::tryLock(int64 msecs)
{
    if(msecs <= 0)
    {
        return mutex_->try_lock();
    }
    else
    {
        std::chrono::duration<int64, std::milli> duration(msecs);
        return mutex_->try_lock_for(duration);
    }
}

bool StdMutex::tryLock(Duration duration)
{
    return mutex_->try_lock_for(duration);
}

bool StdMutex::tryLockUntil(Timestamp timestamp)
{
    return mutex_->try_lock_until(timestamp);
}

}