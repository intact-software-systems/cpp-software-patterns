#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/Export.h"

namespace Reactor
{

class DLL_STATE CommandsHandle
        : public InstanceHandle
{
public:
    CommandsHandle(InstanceHandleType handle);
    virtual ~CommandsHandle();
};

}
