#ifndef BaseLib_Templates_ProtectedBool_h_IsIncluded
#define BaseLib_Templates_ProtectedBool_h_IsIncluded

#include"BaseLib/CommonDefines.h"
#include"BaseLib/WriteLocker.h"
#include"BaseLib/ReadLocker.h"

namespace BaseLib { namespace Templates
{

class ProtectedBool
{
    bool    _x;
    mutable ReadWriteLock _m;

public:
    ProtectedBool(bool i = false)
        :_x(i)
    {
    }

    CLASS_TRAITS(ProtectedBool)

    ProtectedBool& operator=( bool i )
    {
        WriteLocker lock(&_m);
        _x = i;
        return *this;
    }

    bool operator==( bool i ) const
    {
        ReadLocker lock(&_m);
        return ( _x == i );
    }

    operator bool() const   // implicit typecast operator
    {
        ReadLocker lock(&_m);
        return _x;
    }

    bool get() const
    {
        ReadLocker lock(&_m);
        return _x;
    }

    bool getAndSetIf(bool cond)
    {
        WriteLocker lock(&_m);

        if(cond != _x)
        {
            return false;
        }
        else
        {
            _x = true;
            return true;
        }
    }

    void set(const bool &b)
    {
        WriteLocker lock(&_m);
        _x = b;
    }

    void print( std::ostream& ostr ) const
    {
        ReadLocker lock(&_m);
        if (_x)
            ostr << "true";
        else
            ostr << "false";
    }

    friend std::ostream& operator << ( std::ostream& ostr, const ProtectedBool& pb )
    {
        pb.print(ostr);
        return ostr;
    }
};

}}

#endif
