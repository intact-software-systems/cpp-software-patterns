#include "MessageReaderFunctions.h"

namespace BaseLib { namespace Concurrent {

MessageReaderFunctions::MessageReaderFunctions()
{ }

MessageReaderFunctions::MessageReaderFunctions(ScheduledFutureTask<bool> ackNack, ScheduledFutureTask<bool> request)
    : ackNackTask(ackNack)
    , requestTask(request)
{ }

MessageReaderFunctions::~MessageReaderFunctions()
{
    ackNackTask.Cancel();
    requestTask.Cancel();
}

bool MessageReaderFunctions::Activate()
{
    ackNackTask.Resume();
    requestTask.Resume();

    return Status::ActivationStatus::Activate();
}

bool MessageReaderFunctions::Deactivate()
{
    ackNackTask.Suspend();
    requestTask.Suspend();

    return Status::ActivationStatus::Deactivate();
}

}}
