/***************************************************************************
                          Thread.h  -  description
                             -------------------
    begin                : Sun May 9 2010
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#include <exception>
#include <iostream>
#include <string>
#include <stdexcept>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <memory>

#ifdef WIN32
#include <process.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <conio.h>
#else
#include <ctime>
#include <sys/time.h>
#include <unistd.h>
#endif

#include"BaseLib/Exception.h"
#include"BaseLib/CommonDefines.h"
#include"BaseLib/Thread.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/ThreadManager.h"
#include"BaseLib/Debug.h"
#include"BaseLib/Utility.h"

#define BILLION     1000000000
#define MILLION     1000000
#define THOUSAND    1000

using namespace std;

namespace BaseLib
{

#ifdef USE_GCC
// Does some magic and calculate the Unix scheduler priorities
// sched_policy is IN/OUT: it must be set to a valid policy before calling this function
// sched_priority is OUT only
static bool calculateUnixPriority(int priority, int *sched_policy, int *sched_priority)
{
#ifdef Q_OS_QNX
    // without Round Robin drawn intensive apps will hog the cpu
    // and make the system appear frozen
   *sched_policy = SCHED_RR;
#endif

#ifdef SCHED_IDLE
    if (priority == ThreadDetails::IdlePriority) {
        *sched_policy = SCHED_IDLE;
        *sched_priority = 0;
        return true;
    }
    const int lowestPriority = ThreadDetails::LowestPriority;
#else
    const int lowestPriority = ThreadDetails::IdlePriority;
#endif
    const int highestPriority = ThreadDetails::TimeCriticalPriority;

    int prio_min = sched_get_priority_min(*sched_policy);
    int prio_max = sched_get_priority_max(*sched_policy);
    if (prio_min == -1 || prio_max == -1)
        return false;

    int prio;
    // crudely scale our priority enum values to the prio_min/prio_max
    prio = ((priority - lowestPriority) * (prio_max - prio_min) / highestPriority) + prio_min;
    prio = std::max(prio_min, std::min(prio_max, prio));

    *sched_priority = prio;
    return true;
}
#else

ThreadDetails::Priority GetPriority(int prio)
{
    if(prio == 	THREAD_PRIORITY_IDLE)
    {
        return ThreadDetails::IdlePriority;
    }
    else if(prio == THREAD_PRIORITY_LOWEST)
    {
        return ThreadDetails::LowestPriority;
    }
    else if(prio == THREAD_PRIORITY_BELOW_NORMAL)
    {
        return ThreadDetails::LowPriority;
    }
    else if(prio == THREAD_PRIORITY_NORMAL)
    {
        return ThreadDetails::NormalPriority;
    }
    else if(prio == THREAD_PRIORITY_ABOVE_NORMAL)
    {
        return ThreadDetails::HighPriority;
    }
    else if(prio == THREAD_PRIORITY_HIGHEST)
    {
        return ThreadDetails::HighestPriority;
    }
    else if(prio == THREAD_PRIORITY_TIME_CRITICAL)
    {
        return ThreadDetails::TimeCriticalPriority;
    }
    else
    {
        IWARNING() << " Could not map thread priority Win32 scheme! Defaulting to normal priority!";
    }

    return ThreadDetails::NormalPriority;
}
#endif

/*-----------------------------------------------------------------------
                        operators
------------------------------------------------------------------------- */
std::ostream& operator<<(std::ostream& ostr, const ThreadDetails::State &status)
{
    switch(status)
    {
#define CASE(t) case t : ostr << #t ; break;
    CASE(ThreadDetails::NoState)
    CASE(ThreadDetails::RunningState)
    CASE(ThreadDetails::RunningMutexedState)
    CASE(ThreadDetails::SleepingState)
    CASE(ThreadDetails::WaitingState)
    CASE(ThreadDetails::LockingState)
    CASE(ThreadDetails::UnlockingState)
    CASE(ThreadDetails::MonitoringState)
    CASE(ThreadDetails::StopState)
    CASE(ThreadDetails::CancelState)
    CASE(ThreadDetails::SystemEndState)
    CASE(ThreadDetails::PrintState)
    CASE(ThreadDetails::CheckState)
#undef CASE
    default:
        IWARNING()  << "Error! undefined ThreadDetails::State";
        break;
    }
    return ostr;
}


