/***************************************************************************
                          Thread.h  -  description
                             -------------------
    begin                : Sun May 9 2010
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#pragma once

#include <string>
#include <iostream>
#include <limits.h>

#include <BaseLib/Templates/Synchronization.h>
#include"BaseLib/CommonDefines.h"
#include"BaseLib/MutexBare.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/WaitCondition.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class ThreadManager;

class DLL_STATE ThreadDetails
{
public:
    enum Priority
    {
        IdlePriority = 0,
        LowestPriority,
        LowPriority,
        NormalPriority,
        HighPriority,
        HighestPriority,
        TimeCriticalPriority,
        InheritPriority
    };

    enum State
    {
        NoState = 0,

        // states set by the thread itself
        RunningState,
        RunningMutexedState,
        SleepingState,
        WaitingState,
        LockingState,
        UnlockingState,
        MonitoringState,

        // states set by thread itself or other threads
        // to enable ThreadManager to detect faulty threads
        StopState,
        CancelState,
        SystemEndState,
        PrintState,
        CheckState
    };

    friend std::ostream& operator<<(std::ostream& ostr, const ThreadDetails::State &status);

public:
    ThreadDetails(std::string name = std::string(""))
        : threadId_(0)
        , humanReadableThreadId_(0)
        , threadRunning_(false)
        , threadPriority_(ThreadDetails::InheritPriority)
        , threadState_(ThreadDetails::NoState)
        , threadName_(name)
        , mutex_(MutexPolicy::No())
        , interruptedFlag_(false)
    { }
    ~ThreadDetails()
    { }

#ifdef USE_GCC
    pthread_attr_t&		threadAttributes() 				{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadAttributes_; }
#elif _WIN32
    HANDLE				threadAttributes() 				{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadHandle_; }
#endif

    int64		            threadId()			const	{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadId_; }
    int64            		humanReadableId()	const	{ MutexTypeLocker<MutexBare> lock(&mutex_); return humanReadableThreadId_; }
    bool					threadRunning()		const	{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadRunning_; }
    ThreadDetails::Priority threadPriority()	const 	{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadPriority_; }
    ThreadDetails::State	threadState()		const	{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadState_; }
    const std::string&		threadName()		const 	{ MutexTypeLocker<MutexBare> lock(&mutex_); return threadName_; }

    bool                    isInterrupted()     const   { return interruptedFlag_; }

#ifdef USE_GCC
    void threadAttributes(const pthread_attr_t &attr)  	{ threadAttributes_ = attr; }
#elif _WIN32
    void threadAttributes(HANDLE handle)  				{ threadHandle_ = handle; }
#endif

    void threadId(int64 id)					            { MutexTypeLocker<MutexBare> lock(&mutex_); threadId_ = id; }
    void humanReadableId(int64 id)			            { MutexTypeLocker<MutexBare> lock(&mutex_); humanReadableThreadId_ = id; }
    void threadRunning(bool run)						{ MutexTypeLocker<MutexBare> lock(&mutex_); threadRunning_ = run; }
    void threadPriority(ThreadDetails::Priority p)		{ MutexTypeLocker<MutexBare> lock(&mutex_); threadPriority_ = p; }
    void threadState(ThreadDetails::State s)			{ MutexTypeLocker<MutexBare> lock(&mutex_); threadState_ = s; }
    void threadName(std::string name)					{ MutexTypeLocker<MutexBare> lock(&mutex_); threadName_ = name; }

    void setInterruptFlag(bool flag)                           { interruptedFlag_ = flag; }

private:

#ifdef USE_GCC
    pthread_attr_t 		threadAttributes_;
#elif _WIN32
    HANDLE				threadHandle_;
#endif

    int64               threadId_;
    int64               humanReadableThreadId_;
    bool				threadRunning_;
    Priority			threadPriority_;
    State				threadState_;
    std::string			threadName_;
    mutable MutexBare	mutex_;

    std::atomic<bool>   interruptedFlag_;

private:
    ThreadDetails(const ThreadDetails &) {}
    ThreadDetails& operator=(const ThreadDetails&) { return *this; }
};

std::ostream& operator<<(std::ostream& ostr, const ThreadDetails &details);

//-----------------------------------------------------------------------
//					class Thread
//-----------------------------------------------------------------------
class DLL_STATE Thread
{
public:
    Thread(std::string name = std::string(""));
    Thread(std::string name, bool debug);
    virtual ~Thread();

    friend class ThreadManager;

#ifdef USE_GCC
    static void* dispatch(void *thread_obj);
#elif _WIN32
    static unsigned int __stdcall dispatch(void *thread_obj);
#endif

    virtual bool start(ThreadDetails::Priority priority = ThreadDetails::InheritPriority);
    virtual void exit();
    //void quit();
    virtual void terminate();
    virtual bool wait(int64 msecs = LONG_MAX);

    virtual void run() = 0;

    bool 					isFinished() const;
    bool 					isRunning() const;
    ThreadDetails::Priority priority() const;
    const std::string&		threadName() const;
    int64           		threadId() const;
    int64		            humanReadableId() const;

    void					humanReadableId(int64 id);

    void 					setPriority(ThreadDetails::Priority priority);
    void					setThreadName(const std::string &name);

public:
    inline void					setThreadState(ThreadDetails::State state) 	{ threadDetails_.threadState(state); }
    inline ThreadDetails::State getThreadState() const						{ return threadDetails_.threadState(); }
    inline ThreadDetails&		getThreadDetails()							{ return threadDetails_; }

    inline bool					isDebug()	const	{ return debug_; }

public:
    static Thread*			currentThread();
    static int64 	        currentThreadId();
    static int 				idealThreadCount();
    static void				yieldCurrentThread();
    static bool				checkThreadState();
    static int				processId();

public:
    void interrupt();
    static bool isInterrupted();
    static void unsetInterrupt();

    Templates::Notifyable<Thread, WaitCondition, Mutex>&         interruptible();
    Templates::Notifyable<Thread, WaitCondition, ReadWriteLock>& interruptibleReadWrite();

public:
    static void sleep(int secs);
    static void msleep(int msecs);
    static void usleep(int usecs);
    static void nsleep(uint64 nsecs);
    //static void	setTerminationEnabled(bool enabled = true);

private:
    void cleanUp();

private:
    Templates::Notifyable<Thread, WaitCondition, Mutex>         interruptible_ =  Templates::Notifyable<Thread, WaitCondition, Mutex>(MutexPolicy::No());
    Templates::Notifyable<Thread, WaitCondition, ReadWriteLock> interruptibleReadWrite_ = Templates::Notifyable<Thread, WaitCondition, ReadWriteLock>(MutexPolicy::No());

private:
    ThreadDetails			threadDetails_;
    mutable MutexBare		threadMutex_;
    bool					debug_;

private:
    Thread(const Thread &) {}
    Thread& operator=(const Thread&) { return *this; }
};

DLL_STATE std::ostream& operator<<(std::ostream &ostr, const Thread &thread);
DLL_STATE std::ostream& operator<<(std::ostream &ostr, const Thread *const thread);


class ThreadNoOp : public Thread
{
public:
    ThreadNoOp(const std::string& name, bool debug) : Thread(name, debug)
    { }
    virtual ~ThreadNoOp()
    { }

    virtual void run() { ASSERT(0); }
};

}


