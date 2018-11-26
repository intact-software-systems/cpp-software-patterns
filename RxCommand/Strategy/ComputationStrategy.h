#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"

namespace Reactor {

class CommandsGroup;

class ComputationStrategy
    : public Templates::Strategy2<IList<CommandBase::Ptr>, int, const CommandsGroup&>
{
public:
    virtual ~ComputationStrategy()
    { }
};

}
