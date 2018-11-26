#include"BaseLib/Policy/Flyweight.h"

namespace BaseLib { namespace Policy {

Flyweight::Flyweight(FlyweightKind kind)
    : Templates::ComparableImmutableType<FlyweightKind>(kind)
{ }

Flyweight::~Flyweight()
{ }

FlyweightKind Flyweight::Kind() const
{
    return this->Clone();
}

bool Flyweight::IsCopyOnWrite() const
{
    return FlyweightKind::COPY_ON_WRITE == this->delegate();
}

bool Flyweight::IsCopyOnRead() const
{
    return FlyweightKind::COPY_ON_READ == this->delegate();
}

bool Flyweight::IsNoCopy() const
{
    return FlyweightKind::NO == this->delegate();
}

Flyweight Flyweight::CopyOnWrite()
{
    return Flyweight(FlyweightKind::COPY_ON_WRITE);
}

Flyweight Flyweight::CopyOnRead()
{
    return Flyweight(FlyweightKind::COPY_ON_READ);
}

Flyweight Flyweight::No()
{
    return Flyweight(FlyweightKind::NO);
}

}}

