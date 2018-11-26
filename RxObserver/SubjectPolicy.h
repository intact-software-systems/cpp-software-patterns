#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

/** -------------------------------------------------------------------
Policy support in subject:
- Replay last n signals to connecting observer (History + Replay)
- Store n last signal values and signal to observers upon completion - Result (Continuous, AtEnd, Batch, etc)
- Sequential or Parallel signalling to observers (Computation) - Use sequential scheduling thread pool, or control execution here.
- Async or Sync signalling to observers (Invocation)
- Filter based on time (TimebasedFilter) -
- Filter based on content - not here, in a config perhaps
- Ordering - FIFO/Random/etc. If FIFO then share event queue (FlowController) between Subjects
- TODO: Max number of subscribers
- TODO: Policy on whether "subject should always be singleton (unique), i.e., the subject description should be unique". Yet again (key, value)
- TODO: Policy on "retry to submit event upon timeout, max number of retry attempts."
    - Algorithm: Upon timeout then put at back of queue and try again. This means that Event has to store retry status. NB! Only retry to the ones that did not receive!!
 ------------------------------------------------------------------- */
class DLL_STATE SubjectPolicy
{
public:
    SubjectPolicy();
    ~SubjectPolicy();

    Policy::Timeout Timeout() const;
    Policy::Computation Computation() const;
    Policy::Invocation Invocation() const;
    Policy::TimeBasedFilter MinSeparation() const;
    Policy::Replay Replay() const;
    Policy::Ordering Ordering() const;

    int MaxNumSubscribers() const;
    int MaxNumPublishers() const;

    FlowControllerPolicy FlowPolicy() const;

    static SubjectPolicy Default()
    {
        return SubjectPolicy();
    }

private:
    Policy::Replay          replay_;
    Policy::Timeout         timeout_;
    Policy::Computation     computation_;
    Policy::Invocation      invocation_;
    Policy::TimeBasedFilter filterInterval_;
    Policy::Ordering        ordering_;

    Policy::MaxLimit<int>   maxNumSubscribers_;
    Policy::MaxLimit<int>   maxNumPublishers_;

    FlowControllerPolicy    flowPolicy_;
};

}}
