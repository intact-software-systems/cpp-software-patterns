#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/Strategy/CompositionStrategy.h"
#include"RxCommand/Strategy/CompositionAlgorithms.h"

namespace Reactor {

template <typename Return>
class AggregateCompositionStrategy
    : public CompositionStrategy<Return>
      , public StaticSingleton<AggregateCompositionStrategy<Return>>
{
public:
    virtual ~AggregateCompositionStrategy()
    { }

    virtual bool Perform(Policy::Criterion successCriterion, IList<Templates::Action0<Return>*>& actions, Command<Return>* command)
    {
        return CompositionAlgorithms::AggregateComposition<Return>(successCriterion, actions, command);
    }
};

}
