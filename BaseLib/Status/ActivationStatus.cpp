#include "BaseLib/Status/ActivationStatus.h"

namespace BaseLib { namespace Status {

ActivationStatus::ActivationStatus()
    : activationStatus_(Duration::FromMinutes(2), Count::Create(2))
{ }

ActivationStatus::~ActivationStatus()
{ }

bool ActivationStatus::IsActive() const
{
    return activationStatus_.CurrentState() == ActivationKind::ACTIVE;
}

bool ActivationStatus::Activate()
{
    activationStatus_.NextState(ActivationKind::ACTIVE, 1);
    return true;
}

bool ActivationStatus::Deactivate()
{
    activationStatus_.NextState(ActivationKind::INACTIVE, 1);
    return true;
}

}}
