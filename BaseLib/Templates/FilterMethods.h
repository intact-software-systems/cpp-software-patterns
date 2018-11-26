#ifndef BaseLib_Templates_FilterMethods_h_IsIncluded
#define BaseLib_Templates_FilterMethods_h_IsIncluded

#include "BaseLib/CommonDefines.h"

namespace BaseLib { namespace Templates
{

template <typename Return, typename Arg1>
class FilterMethod1
{
public:
    virtual ~FilterMethod1() {}

    virtual Return Filter(Arg1) = 0;
};

template <typename Return, typename Arg1, typename Arg2>
class FilterMethod2
{
public:
    virtual ~FilterMethod2() {}

    virtual Return Filter(Arg1, Arg2) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3>
class FilterMethod3
{
public:
    virtual ~FilterMethod3() {}

    virtual Return Filter(Arg1, Arg2, Arg3) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
class FilterMethod4
{
public:
    virtual ~FilterMethod4() {}

    virtual Return Filter(Arg1, Arg2, Arg3, Arg4) = 0;
};

template <typename Return, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
class FilterMethod5
{
public:
    virtual ~FilterMethod5() {}

    virtual Return Filter(Arg1, Arg2, Arg3, Arg4, Arg5) = 0;
};

}}

#endif
