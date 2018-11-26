#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandPolicy.h"

namespace Reactor {

template <typename T>
class CommandStrategyPolicyBase
{
public:
    virtual ~CommandStrategyPolicyBase()
    { }

    virtual T Strategy() const = 0;

    virtual CommandPolicy Policy() const = 0;
};

template <typename T>
class CommandStrategyPolicy : public CommandStrategyPolicyBase<T>
{
public:
    CommandStrategyPolicy(CommandPolicy policy, T strategy)
        : policy_(policy)
        , strategy_(strategy)
    { }

    virtual T Strategy() const
    {
        return strategy_;
    }

    virtual CommandPolicy Policy() const
    {
        return policy_;
    }

private:
    const CommandPolicy policy_;
    const T             strategy_;
};

}
