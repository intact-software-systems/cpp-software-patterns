#pragma once

#include"RxCommand/IncludeExtLibs.h"

namespace Reactor
{

template <typename Return>
class Command;

template <typename Return>
class CompositionStrategy
        : public Templates::Strategy3<bool, Policy::Criterion, IList<Templates::Action0<Return>*> &, Command<Return>*>
{
public:
    virtual ~CompositionStrategy() {}
};

}
