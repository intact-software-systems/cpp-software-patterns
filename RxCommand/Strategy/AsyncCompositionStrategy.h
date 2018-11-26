#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/Strategy/CompositionStrategy.h"
#include"RxCommand/Strategy/CompositionAlgorithms.h"

namespace Reactor {

template <typename Return>
class AsyncCompositionStrategy
    : public CompositionStrategy<Return>
      , public StaticSingleton<AsyncCompositionStrategy<Return>>
{
public:
    virtual ~AsyncCompositionStrategy()
    { }

    virtual bool Perform(Policy::Criterion successCriterion, IList<Templates::Action0<Return>*>& actions, Command<Return>* command)
    {
        return CompositionAlgorithms::AsyncComposition<Return>(successCriterion, actions, command);
    }
};

}
