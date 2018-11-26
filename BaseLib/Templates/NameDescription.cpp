#include "BaseLib/Templates/NameDescription.h"

namespace BaseLib { namespace Templates {

NameDescription::NameDescription()
    : Templates::ComparableImmutableType<Templates::AnyKey>(Templates::AnyKey(std::hash<std::string>()("dom")))
    , name_("aname")
    , domainId_("dom")
{ }

NameDescription::NameDescription(std::string name)
    : Templates::ComparableImmutableType<Templates::AnyKey>(Templates::AnyKey(std::hash<std::string>()(name + "dom")))
    , name_(name)
    , domainId_("dom")
{ }

NameDescription::NameDescription(std::string name, std::string domain)
    : Templates::ComparableImmutableType<Templates::AnyKey>(Templates::AnyKey(std::hash<std::string>()(name + domain)))
    , name_(name)
    , domainId_(domain)
{ }

NameDescription::~NameDescription()
{ }

std::string NameDescription::Name() const
{
    return name_;
}

std::string NameDescription::Domain() const
{
    return domainId_;
}
}}