std::ostream& operator<<(std::ostream& ostr, const ThreadDetails &details)
{
    ostr << "id(" << details.threadId();
    ostr << "," << details.humanReadableId();
    ostr << "," << details.threadName() << ")";
    ostr << "state(" << details.threadState();
    ostr << "," << details.threadRunning();
    ostr << "," << details.threadPriority() << ")";
    return ostr;
}

/*-----------------------------------------------------------------------
                    class Thread
------------------------------------------------------------------------- */
Thread::Thread(std::string name)
    : threadDetails_(name)
    , threadMutex_(MutexPolicy::No())
    , debug_(true)
{

}

Thread::Thread(std::string name, bool debug)
    : threadDetails_(name)
    , threadMutex_(MutexPolicy::No())
    , debug_(debug)
{

}

Thread::~Thread()
{
    this->terminate();

    cleanUp();
}

void Thread::cleanUp()
{
    if(threadDetails_.threadId() > 0)
    {
#ifdef USE_GCC
        int status = pthread_attr_destroy(&(threadDetails_.threadAttributes()));
        if(status != 0) throw std::runtime_error("Thread::~Thread(): pthread_attr_destroy failed!");
#elif WIN32
        CloseHandle(threadDetails_.threadAttributes());
#endif
        threadDetails_.threadId(0);
    }
}

#ifdef USE_GCC
void* Thread::dispatch(void *thread_obj)
#elif WIN32
unsigned int __stdcall Thread::dispatch(void *thread_obj)
#endif
{
    Thread *thread = (Thread*)thread_obj;
    ASSERT(thread);

    try
    {
        thread->setPriority(thread->priority());
        thread->getThreadDetails().threadState(ThreadDetails::RunningState);

        while(!thread->isRunning())
            Thread::nsleep(10);

        // -- for monitoring purposes --
        ThreadManager::GetInstance().AddThread(thread, thread->threadId());

        thread->run();
    }
    catch(ErrorException ex)
    {
        IWARNING() << "Error exception caught: " << ex.what();
    }
    catch(WarningException ex)
    {
        IWARNING() << "Warning exception caught: " << ex.what();
    }
    catch(CriticalException ex)
    {
        ICRITICAL() << "Critical exception caught: " << ex.what();
        // TODO: Should I pause process?
    }
    catch(FatalException ex)
    {
        IFATAL() << "Fatal exception caught: " << ex.what();
        // TODO: abort?
        abort();
    }
    catch(DeadlockException ex)
    {
        IFATAL() << "DeadlockException exception caught: " << ex.what();
    }
    catch(Exception ex)
    {
        // TODO: Add: Exception::State() == SEVERITY_SYSTEM_END
        //								== STOPALL
        // then notify ThreadManager to stop all threads!
        // ThreadManager::GetInstance().getThreadDetails().threadState(ThreadDetails::StopState);
        IDEBUG() << "Thread::dispatch(Thread): Exception caught! " << ex.what();
    }
    catch(GeneralException ex)
    {
        // TODO: Add: Exception::State() == SEVERITY_SYSTEM_END
        //								== STOPALL
        // then notify ThreadManager to stop all threads!
        // ThreadManager::GetInstance().getThreadDetails().threadState(ThreadDetails::StopState);
        IDEBUG() << "Thread::dispatch(Thread): General exception caught! " << ex.what();
    }
    catch(std::runtime_error error)
    {
        ICRITICAL() << "Critical exception caught: " << error.what();
        abort();
    }
    catch(std::exception x)
    {
        ICRITICAL() << "Exception caught! " << x.what();
    }
    catch(...)
    {
        ICRITICAL() << "Unhandled exception caught!";
        // TODO: add general system monitor catch here!
    }

    thread->setThreadState(ThreadDetails::StopState);
    thread->getThreadDetails().threadRunning(false);

    ThreadManager::GetInstance().RemoveThread(thread);

#ifdef USE_GCC
    return nullptr;
#elif WIN32
    return 0;
#endif
}

