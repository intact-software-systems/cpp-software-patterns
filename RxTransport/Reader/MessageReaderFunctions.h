#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

struct DLL_STATE MessageReaderFunctions : public Status::ActivationStatus
{
    MessageReaderFunctions();
    MessageReaderFunctions(ScheduledFutureTask<bool> ackNack, ScheduledFutureTask<bool> request);
    virtual ~MessageReaderFunctions();

    virtual bool Activate() override;
    virtual bool Deactivate() override;

    ScheduledFutureTask<bool> ackNackTask;
    ScheduledFutureTask<bool> requestTask;
};

}}
