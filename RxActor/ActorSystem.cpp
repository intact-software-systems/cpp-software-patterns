#include "ActorSystem.h"

namespace RxActor {


std::string ActorSystem::ToString() const
{
    std::stringstream ostr;

    ostr << "ActorSystem(";
    for(auto entry :this->context()->collection())
    {
        ostr << entry.first << ", ";
    }
    ostr << ")";

    return ostr.str();
}

bool ActorSystem::Activate(ActorId handle)
{
    return false;
}

bool ActorSystem::Deactivate(ActorId handle)
{
    return false;
}

bool ActorSystem::IsActive(ActorId handle)
{
    return false;
}

bool ActorSystem::Activate(ActorPath handle)
{
    return false;
}

bool ActorSystem::Deactivate(ActorPath handle)
{
    return false;
}

bool ActorSystem::IsActive(ActorPath handle)
{
    return false;
}
}