bool Thread::start(ThreadDetails::Priority priority)
{
    MutexTypeLocker<MutexBare> mutexLocker(&threadMutex_);

    if(isRunning())
    {
        Thread* self = Thread::currentThread();
        if(self)
        {
            ICRITICAL() << "Attempting to start thread that is already started: (" << self->threadName() << "," << self->threadId() << ")";
        }
        else
        {
            ICRITICAL() << "Attempting to start thread that is already started!";
        }
        return false;
    }

    try
    {
        ASSERT(!isRunning());

        // -----------------------------------------
        // If executed multiple times, clean up first
        // TODO: Check if it is necessary, pthread_attr_t is on stack?
        // -----------------------------------------
        //cleanUp();

#ifdef USE_GCC
        pthread_t m_pid = 0;
        int err = 0;

        // For portability, explicitly create threads in a joinable state
        pthread_attr_init(&(threadDetails_.threadAttributes()));
        pthread_attr_setdetachstate(&(threadDetails_.threadAttributes()), PTHREAD_CREATE_JOINABLE);

        // Set priority
        {
            switch (priority) {
            case ThreadDetails::InheritPriority:
                {
                    pthread_attr_setinheritsched( &(threadDetails_.threadAttributes()), PTHREAD_INHERIT_SCHED);
                    break;
                }
            default:
                {
                    int sched_policy = 0;
                    if (pthread_attr_getschedpolicy( &(threadDetails_.threadAttributes()), &sched_policy) != 0)
                    {
                        // failed to get the scheduling policy, don't bother
                        // setting the priority
                        IWARNING()<< "Cannot determine default scheduler policy";
                        break;
                    }

                    int prio = 0;
                    if (!calculateUnixPriority(priority, &sched_policy, &prio))
                    {
                        // failed to get the scheduling parameters, don't
                        // bother setting the priority
                        IWARNING() << "Cannot determine scheduler priority range";
                        break;
                    }

                    sched_param sp;
                    sp.sched_priority = prio;

                    if (pthread_attr_setinheritsched(&(threadDetails_.threadAttributes()), PTHREAD_EXPLICIT_SCHED) != 0
                        || pthread_attr_setschedpolicy(&(threadDetails_.threadAttributes()), sched_policy) != 0
                        || pthread_attr_setschedparam(&(threadDetails_.threadAttributes()), &sp) != 0) {
                        // could not set scheduling hints, fallback to inheriting them
                        // we'll try again from inside the thread
                        pthread_attr_setinheritsched(&(threadDetails_.threadAttributes()), PTHREAD_INHERIT_SCHED);
                        priority = ThreadDetails::InheritPriority; //Priority(priority | ThreadPriorityResetFlag);
                    }
                    break;
                }
            }
        }

        err = pthread_create(&m_pid, &(threadDetails_.threadAttributes()), Thread::dispatch, (void *)this);
        if(err != 0)
            throw("Thread::Start() pthread create failed "); // + string(strerror(err)));

        //IDEBUG() << " Thread up: " << m_pid ;

        threadDetails_.threadId((int64)m_pid);
        threadDetails_.threadRunning(true);
        threadDetails_.threadPriority(priority);

        // makes thread not joinable!
        //pthread_detach(m_pid);

#elif WIN32
        unsigned int m_pid;
        HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, Thread::dispatch, this, CREATE_SUSPENDED, &m_pid);
        if(threadHandle == NULL)
            throw CriticalException("Thread failed to start!");

        threadDetails_.threadId((int64)m_pid);
        threadDetails_.threadRunning(true);
        threadDetails_.threadPriority(priority);
        threadDetails_.threadAttributes(threadHandle);

        // initialize priorities!
        int prio = THREAD_PRIORITY_NORMAL;
        switch (priority)
        {
            case ThreadDetails::IdlePriority:
                prio = THREAD_PRIORITY_IDLE;
                break;

            case ThreadDetails::LowestPriority:
                prio = THREAD_PRIORITY_LOWEST;
                break;

            case ThreadDetails::LowPriority:
                prio = THREAD_PRIORITY_BELOW_NORMAL;
                break;

            case ThreadDetails::NormalPriority:
                prio = THREAD_PRIORITY_NORMAL;
                break;

            case ThreadDetails::HighPriority:
                prio = THREAD_PRIORITY_ABOVE_NORMAL;
                break;

            case ThreadDetails::HighestPriority:
                prio = THREAD_PRIORITY_HIGHEST;
                break;

            case ThreadDetails::TimeCriticalPriority:
                prio = THREAD_PRIORITY_TIME_CRITICAL;
                break;

            case ThreadDetails::InheritPriority:
            default:
                prio = GetThreadPriority(GetCurrentThread());
                break;
        }

        if (!SetThreadPriority(threadHandle, prio))
            IWARNING() << "Failed to set thread priority";

        if (ResumeThread(threadHandle) == (DWORD) -1)
            IWARNING() << "Failed to resume new thread";

        threadDetails_.threadPriority(GetPriority(prio));
#endif
    }
    catch(Exception ex)
    {
        ICRITICAL() << "Thread::start(priority): Failed! : " << ex.msg();
        return false;
    }
    catch(...)
    {
        ICRITICAL() << "Thread::start(priority): Failed! ";
        return false;
    }

    return true;
}

