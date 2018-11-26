/***************************************************************************
                          WaitCondition.h  -  description
                             -------------------
    begin                : Sat Mar 12 2011
    copyright            : (C) 2011 by Knut-Helge Vik
    email                : knuthelge@vik.gs
 ***************************************************************************/
#pragma once

#include "BaseLib/Mutex.h"

#ifdef WIN32
#include <process.h>
#endif

#include "BaseLib/Export.h"

namespace BaseLib {

class Mutex;

class WaitConditionPrivate;

class ReadWriteLock;

class DLL_STATE WaitCondition
{
public:
    WaitCondition();
    ~WaitCondition();

    bool wait(Mutex* mutex, int64 milliseconds = LONG_MAX);
    bool wait(ReadWriteLock* readWriteLock, int64 milliseconds = LONG_MAX);

    void wakeAll();
    void wakeOne();

    int numWaiters() const;

private:
    std::shared_ptr<WaitConditionPrivate> self;
};

}
