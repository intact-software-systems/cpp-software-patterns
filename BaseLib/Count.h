#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib {

template <typename T>
class CountType : public Templates::ComparableMutableType<T>
{
public:
    CountType()
        : Templates::ComparableMutableType<T>()
    { }

    CountType(T count)
        : Templates::ComparableMutableType<T>(count)
    { }

    virtual ~CountType()
    { }

    T Number() const
    {
        return this->delegate();
    }

    bool IsZero() const
    {
        return this->delegate() == 0;
    }

    // ---------------------------------------
    // Factory functions
    // ---------------------------------------

    static CountType<T> Infinite()
    {
        return CountType<T>(std::numeric_limits<T>::max());
    }

    static CountType<T> Create(T count)
    {
        return CountType<T>(count);
    }

    static CountType<T> Zero()
    {
        return CountType<T>();
    }

    // ---------------------------------------
    // print out
    // ---------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const CountType<T>& count)
    {
        ostr << count.delegate();
        return ostr;
    }
};

typedef CountType<int>   Count;
typedef CountType<int64> CountInt64;

}
