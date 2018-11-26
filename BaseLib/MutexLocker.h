/***************************************************************************
                          MutexLocker.h  -  description
                             -------------------
    begin                : Sat Mar 12 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#ifndef BaseLib_MutexLocker_h_IsIncluded
#define BaseLib_MutexLocker_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ElapsedTimer.h"
#include "BaseLib/Mutex.h"
#include "BaseLib/Export.h"

namespace BaseLib
{

class DLL_STATE MutexLocker
{
public:
    inline explicit MutexLocker(Mutex* mutex)
        : mutex_(mutex)
    {
        if(mutex_ == NULL)
            throw std::runtime_error("MutexLocker::MutexLocker(Mutex*): Argument error! Mutex* == NULL!");

        mutex_->lock();
    }

    inline ~MutexLocker()
    {
        mutex_->unlock();
    }

private:
    Mutex *mutex_;

private:
    MutexLocker(const MutexLocker &) {}
    MutexLocker& operator=(const MutexLocker&) { return *this; }
};

template <typename T>
class MutexTypeLocker
{
public:
    inline explicit MutexTypeLocker(const T * const mutex)
        : mutex_((T*)mutex)
    {
        if(mutex_ == NULL)
            throw std::runtime_error("MutexTypeLocker::MutexTypeLocker(Mutex*): Argument error! T* == NULL!");

        mutex_->lock();
    }

    inline ~MutexTypeLocker()
    {
        mutex_->unlock();
    }

private:
    mutable T *mutex_;

private:
    MutexTypeLocker(const MutexTypeLocker &) {}
    MutexTypeLocker& operator=(const MutexTypeLocker&) { return *this; }
};

/**
 * @brief The TryMutexTypeLocker class
 */
template <typename T>
class TryMutexTypeLocker
{
public:
    inline explicit TryMutexTypeLocker(T* mutex, int64 milliseconds = LONG_MAX)
        : mutex_(mutex)
        , isLocked_(false)
        , milliseconds_(milliseconds)
    {
        if(mutex_ == NULL)
            throw std::runtime_error("TryMutexTypeLocker::TryMutexTypeLocker(Mutex*, milliseconds): Argument error! T* == NULL!");

        isLocked_ = mutex_->tryLock(milliseconds);
    }

    inline ~TryMutexTypeLocker()
    {
        if(isLocked_)
        {
            mutex_->unlock();
            isLocked_ = false;
        }
    }

    void unlock()
    {
        if(isLocked_)
        {
            mutex_->unlock();
            isLocked_ = false;
        }
    }

    bool relock()
    {
        if(!isLocked_)
            isLocked_ = mutex_->tryLock(milliseconds_);

        return isLocked_;
    }

    bool isLocked() const { return isLocked_; }

private:
    T       *mutex_;
    bool    isLocked_;

    int64 milliseconds_;

private:
    TryMutexTypeLocker(const TryMutexTypeLocker &) {}
    TryMutexTypeLocker& operator=(const TryMutexTypeLocker&) { return *this; }
};

}

#endif

