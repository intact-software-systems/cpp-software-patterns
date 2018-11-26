#pragma once

#include"RxMonitor/IncludeExtLibs.h"

namespace Reactor { namespace Strategy
{

class CommandMonitorStrategy
        : public BaseLib::Templates::Strategy0<int>
{
public:
    virtual ~CommandMonitorStrategy() {}
};

class MonitorAllCommandControllers
        : public CommandMonitorStrategy
        , public StaticSingletonPtr<MonitorAllCommandControllers>
{
public:
    virtual ~MonitorAllCommandControllers() {}

    CLASS_TRAITS(MonitorAllCommandControllers)

    virtual int Perform();

    static int monitorControllers();
};

}}
