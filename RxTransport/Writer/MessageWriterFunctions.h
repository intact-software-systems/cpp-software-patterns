#pragma once

#include"RxTransport/CommonDefines.h"
#include"RxTransport/Export.h"

namespace BaseLib { namespace Concurrent {

class DLL_STATE MessageWriterFunctions
    : public Status::ActivationStatus
{
public:
    MessageWriterFunctions();
    MessageWriterFunctions(ScheduledFutureTask<bool> heartbeatTask, ScheduledFutureTask<bool> nackTask, ScheduledFutureTask<bool> dataFuture);
    virtual ~MessageWriterFunctions();

    virtual bool Activate() override;
    virtual bool Deactivate() override;

    ScheduledFutureTask<bool> heartbeatTask;
    ScheduledFutureTask<bool> nackTask;
    ScheduledFutureTask<bool> dataSchedFuture;
};

}}
