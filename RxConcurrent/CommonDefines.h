#pragma once

#include <RxConcurrent/ThreadPool/TaskPolicy.h>
#include"RxConcurrent/IncludeExtLibs.h"
#include"RxConcurrent/Export.h"

namespace BaseLib { namespace Concurrent {

class ScheduledTasksBase;


enum class DLL_STATE FlowStatusKind
{
    NO,
    ENQUEUED,
    DEQUEUED,
    REJECTED,
    FILTERED
};

typedef Status::EventStatusTracker<FlowStatusKind, int> FlowControllerStatus;

typedef std::function<Duration(const Status::ExecutionStatus&, const TaskPolicy&)> ScheduleFunction;

namespace Function
{
static const ScheduleFunction defaultScheduleFunction =
                                  [](const Status::ExecutionStatus& executionStatus, const TaskPolicy& taskPolicy) -> Duration
                                  {
                                      return BaseLib::Strategy::ComputeEarliestReadyIn::Perform(
                                          executionStatus,
                                          taskPolicy.Attempt(),
                                          taskPolicy.Interval(),
                                          taskPolicy.RetryInterval(),
                                          taskPolicy.Timeout()
                                      );
                                  };

}

}}
