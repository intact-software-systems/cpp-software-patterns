#include "RxObserver/SubjectDescription.h"

namespace BaseLib { namespace Concurrent
{

SubjectDescription::SubjectDescription(std::string name, std::string domain)
    : Templates::ComparableImmutableType<Templates::AnyKey>(
          Templates::AnyKey(
              std::hash<std::string>()(name + domain)
              )
          )
    , name_(name)
    , domainId_(domain)
{ }

SubjectDescription::~SubjectDescription()
{ }

std::string SubjectDescription::Name() const
{
    return name_;
}

std::string SubjectDescription::Domain() const
{
    return domainId_;
}

}}
