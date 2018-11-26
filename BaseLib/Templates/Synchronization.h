#ifndef BaseLib_Templates_Synchronization_h_Included
#define BaseLib_Templates_Synchronization_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/ReadLocker.h"
#include"BaseLib/WriteLocker.h"
#include"BaseLib/WaitCondition.h"
#include"BaseLib/ElapsedTimer.h"
#include"BaseLib/Debug.h"

#include"BaseLib/Templates/MethodInterfaces.h"

namespace BaseLib { namespace Templates
{

// -------------------------------------------------
// "Execute around objects" pattern
// -------------------------------------------------

template <typename T>
class WaitForNotificationTypeLocker
{
public:
    inline explicit WaitForNotificationTypeLocker(const T * const mutex, int64 msecs = LONG_MAX)
        : mutex_((T*)mutex)
        , isWoken_(false)
    {
        if(mutex_ == NULL)
            throw std::runtime_error("WaitForNotificationTypeLocker::WaitForNotificationTypeLocker(T*): Argument error! T* == NULL!");

        mutex_->lock();
        isWoken_ = mutex_->wait(msecs);
    }

    inline ~WaitForNotificationTypeLocker()
    {
        mutex_->unlock();
    }

    bool IsWoken() const
    {
        return isWoken_;
    }

private:
    mutable T *mutex_;
    bool isWoken_;

private:
    WaitForNotificationTypeLocker(const WaitForNotificationTypeLocker &) {}
    WaitForNotificationTypeLocker& operator=(const WaitForNotificationTypeLocker&) { return *this; }
};

template <typename T>
class WakeAllTypeLocker
{
public:
    inline explicit WakeAllTypeLocker(const T * const mutex)
        : mutex_((T*)mutex)
    {
        if(mutex_ == NULL)
            throw std::runtime_error("WakeAllTypeLocker::WakeAllTypeLocker(T*): Argument error! T* == NULL!");

        mutex_->lock();
        mutex_->wakeAll();
    }

    inline ~WakeAllTypeLocker()
    {
        mutex_->unlock();
    }

private:
    mutable T *mutex_;

private:
    WakeAllTypeLocker(const WakeAllTypeLocker &) {}
    WakeAllTypeLocker& operator=(const WakeAllTypeLocker&) { return *this; }
};

//class SignalWaiter
//{
//private:
//    MutexLocker locker;

//public:
//    SignalWaiter(Mutex *mutex)
//        : locker(mutex)
//    {}

//    void waitForSignal()
//    {
//        while (!signalled)
//            waitCondition.wait(locker.mutex());
//    }
//};

// -------------------------------------------------
// Base classes for synchronizations
// -------------------------------------------------

/**
 * @brief The Lockable class
 */
template <typename Lock = Mutex>
class Lockable
{
public:
    Lockable()
        : mutex_()
    {}
    Lockable(MutexPolicy policy)
        : mutex_(policy)
    {}
    virtual ~Lockable()
    {}

    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock(int64 msecs = 0) const { return mutex_.tryLock(msecs); }

protected:
    mutable Lock mutex_;
};

/**
 * @brief The LockableType class
 */
template <typename T, typename Lock = Mutex>
class LockableType
{
public:
    LockableType()
        : mutex_()
    {}
    LockableType(MutexPolicy policy)
        : mutex_(policy)
    {}
    virtual ~LockableType()
    {}

    void lock() const { mutex_.lock(); }
    void unlock()  const { mutex_.unlock(); }
    bool tryLock(int64 msecs = 0) const { return mutex_.tryLock(msecs); }

protected:
    typedef MutexTypeLocker<T>  Locker;

protected:
    mutable Lock mutex_;
};

/**
 * @brief The Notifyable class
 */
template <typename T, typename Cond = WaitCondition, typename Lock = Mutex>
class Notifyable : public LockableType<T, Lock>
{
public:
    Notifyable(MutexPolicy policy = MutexPolicy::Debug())
        : LockableType<T, Lock>(policy)
        , cond_()
    {}
    virtual ~Notifyable()
    {}

