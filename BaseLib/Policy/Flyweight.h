#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE FlyweightKind : char
{
    NO            = '-',
    COPY_ON_WRITE = 'W',
    COPY_ON_READ  = 'R'
};

class DLL_STATE Flyweight
    : public Templates::ComparableImmutableType<FlyweightKind>
{
public:
    Flyweight(FlyweightKind kind = FlyweightKind::NO);
    virtual ~Flyweight();

    FlyweightKind Kind() const;

    bool IsCopyOnWrite() const;
    bool IsCopyOnRead() const;
    bool IsNoCopy() const;

    static Flyweight CopyOnWrite();
    static Flyweight CopyOnRead();
    static Flyweight No();
};

}}
