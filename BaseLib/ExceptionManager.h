#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Mutex.h"
#include"BaseLib/Thread.h"
#include"BaseLib/ObjectBase.h"
#include"BaseLib/Export.h"

namespace BaseLib
{

class Exception;

class DLL_STATE ExceptionManager
        : public Thread
        , public ObjectBase
{
private:
    ExceptionManager(std::string name = std::string("BaseLib.Manager.Exception"));
    ~ExceptionManager();

public:
    static ExceptionManager*	getOrCreateExceptionManager();

    bool AddObject(Exception *base);
    bool RemoveObject(Exception *base);

private:
    virtual void run();

private:
    static ExceptionManager *exceptionManager_;

    typedef std::set<Exception*>	SetException;
    SetException					setException_;
    mutable	Mutex					managerMutex_;
};

}