void Thread::exit()
{
    MutexTypeLocker<MutexBare> mutexLocker(&threadMutex_);
    // TODO: Notify ThreadManager to stop thread instead?
#ifdef USE_GCC
    pthread_exit(NULL);
#elif WIN32
    //_endthreadex(0);
    TerminateThread(threadDetails_.threadAttributes(), 0);
#else
    ASSERT(0);
#endif
}

void Thread::terminate()
{
    MutexTypeLocker<MutexBare> mutexLocker(&threadMutex_);
    if(!isRunning()) return;

#ifdef USE_GCC
    try
    {
        int err = pthread_cancel((pthread_t)threadDetails_.threadId());
        if(err != 0) cerr << "Thread::terminate() error: " << endl; //strerror(err) << endl;
    }
    catch(...)
    {
        cout << "Thread::terminate(): Exception!" << endl;
    }
#elif WIN32
    //_endthreadex(0);
    TerminateThread(threadDetails_.threadAttributes(), 0);
#endif

    this->threadDetails_.threadState(ThreadDetails::CancelState);
    this->threadDetails_.threadRunning(false);
}

bool Thread::wait(int64 msecs)
{
    {
        MutexTypeLocker<MutexBare> lock(&threadMutex_);
        if(!isRunning())
            return true;
    }

    if(msecs == LONG_MAX)
    {
#ifdef USE_GCC
        int status = pthread_join((pthread_t)threadDetails_.threadId(), NULL);
        if(status == EINVAL)
            throw std::runtime_error("Thread::wait(): EINVAL The implementation has detected that the  value specified by thread does not refer to a joinable thread");
        else if(status == ESRCH)
            throw std::runtime_error("Thread::wait(): ESRCH No thread could be found corresponding to that specified by the given thread ID.");
        else if(status == EDEADLK)
            throw std::runtime_error("Thread::wait(): EDEADLK A deadlock was detected or the value  of  thread  specifies  the calling thread.");
        else if(status != 0)
            throw std::runtime_error("Thread::wait(): pthread_join failed and unidentified error occurred!");
#elif WIN32
        WaitForSingleObject(threadDetails_.threadAttributes(), INFINITE);
#endif
        return true;
    }
    else
    {
#ifdef USE_GCC
        struct timespec ts = {0,0};
        int ret = Utility::ClockInitEpochWaitTimeMsec(ts, msecs);
        if(ret != 0) throw std::runtime_error("Thread::wait(time): clock_gettime(CLOCK_REALTIME, ts) failed");

        int status = Utility::pthread_timedjoin((pthread_t)threadDetails_.threadId(), NULL, &ts);
        if(status == ETIMEDOUT)
            return false;
        else if(status == EINVAL)
            throw std::runtime_error("Thread::wait(): EINVAL The implementation has detected that the  value specified by thread does not refer to a joinable thread");
        else if(status == ESRCH)
            throw std::runtime_error("Thread::wait(): ESRCH No thread could be found corresponding to that specified by the given thread ID.");
        else if(status == EDEADLK)
            throw std::runtime_error("Thread::wait(): EDEADLK A deadlock was detected or the value  of  thread  specifies  the calling thread.");
        else if(status != 0)
            throw std::runtime_error("Thread::wait(time): pthread_timedjoin_np failed");
#elif WIN32
        DWORD ret = WaitForSingleObject(threadDetails_.threadAttributes(), (DWORD) msecs);
        if(ret == 0) return false;
#endif
        return true;
    }
}

