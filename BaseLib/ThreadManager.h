/***************************************************************************
                          ThreadManager.h  -  description
                             -------------------
    begin                : Sun Mar 13 2011
    copyright            : (C) 2010 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Singleton.h"
#include"BaseLib/Thread.h"
#include"BaseLib/MutexBare.h"
#include"BaseLib/MutexLocker.h"
#include"BaseLib/ObjectBase.h"

#include"BaseLib/Export.h"
namespace BaseLib
{

class DLL_STATE ThreadManager : public Thread
{
    typedef MutexTypeLocker<MutexBare> Locker;

private:
    ThreadManager(std::string name = std::string("BaseLib.Manager.Thread"));

public:
    virtual ~ThreadManager();

    friend class Singleton<ThreadManager>;

public:
    CLASS_TRAITS(ThreadManager)

    typedef int64            						ThreadId;
    typedef std::map<ThreadId, Thread*>				MapIdThread;	// threads that implement Thread
    typedef std::map<ThreadId, std::string>			MapIdThreadName;// threads that does not implement any object

public:
    static ThreadManager& 	GetInstance();

public:
    bool	AddThread(Thread*);
    bool 	AddThread(Thread*, ThreadId id);
    bool 	AddThread(ThreadId id, std::string name);

    bool 	RemoveThread(Thread*);

    // TODO: Dangerous?
    //bool 	StopThread(ThreadId id);
    //bool  RestartThread()

    Thread*	GetThread(ThreadId id);

protected:
    virtual void run();

private:
    static Singleton<ThreadManager> threadManager_;

private:
    MapIdThread					mapIdThread_;
    MapIdThreadName				mapIdThreadName_;

    mutable MutexBare			managerMutex_;
    ThreadId					threadIdAssigner_;
};

}

