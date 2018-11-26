/***************************************************************************
                          WaitCondition.cpp  -  description
                             -------------------
    begin                : Sat Mar 12 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#include <stdio.h>
#include <exception>
#include <iostream>

#include "BaseLib/WaitCondition.h"
#include "BaseLib/Thread.h"
#include "BaseLib/Debug.h"
#include "BaseLib/Utility.h"
#include "BaseLib/ReadWriteLock.h"
#include "BaseLib/Timestamp.h"
#include "Exception.h"

namespace BaseLib
{

static void report_error(int code, const char *where, const char *what)
{
    if (code != 0)
        iWarning("%s: %s failure: %i", where, what, code);
}

static void deleteSelf(WaitConditionPrivate *self);

#ifdef USE_GCC
class WaitConditionPrivate
{
public:
    WaitConditionPrivate()
        : waiters(0)
        , wakeups(0)
    {}
    virtual ~WaitConditionPrivate()
    {
        deleteSelf(this);
    }

    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int waiters;
    int wakeups;

    bool wait(int64 msecs)
    {
        ElapsedTimer timer(msecs);

        int code = -1;
        bool waited = false;

        while((wakeups <= 0 && code != 0) && !timer.HasExpired() && !Thread::isInterrupted())
        {
//            if(msecs != LONG_MAX)
//            {
                struct timespec ts = {0,0};
                int init = Utility::ClockInitEpochWaitTimeMsec(ts, std::min<int64>(5000, timer.Remaining().InMillis()));

                ASSERT(init == 0);

                /**
                    Except in the case of [ETIMEDOUT], all these error checks shall act as if they were performed
                    immediately at the beginning of processing for the function and shall cause an error return,
                    in effect, prior to modifying the state of the mutex specified by mutex or the condition variable
                    specified by cond.

                    Upon successful completion, a value of zero shall be returned; otherwise, an error number shall
                    be returned to indicate the error.
                 */

                code = pthread_cond_timedwait(&cond, &mutex, &ts);

                // -- debug --
                //if(code == ETIMEDOUT)
                //    IDEBUG() << "The time specified by abstime to pthread_cond_timedwait() has passed";
                if(code == EINVAL) {
                    ICRITICAL() << "Wait time: " << msecs << ". Remining " << timer.Remaining() << ". Error: The value specified by abstime is invalid: ts(" << ts.tv_sec << "," << ts.tv_nsec << ")";
                    ASSERT(code != EINVAL);
                }
                else if(code == EPERM) {
                    iCritical("The mutex was not owned by the current thread at the time of the call");
                    ASSERT(code != EPERM);
                }
                else if(code == EDEADLK) {
                    iCritical("Thread::wait(): EDEADLK A deadlock was detected or the value  of  thread  specifies  the calling thread.");
                    ASSERT(code != EDEADLK);
                }
                // -- debug --
//            }
//            else
//            {
//                code = pthread_cond_wait(&cond, &mutex);
//            }

            waited = true;
        }

        ASSERT(waiters > 0);
        --waiters;

        if(code == 0 && !Thread::isInterrupted())
        {
            ASSERT(wakeups > 0);
            wakeups = std::max<int>(0, --wakeups);
        }

        report_error(pthread_mutex_unlock(&mutex), "WaitCondition::wait()", "mutex unlock");

        if (code && code != ETIMEDOUT && waited)
            report_error(code, "WaitCondition::wait()", "cv wait");

        return (code == 0);
    }
};
#elif WIN32
class WaitConditionPrivate
{
public:
    WaitConditionPrivate()
        : waiters(0)
        , wakeups(0)
    {}
    virtual ~WaitConditionPrivate()
    {
        deleteSelf(this);
    }

    CRITICAL_SECTION 	criticalSection;
    CONDITION_VARIABLE 	conditionVariable;

    int waiters;
    int wakeups;

    bool wait(int64 msecs)
    {
        ElapsedTimer timer(msecs);

        BOOL code = 0;
        while( (wakeups <= 0 && code == 0) && !timer.HasExpired() && !Thread::isInterrupted())
        {
            /*--------------------------------------------------------------
                BOOL WINAPI SleepConditionVariableCS(
                  __inout  PCONDITION_VARIABLE ConditionVariable,
                  __inout  PCRITICAL_SECTION CriticalSection,
                  __in     DWORD dwMilliseconds
                );
            If the function succeeds, the return value is nonzero.

            If the function fails or the time-out interval elapses, the return value is zero.
            To get extended error information, call GetLastError. Possible error codes include
            ERROR_TIMEOUT, which indicates that the time-out interval has elapsed before another
            thread has attempted to wake the sleeping thread.
            --------------------------------------------------------------*/

            //if(msecs == LONG_MAX)
            //{
            //    code = SleepConditionVariableCS(&conditionVariable, &criticalSection, INFINITE);
            //}
            //else
            //{
                code = SleepConditionVariableCS(&conditionVariable, &criticalSection, std::min<int64>(5000, timer.Remaining().InMillis()));
            //}

            //IDEBUG() << "waiters = " << waiters << ", wakeups = " << wakeups << ", code = " << code;
        }

        ASSERT(waiters > 0);

        --waiters;

        ASSERT(waiters >= 0);

        if(code != 0)
        {
            ASSERT(wakeups > 0);
            wakeups = std::max<int>(0, --wakeups);
        }

        LeaveCriticalSection(&criticalSection);

        return (code == 1);
    }
};
#endif