bool Thread::isFinished() const
{
    return !threadDetails_.threadRunning();
}

bool Thread::isRunning() const
{
    return threadDetails_.threadRunning();
}

ThreadDetails::Priority Thread::priority() const
{
    return threadDetails_.threadPriority();
}

const std::string &Thread::threadName() const
{
    return threadDetails_.threadName();
}

int64 Thread::threadId() const
{
    return threadDetails_.threadId();
}

int64 Thread::humanReadableId() const
{
    return threadDetails_.humanReadableId();
}

void Thread::humanReadableId(int64 id)
{
    threadDetails_.humanReadableId(id);
}

void Thread::setPriority(ThreadDetails::Priority priority)
{
    MutexTypeLocker<MutexBare> mutexLocker(&threadMutex_);

    threadDetails_.threadPriority(priority);

#if 0
    if(isRunning() == false) return;

    struct sched_param param;
    // sched_priority will be the priority of the thread
    param.sched_priority = priority;
    // scheduling parameters of target thread
    pthread_t m_pid = (pthread_t)threadDetails_.threadId();
    pthread_setschedparam(m_pid, SCHED_OTHER, &param);
#endif

#ifdef USE_GCC
    int sched_policy;
    sched_param param;

    if (pthread_getschedparam((pthread_t)threadDetails_.threadId(), &sched_policy, &param) != 0) {
        // failed to get the scheduling policy, don't bother setting
        // the priority
        IWARNING() << "Cannot get scheduler parameters";
        return;
    }

    int prio;
    if (!calculateUnixPriority(priority, &sched_policy, &prio)) {
        // failed to get the scheduling parameters, don't
        // bother setting the priority
        IWARNING() << "Cannot determine scheduler priority range";
        return;
    }

    param.sched_priority = prio;
    int status = pthread_setschedparam((pthread_t)threadDetails_.threadId(), sched_policy, &param);
    if(status < 0)
    {
        IWARNING() << "Failed to setschedparam!";
    }


/*# ifdef SCHED_IDLE
    // were we trying to set to idle priority and failed?
    if (status == -1 && sched_policy == SCHED_IDLE && errno == EINVAL) {
        // reset to lowest priority possible
        pthread_getschedparam(d->thread_id, &sched_policy, &param);
        param.sched_priority = sched_get_priority_min(sched_policy);
        pthread_setschedparam(d->thread_id, sched_policy, &param);
    }
# else
    Q_UNUSED(status);
# endif // SCHED_IDLE
*/

#elif WIN32
    int prio = THREAD_PRIORITY_NORMAL;
    switch(priority)
    {
        case ThreadDetails::IdlePriority:
            prio = THREAD_PRIORITY_IDLE;
            break;

        case ThreadDetails::LowestPriority:
            prio = THREAD_PRIORITY_LOWEST;
            break;

        case ThreadDetails::LowPriority:
            prio = THREAD_PRIORITY_BELOW_NORMAL;
            break;

        case ThreadDetails::NormalPriority:
            prio = THREAD_PRIORITY_NORMAL;
            break;

        case ThreadDetails::HighPriority:
            prio = THREAD_PRIORITY_ABOVE_NORMAL;
            break;

        case ThreadDetails::HighestPriority:
            prio = THREAD_PRIORITY_HIGHEST;
            break;

        case ThreadDetails::TimeCriticalPriority:
            prio = THREAD_PRIORITY_TIME_CRITICAL;
            break;

        case ThreadDetails::InheritPriority:
        default:
            IWARNING() << "Argument cannot be InheritPriority!";
            break;
    }

    if (!SetThreadPriority(threadDetails_.threadAttributes(), prio))
        IWARNING() << "Failed to set thread priority";
#endif
}

