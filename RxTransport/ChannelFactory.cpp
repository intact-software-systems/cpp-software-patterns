#include "ChannelFactory.h"

namespace BaseLib { namespace Concurrent {

ChannelFactory::ChannelFactory()
    : Templates::ContextDataShared<LookupScopeManager>()
{ }

ChannelFactory::~ChannelFactory()
{ }

bool ChannelFactory::Activate(DomainScopeHandle handle)
{
    return this->context()->Apply(
        handle,
        [](ChannelScopeManagerPtr manager) { return manager->Activate(); }
    );
}

bool ChannelFactory::Deactivate(DomainScopeHandle handle)
{
    return this->context()->Apply(
        handle,
        [](ChannelScopeManagerPtr manager) { return manager->Deactivate(); }
    );
}

bool ChannelFactory::IsActive(DomainScopeHandle handle)
{
    return this->context()->Apply(
        handle,
        [](ChannelScopeManagerPtr manager) { return manager->IsActive(); }
    );
}

bool ChannelFactory::Activate(ChannelHandle handle)
{
    return this->context()->Apply(
        DomainScopeHandle{handle.first(), handle.second()},
        [&handle](ChannelScopeManagerPtr manager) { return manager->ActivateById(handle.third()); }
    );
}

bool ChannelFactory::Deactivate(ChannelHandle handle)
{
    return this->context()->Apply(
        DomainScopeHandle{handle.first(), handle.second()},
        [&handle](ChannelScopeManagerPtr manager) { return manager->DeactivateById(handle.third()); }
    );
}

bool ChannelFactory::IsActive(ChannelHandle handle)
{
    return this->context()->Apply(
        DomainScopeHandle{handle.first(), handle.second()},
        [&handle](ChannelScopeManagerPtr manager) { return manager->IsActiveById(handle.third()); }
    );
}

std::string ChannelFactory::ToString() const
{
    std::stringstream ostr;

    ostr << "ChannelFactory(";
    for(auto entry :this->context()->collection())
    {
        ostr << entry.first << ", ";
    }
    ostr << ")";

    return ostr.str();
}

}}
