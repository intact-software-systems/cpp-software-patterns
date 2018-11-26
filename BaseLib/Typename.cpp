#include "BaseLib/Typename.h"

namespace BaseLib
{

Typename::Typename(std::string name)
    : Templates::ComparableImmutableType<std::string>(name)
{ }

Typename::~Typename()
{ }

std::string Typename::Name() const
{
    return this->Clone();
}

}