void Thread::setThreadName(const std::string &name)
{
    threadDetails_.threadName(name);
}

Thread* Thread::currentThread()
{
    int64 threadId = 0;
#ifdef USE_GCC
    threadId = (int64) pthread_self();
#else
    threadId = ::GetCurrentThreadId();
#endif

    return ThreadManager::GetInstance().GetThread(threadId);
}

int64 Thread::currentThreadId()
{
#ifdef USE_GCC
    return (int64)pthread_self();
#else
    return (int64)::GetCurrentThreadId();
#endif
}

int Thread::idealThreadCount()
{
#ifdef MINGW
    return 8;
#elif USE_GCC
    return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
#endif
}

void Thread::yieldCurrentThread()
{
#ifdef WIN32
    #ifndef Q_OS_WINCE
        SwitchToThread();
    #else
        ::Sleep(0);
    #endif
#elif USE_GCC
   sched_yield();
#endif
}

int Thread::processId()
{
#ifdef WIN32
//    return _getpid();
    return (int)GetCurrentProcessId();
#else
    return getpid();
#endif
}

bool Thread::checkThreadState()
{
    Thread *thread = Thread::currentThread();
    if(thread == NULL)
    {
        throw BaseLib::Exception("Thread::checkThreadState(): Thread::currentThread() returned NULL!");
    }

    switch(thread->getThreadDetails().threadState())
    {
        case ThreadDetails::NoState:
        {
            IDEBUG() << "WARNING! Thread " << Thread::currentThreadId() << " has state " << thread->getThreadDetails().threadState();
        }
        case ThreadDetails::RunningState:
        case ThreadDetails::RunningMutexedState:
        case ThreadDetails::SleepingState:
        case ThreadDetails::WaitingState:
        case ThreadDetails::LockingState:
        case ThreadDetails::UnlockingState:
        case ThreadDetails::MonitoringState:
        {
            return true;
        }
        case ThreadDetails::StopState:
        case ThreadDetails::CancelState:
        {
            IDEBUG() << " Thread " << Thread::currentThreadId() << " " << thread->getThreadDetails().threadState();
            return false;
        }
        case ThreadDetails::SystemEndState:
        {
            IDEBUG() << " Thread " << Thread::currentThreadId() << " " << thread->getThreadDetails().threadState();
            return false;
        }
        case ThreadDetails::PrintState:
        case ThreadDetails::CheckState:
        {
            IDEBUG() << " Thread " << Thread::currentThreadId() << " " << thread->getThreadDetails().threadState();
            return true;
        }
        default:
            IDEBUG() << "ERROR! Unable to match case! " << thread->getThreadDetails().threadState();
            break;
    }
    return false;
}

void Thread::sleep(int secs)
{
    // -- debug mode --
    Thread *self = Thread::currentThread();
    ThreadDetails::State currState = ThreadDetails::RunningState;
    if(self && self->isDebug())
    {
        currState = self->getThreadState();
        self->setThreadState(ThreadDetails::SleepingState);
    }
    // -- end --

#ifdef USE_GCC
    struct timespec wait_time = {secs, 0};
    nanosleep(&wait_time, 0);
#elif WIN32
    ::Sleep((int) (secs * THOUSAND) );
#else
    ASSERT(0);
#endif

    // -- debug mode --
    if(self && self->isDebug())
    {
        self->setThreadState(currState);
    }
    // -- end --

    // alternatives:
    // clock_nanosleep(CLOCK_REALTIME, &wait_time, 0);
    // clock_nanosleep(CLOCK_MONOTONIC, &wait_time, 0); // -> equivalent to nanosleep(&wait_time, 0);
    // clock_nanosleep(CLOCK_PROCESS_CPUTIME_ID, &wait_time, 0);
}