static void deleteSelf(WaitConditionPrivate *self)
{
#ifdef USE_GCC
    report_error(pthread_cond_destroy(&self->cond), "WaitCondition", "cv destroy");
    report_error(pthread_mutex_destroy(&self->mutex), "WaitCondition", "mutex destroy");
#elif WIN32
    //DeleteConditionVariable(&self->conditionVariable);
    DeleteCriticalSection(&self->criticalSection);
#endif
}

WaitCondition::WaitCondition()
    : self(new WaitConditionPrivate())
{
#ifdef USE_GCC
    report_error(pthread_mutex_init(&self->mutex, NULL), "WaitCondition", "mutex init");
    report_error(pthread_cond_init(&self->cond, NULL), "WaitCondition", "cv init");
    self->waiters = self->wakeups = 0;
#elif WIN32
    InitializeConditionVariable(&self->conditionVariable);
    InitializeCriticalSection(&self->criticalSection);
    self->waiters = self->wakeups = 0;
#endif

}
WaitCondition::~WaitCondition()
{ }

void WaitCondition::wakeOne()
{
#ifdef USE_GCC
    report_error(pthread_mutex_lock(&self->mutex), "WaitCondition::wakeOne()", "mutex lock");
    self->wakeups = std::min(self->wakeups + 1, self->waiters);
    report_error(pthread_cond_signal(&self->cond), "WaitCondition::wakeOne()", "cv signal");
    report_error(pthread_mutex_unlock(&self->mutex), "WaitCondition::wakeOne()", "mutex unlock");
#elif WIN32
    EnterCriticalSection(&self->criticalSection);
    self->wakeups = std::min(self->wakeups + 1, self->waiters);
    WakeConditionVariable(&self->conditionVariable);
    LeaveCriticalSection(&self->criticalSection);
#endif
}

void WaitCondition::wakeAll()
{
#ifdef USE_GCC
    report_error(pthread_mutex_lock(&self->mutex), "WaitCondition::wakeAll()", "mutex lock");
    self->wakeups = self->waiters;
    report_error(pthread_cond_broadcast(&self->cond), "WaitCondition::wakeAll()", "cv broadcast");
    report_error(pthread_mutex_unlock(&self->mutex), "WaitCondition::wakeAll()", "mutex unlock");
#elif WIN32
    EnterCriticalSection(&self->criticalSection);

    self->wakeups = self->waiters;
    WakeAllConditionVariable(&self->conditionVariable);

    LeaveCriticalSection(&self->criticalSection);
#endif
}

bool WaitCondition::wait(Mutex *mutex, int64 msecs)
{
    if(!mutex) return false;

#ifdef USE_GCC
    report_error(pthread_mutex_lock(&self->mutex), "WaitCondition::wait()", "mutex lock");
#elif WIN32
    EnterCriticalSection(&self->criticalSection);
#endif
    ++self->waiters;

    mutex->unlock();

    if(mutex->isDebug())
    {
        Thread* thread_self = Thread::currentThread();
        if(thread_self)
            thread_self->setThreadState(ThreadDetails::MonitoringState);
    }

    bool isWoken = self->wait(msecs);

    mutex->lock();

    return isWoken;
}

bool WaitCondition::wait(ReadWriteLock *readWriteLock, int64 msecs)
{
    if(!readWriteLock || readWriteLock->accessCount_ == 0) return false;

#ifdef USE_GCC
    report_error(pthread_mutex_lock(&self->mutex), "WaitCondition::wait()", "mutex lock");
#elif WIN32
    EnterCriticalSection(&self->criticalSection);
#endif

    ++self->waiters;

    int previousAccessCount = readWriteLock->accessCount_;
    readWriteLock->unlock();

    bool isWoken = self->wait(msecs);

    if (previousAccessCount < 0)
        readWriteLock->lockForWrite();
    else
        readWriteLock->lockForRead();

    return isWoken;
}

int WaitCondition::numWaiters() const
{
    int numWaiters = 0;

#ifdef USE_GCC
    report_error(pthread_mutex_lock(&self->mutex), "WaitCondition::numWaiters() const", "mutex lock");
    numWaiters = self->waiters;
    report_error(pthread_mutex_unlock(&self->mutex), "WaitCondition::numWaiters() const", "mutex unlock");
#elif WIN32
    EnterCriticalSection(&self->criticalSection);
    numWaiters = self->waiters;
    LeaveCriticalSection(&self->criticalSection);
#endif

    return numWaiters;
}

}
