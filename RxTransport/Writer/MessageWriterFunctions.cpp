#include "MessageWriterFunctions.h"

namespace BaseLib { namespace Concurrent {

MessageWriterFunctions::MessageWriterFunctions()
{ }

MessageWriterFunctions::MessageWriterFunctions(ScheduledFutureTask<bool> heartbeatTask, ScheduledFutureTask<bool> nackTask, ScheduledFutureTask<bool> dataFuture)
    : heartbeatTask(heartbeatTask)
    , nackTask(nackTask)
    , dataSchedFuture(dataFuture)
{ }

MessageWriterFunctions::~MessageWriterFunctions()
{
    heartbeatTask.Cancel();
    nackTask.Cancel();
    dataSchedFuture.Cancel();
}

bool MessageWriterFunctions::Activate()
{
    heartbeatTask.Resume();
    nackTask.Resume();
    dataSchedFuture.Resume();

    return Status::ActivationStatus::Activate();
}

bool MessageWriterFunctions::Deactivate()
{
    heartbeatTask.Suspend();
    nackTask.Suspend();
    dataSchedFuture.Suspend();

    return Status::ActivationStatus::Deactivate();
}

}}