    bool wait(Lock *lock, int64 milliseconds = LONG_MAX) const
    {
        return cond_.wait(lock, milliseconds);
    }

    bool wait(int64 milliseconds = LONG_MAX) const
    {
        return cond_.wait(&this->mutex_, milliseconds);
    }

    template <typename Condition>
    bool waitFor(const Condition &condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition.IsTriggered() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition.IsTriggered();
    }

    template <typename U>
    bool waitForReceived(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsReceived() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsReceived();
    }

    template <typename U>
    bool waitForAccepted(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsAccepted() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsAccepted();
    }

    template <typename U>
    bool waitForDone(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsDone() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsDone();
    }

    bool waitForTrigger(const Templates::IsTriggeredMethod *const condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition->IsTriggered() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition->IsTriggered();
    }

    bool waitForSuccess(const Templates::IsSuccessMethod *const condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition->IsSuccess() && !timer.HasExpired())
        {
            bool isWoken = cond_.wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition->IsSuccess();
    }

    void wakeAll() const { cond_.wakeAll(); }
    void wakeOne() const { cond_.wakeOne(); }

    void lockedWakeAll() const
    {
        MutexTypeLocker<Lock> lock(&this->mutex_);
        cond_.wakeAll();
    }

    void lockedWakeOne() const
    {
        MutexTypeLocker<Lock> lock(&this->mutex_);
        cond_.wakeOne();
    }

protected:
    typedef WakeAllTypeLocker<T> WakeAllLocker;
    typedef WaitForNotificationTypeLocker<T> WaitForNotificationLocker;

private:
    mutable Cond cond_;
};

/**
 * @brief The Notifyable class
 */
template <typename T, typename Cond = WaitCondition, typename Lock = Mutex>
class NotifyableShared : public LockableType<T, Lock>
{
public:
    NotifyableShared()
        : LockableType<T, Lock>(MutexPolicy::DebugAndShare())
        , cond_(new Cond())
    {}
    virtual ~NotifyableShared()
    {}

    bool wait(Lock *lock, int64 milliseconds = LONG_MAX) const
    {
        return cond_->wait(lock, milliseconds);
    }

    bool wait(int64 milliseconds = LONG_MAX) const
    {
        return cond_->wait(&this->mutex_, milliseconds);
    }

    template <typename Condition>
    bool waitFor(const Condition &condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition.IsTriggered() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition.IsTriggered();
    }

    template <typename U>
    bool waitForReceived(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsReceived() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsReceived();
    }

    template <typename U>
    bool waitForAccepted(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsAccepted() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsAccepted();
    }

    template <typename U>
    bool waitForDone(const U *const t, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!t->IsDone() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return t->IsDone();
    }

    bool waitForTrigger(const Templates::IsTriggeredMethod *const condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition->IsTriggered() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition->IsTriggered();
    }

    bool waitForSuccess(const Templates::IsSuccessMethod *const condition, int64 msecs) const
    {
        ElapsedTimer timer(msecs);

        while(!condition->IsSuccess() && !timer.HasExpired())
        {
            bool isWoken = cond_->wait(&this->mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return condition->IsSuccess();
    }

    void wakeAll() const { cond_->wakeAll(); }
    void wakeOne() const { cond_->wakeOne(); }

    void lockedWakeAll() const
    {
        MutexTypeLocker<Lock> lock(&this->mutex_);
        cond_->wakeAll();
    }

    void lockedWakeOne() const
    {
        MutexTypeLocker<Lock> lock(&this->mutex_);
        cond_->wakeOne();
    }

protected:
    typedef WakeAllTypeLocker<T> WakeAllLocker;
    typedef WaitForNotificationTypeLocker<T> WaitForNotificationLocker;

private:
    mutable std::shared_ptr<Cond> cond_;
};

}}

#endif

