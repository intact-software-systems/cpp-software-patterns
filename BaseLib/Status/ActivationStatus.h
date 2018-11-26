#pragma once

#include <BaseLib/Templates/Lifecycle.h>
#include "BaseLib/Status/Metric.h"
#include "BaseLib/Export.h"

namespace BaseLib { namespace Status {

class DLL_STATE ActivationStatus : Templates::ActivationMethods
{
private:
    enum class DLL_STATE ActivationKind
    {
        ACTIVE,
        INACTIVE,
    };

public:
    ActivationStatus();
    virtual ~ActivationStatus();

    virtual bool IsActive() const;
    virtual bool Activate();
    virtual bool Deactivate();

private:
    Status::EventStatusTracker<ActivationKind, int> activationStatus_;
};

}}


