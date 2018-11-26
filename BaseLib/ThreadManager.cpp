/***************************************************************************
                          ThreadManager.h  -  description
                             -------------------
    begin                : Sun Mar 13 2011
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#include "BaseLib/ThreadManager.h"
#include "BaseLib/CommonDefines.h"
#include "BaseLib/Exception.h"
#include "BaseLib/LogDebug.h"
#include "BaseLib/Debug.h"

using namespace std;

namespace BaseLib
{

Singleton<ThreadManager> ThreadManager::threadManager_;

// ---------------------------------------------------------------------
// TODO: Deadlock when RemoveThread (or any mutexed function) is called and then inside IDEBUG() this->GetThread is called!
// To resolve use thread-safe collection
// ---------------------------------------------------------------------
ThreadManager::ThreadManager(std::string name)
    : Thread(name, false)
    , managerMutex_(MutexPolicy::No())
    , threadIdAssigner_(0)
{
    //start();
}

ThreadManager::~ThreadManager()
{ }

ThreadManager& ThreadManager::GetInstance()
{
    return threadManager_.GetRef();
}

// TODO: Check out system_manager.h in planetlab to see what can be added
void ThreadManager::run()
{
    try
    {
        //IDEBUG() << "Thread manager is up!"; ;
        while(true)
        {
            // do something!
            // Wait out all threads!
            // TODO: Set flag in all threads and make them use pause() to suspend themselves!
            sleep(1);
        }
        ICRITICAL() << "ThreadManager stopped!";
    }
    catch(Exception ex)
    {
        ICRITICAL() << "Unhandled exception " << ex.msg();
    }
}

bool ThreadManager::AddThread(Thread *thread)
{
    Locker lock(&managerMutex_);

    if(mapIdThread_.count(thread->threadId()) > 0)
    {
        //ICRITICAL() << "Thread " << thread->threadId() << " already registered!";
        return false;
    }

    thread->humanReadableId(threadIdAssigner_++);
    mapIdThread_[thread->threadId()] = thread;
    //IDEBUG() << "Thread " << thread->threadId() << " started ";

    return true;
}

bool ThreadManager::AddThread(Thread *thread, ThreadId id)
{
    Locker lock(&managerMutex_);

    if(mapIdThread_.count(id) > 0)
    {
        //ICRITICAL() << "Thread " << id << " already registered!";
        return false;
    }

    thread->humanReadableId(threadIdAssigner_++);
    mapIdThread_[id] = thread;
    //IDEBUG() << "Thread " << id << " " << thread->threadName() << " started ";

    return true;
}

bool ThreadManager::AddThread(ThreadId id, std::string name)
{
    Locker lock(&managerMutex_);

    if(mapIdThread_.count(id) > 0)
    {
        //ICRITICAL() << "Thread " << id << " already registered!";
        return false;
    }
    mapIdThread_[id] = NULL;

    if(mapIdThreadName_.count(id) > 0)
    {
        //ICRITICAL() << "Thread " << id << " already registered!";
        return false;
    }

    mapIdThreadName_[id] = name;
    //IDEBUG() << "Thread " << id << " " << name << " registered ";

    return true;
}

// ---------------------------------------------------------------------
// TODO: Deadlock when RemoveThread is called and then inside IDEBUG() this->GetThread is called!
// To resolve use thread-safe collection -- NO!
// Cannot be resolved as long as CurrentThread is accessed inside IDEBUG() etc,,
// but attempted to check if this->threadId() equals calling thread id
// ---------------------------------------------------------------------

bool ThreadManager::RemoveThread(Thread *thread)
{
    if(thread->threadId() == this->threadId()) return true;

    Locker lock(&managerMutex_);

    if(mapIdThread_.count(thread->threadId()) <= 0)
    {
        //IWARNING() << "Thread " << thread->threadName() << "(" << thread->threadId() << ")" << " already stopped!";
        return false;
    }

    mapIdThread_.erase(thread->threadId());
    //IDEBUG() << "Thread stopped: " << thread->threadId() << " " << thread->threadName(); //thread->getThreadDetails();

    return true;
}

Thread* ThreadManager::GetThread(ThreadId id)
{
    if(id == this->threadId()) return this;

    {
        Locker lock(&managerMutex_);
        MapIdThread::iterator entry = mapIdThread_.find(id);

        if(entry != mapIdThread_.end())
        {
            return entry->second;
        }
    }

    ThreadNoOp* threadNoOp = new ThreadNoOp(std::string("main.thread"), false);
    threadNoOp->getThreadDetails().threadId(id);

    BaseLib::ThreadManager::GetInstance().AddThread(threadNoOp);

    return threadNoOp;
}

}