void Thread::msleep(int milliseconds)
{
    // -- debug mode --
    Thread *self = Thread::currentThread();;
    ThreadDetails::State currState = ThreadDetails::RunningState;
    if(self && self->isDebug())
    {
        currState = self->getThreadState();
        self->setThreadState(ThreadDetails::SleepingState);
    }
    // -- end --

#ifdef USE_GCC
    struct timespec wait_time = {0,0};
    Utility::ClockInitWaitTimespecMsec(wait_time, milliseconds);

    nanosleep(&wait_time, 0);
#elif WIN32
    ::Sleep(milliseconds);
#else
    ASSERT(0);
#endif

    // -- debug mode --
    if(self && self->isDebug())
    {
        self->setThreadState(currState);
    }
    // -- end --
}

void Thread::usleep(int usecs)
{
    // -- debug mode --
    Thread *self = Thread::currentThread();;
    ThreadDetails::State currState = ThreadDetails::RunningState;
    if(self && self->isDebug())
    {
        currState = self->getThreadState();
        self->setThreadState(ThreadDetails::SleepingState);
    }
    // -- end --

#ifdef USE_GCC
    struct timespec wait_time = {0,0};
    Utility::ClockInitWaitTimespecUsec(wait_time, usecs);

    nanosleep(&wait_time, 0);
#elif WIN32
    ::Sleep((int)(usecs/THOUSAND));
#else
    ASSERT(0);
#endif

    // -- debug mode --
    if(self && self->isDebug())
    {
        self->setThreadState(currState);
    }
    // -- end --
}

void Thread::nsleep(uint64 nanoseconds)
{
    // -- debug mode --
    Thread *self = Thread::currentThread();;
    ThreadDetails::State currState = ThreadDetails::RunningState;
    if(self && self->isDebug())
    {
        currState = self->getThreadState();
        self->setThreadState(ThreadDetails::SleepingState);
    }
    // -- end --

#ifdef USE_GCC
    struct timespec wait_time = {0,0};
    Utility::ClockInitWaitTimespecNsec(wait_time, nanoseconds);

    nanosleep(&wait_time, 0);
#elif WIN32
    ::Sleep((int)(nanoseconds/MILLION));
#else
    ASSERT(0);
#endif

    // -- debug mode --
    if(self && self->isDebug())
    {
        self->setThreadState(currState);
    }
    // -- end --
}

void Thread::interrupt()
{
    threadDetails_.setInterruptFlag(true);
    interruptible_.lockedWakeAll();
    interruptibleReadWrite_.lockedWakeAll();
}

bool Thread::isInterrupted()
{
    Thread *thread = Thread::currentThread();
    ASSERT(thread);

    return thread != nullptr ? thread->threadDetails_.isInterrupted() : false;
}

void Thread::unsetInterrupt()
{
    Thread *thread = Thread::currentThread();
    ASSERT(thread);

    if(thread)
    {
        thread->threadDetails_.setInterruptFlag(false);
    }
}

Templates::Notifyable<Thread, WaitCondition, Mutex>& Thread::interruptible()
{
    return interruptible_;
}

Templates::Notifyable<Thread, WaitCondition, ReadWriteLock>& Thread::interruptibleReadWrite()
{
    return interruptibleReadWrite_;
}

std::ostream &operator<<(std::ostream &ostr, const Thread &thread)
{
    ostr << thread.humanReadableId();
    if(!thread.threadName().empty())
        ostr << "@" << thread.threadName();
    return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const Thread *const thread)
{
    if(thread) ostr << *thread;

    return ostr;
}

}

