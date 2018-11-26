#pragma once

#include"RxCommand/IncludeExtLibs.h"
#include"RxCommand/CommandBase.h"
#include"RxCommand/Strategy/CompositionStrategy.h"
#include"RxCommand/Strategy/CompositionAlgorithms.h"

namespace Reactor {

template <typename Return>
class IndividualCompositionStrategy
    : public CompositionStrategy<Return>
      , public StaticSingleton<IndividualCompositionStrategy<Return>>
{
public:
    virtual ~IndividualCompositionStrategy()
    { }

    virtual bool Perform(Policy::Criterion successCriterion, IList<Templates::Action0<Return>*>& actions, Command <Return>* command)
    {
        return CompositionAlgorithms::IndividualComposition<Return>(successCriterion, actions, command);
    }
};

}
