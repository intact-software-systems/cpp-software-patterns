#pragma once

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <assert.h>
//#include <fstream>
#include <time.h>
//#include <sys/types.h>
#include <string.h>
#include <math.h>

#include <memory>
#include <iostream>
#include <sstream>
#include <limits>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <ostream>
#include <functional>
#include <thread>
#include <stdexcept>
#include <exception>
#include <ctime>
#include <bitset>
#include <typeinfo>
#include <algorithm>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <atomic>
#include <type_traits>

//#include<boost/pointer_cast.hpp>
#ifdef MACX

#include <mach/clock.h>
#include <mach/mach.h>
//#include <mach/mach_time.h>
#endif

#ifdef USE_GCC

#include<sys/wait.h>
//#include<sys/sysinfo.h>
//#include<atomic>

#include<sys/time.h>
#include<inttypes.h>
#include<pthread.h>

#else
#include<sys/timeb.h>
#include<time.h>

/*
 http://stackoverflow.com/questions/1372480/c-redefinition-header-files

As others suggested, the problem is when windows.h is included before WinSock2.h.
Because windows.h includes winsock.h. You can not use both WinSock2.h and winsock.h.

Solutions:

Include WinSock2.h before windows.h. In the case of precompiled headers, you should solve it there.
In the case of simple project, it is easy. However in big projects (especially when writing portable
code, without precompiled headers) it can be very hard, because when your header with WinSock2.h
is included, windows.h can be already included from some other header/implementation file.

Define WIN32_LEAN_AND_MEAN before windows.h or project wide. But it will exclude many other
stuff you may need and you should include it by your own.

Define _WINSOCKAPI_ before windows.h or project wide. But when you include WinSock2.h you get
macro redefinition warning.

Use windows.h instead of WinSock2.h when winsock.h is enough for your project (in most cases it is).
This will probably result in longer compilation time but solves any errors/warnings.
*/

#ifndef WINDOWS_HEADERS_ARE_INCLUDED
#define WINDOWS_HEADERS_ARE_INCLUDED
#include<winsock2.h>
#include<windows.h>
#include<ws2tcpip.h>
#include<iphlpapi.h>
#include<Objbase.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Ole32.lib")

#endif

#endif

#include"BaseLib/Export.h"

namespace BaseLib {

using Clock = std::chrono::high_resolution_clock;

template <class Duration>
using TimePoint = std::chrono::time_point<Clock, Duration>;

using TimePointNs = TimePoint<std::chrono::nanoseconds>;

DLL_STATE std::string GetTimeStamp();
DLL_STATE int         GetTimeStampInteger();
DLL_STATE std::string WriteFunctionToString(const char* function, int line);
DLL_STATE std::string WriteTimedFunctionToString(const std::string& time, const char* function, int line);
DLL_STATE void        WriteAssertFailedToString(const char* test, const char* file, const char* function, int line, std::function<void()> endFunction);
DLL_STATE std::string GetCompilerInfo();

}

#ifdef USE_GCC
#define STRTOK_S(name, delim, context) strtok_r(name, delim, context)
#else
#define STRTOK_S(name, delim, context) strtok_s(name, delim, context)
#endif

#ifdef USE_GCC
#define ATOLL(name)    atoll(name)
#else
#define ATOLL(name)	_atoi64(name)
#endif


#ifdef USE_GCC
#define STRERROR_S(errnum, buf, size) strerror_r(errnum, buf, size)
#else
#define STRERROR_S(errnum, buf, size) strerror_s(buf, size, errnum)
#endif

// ----------------------------------------------------------------------------
// Typedefs for integers with defined sizes
// TODO: Use std::int8_t, etc instead. See C++11
// ----------------------------------------------------------------------------

#ifdef WIN32
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
typedef uint64_t u_int64_t;
#endif

typedef int8_t   int8;
typedef u_int8_t uint8;

typedef int16_t   int16;
typedef u_int16_t uint16;

typedef int32_t  int32;
typedef uint32_t uint32;

typedef int64_t   int64;
typedef u_int64_t uint64;

/**
typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
*/

// ----------------------------------------------------------------------------
// Definitions platform specific values
// ----------------------------------------------------------------------------
#ifndef USE_GCC
typedef int	socklen_t;
//#define socklen_t int
#endif

#ifdef WIN32

struct timespec {
        time_t   tv_sec;        /* seconds */
        uint64   tv_nsec;       /* nanoseconds */
};

#endif

// ----------------------------------------------------------------------------
// Defines for types used in various classes
// ----------------------------------------------------------------------------
#define DOMAINID_TYPE_NATIVE            int32
#define HANDLE_TYPE_NATIVE                int32
#define HANDLE_NIL_NATIVE                0

// ----------------------------------------------------------------------------
// Definitions for numerical limits
// ----------------------------------------------------------------------------
#define INTACT_INT8_MAX    (std::numeric_limits<int8>::max)()
#define INTACT_UINT8_MAX    (std::numeric_limits<uint8>::max)()

#define INTACT_INT16_MAX   (std::numeric_limits<int16>::max)()
#define INTACT_UINT16_MAX    (std::numeric_limits<uint16>::max)()

#define INTACT_INT32_MAX    (std::numeric_limits<int32>::max)()
#define INTACT_UINT32_MAX    (std::numeric_limits<uint32>::max)()

#define INTACT_INT64_MAX    (std::numeric_limits<int64>::max)()
#define INTACT_UINT64_MAX    (std::numeric_limits<uint64>::max)()

#define INTACT_FLOAT_MAX    (std::numeric_limits<float>::max)()
#define INTACT_DOUBLE_MAX    (std::numeric_limits<double>::max)()

#define INTACT_INT_MAX      (std::numeric_limits<int>::max)()
#define INTACT_UINT_MAX     (std::numeric_limits<unsigned int>::max)()

// ----------------------------------------------------------------------------
// Definitions for class traits
// ----------------------------------------------------------------------------

#define CLASS_TRAITS(clazzName) \
        typedef std::shared_ptr<clazzName >    Ptr; \
        typedef std::weak_ptr<clazzName >        WeakPtr;

#define FORWARD_CLASS_TRAITS(clazzName) \
        typedef std::shared_ptr<clazzName >    clazzName ## Ptr; \
        typedef std::weak_ptr<clazzName >        clazzName ## WeakPtr;

#define ENABLE_SHARED_FROM_THIS(clazzName) \
        std::enable_shared_from_this<clazzName >

#define ENABLE_SHARED_FROM_THIS_T1(clazzName, a) \
        std::enable_shared_from_this<clazzName<a> >

#define ENABLE_SHARED_FROM_THIS_T2(clazzName, a, b) \
        std::enable_shared_from_this<clazzName<a, b> >

#define ENABLE_SHARED_FROM_THIS_T3(clazzName, a, b, c) \
        std::enable_shared_from_this<clazzName<a, b, c> >

#define DYNAMIC_POINTER_CAST_T2(T, a, b, V) \
        std::dynamic_pointer_cast< T<a,b> >(V)

// ----------------------------------------------------------------------------
// Definitions for debugging
// ----------------------------------------------------------------------------
//#define INTACT_NO_DEBUG_OUTPUT
//#define INTACT_NO_WARNING_OUTPUT

#ifndef WRITE_FUNCTION
#define WRITE_FUNCTION BaseLib::WriteFunctionToString(__FUNCTION__, __LINE__)
//"[" << __FUNCTION__ << "," << __LINE__ << "]: "
#endif

#ifndef TSWRITE_FUNCTION
#define TSWRITE_FUNCTION BaseLib::WriteTimedFunctionToString(BaseLib::GetTimeStamp(), __FUNCTION__, __LINE__)
//"[" << BaseLib::GetTimeStamp() << "," << __FUNCTION__ << "," << __LINE__ << "]: "
#endif

#ifndef PRETTY_FUNCTION
#ifdef __GNUC__
#define PRETTY_FUNCTION BaseLib::WriteFunctionToString(__PRETTY_FUNCTION__, __LINE__)
//"[" << __PRETTY_FUNCTION__ << "," << __LINE__ << "]: "
#else
#define PRETTY_FUNCTION BaseLib::WriteFunctionToString(__FUNCSIG__, __LINE__)
//"[" << __FUNCSIG__ << "," << __LINE__ << "]: "
#endif
#endif

#ifndef TSPRETTY_FUNCTION
#ifdef __GNUC__
#define TSPRETTY_FUNCTION BaseLib::WriteTimedFunctionToString(BaseLib::GetTimeStamp(), __PRETTY_FUNCTION__, __LINE__)
//"[" << BaseLib::GetTimeStamp() << "," << __PRETTY_FUNCTION__ << "," << __LINE__ << "]: "
#else
#define TSPRETTY_FUNCTION BaseLib::WriteTimedFunctionToString(BaseLib::GetTimeStamp(), __FUNCSIG__, __LINE__)
//"[" << BaseLib::GetTimeStamp() << "," << __FUNCSIG__ << "," << __LINE__ << "]: "
#endif
#endif

#ifdef __GNUC__
#define MY_ASSERT(test)             ( (test) ? ((void)0) : BaseLib::WriteAssertFailedToString(#test, __FILE__, __PRETTY_FUNCTION__, __LINE__, [] () { abort();}) )
#define MY_STATIC_ASSERT(test, msg) ( (test) ? ((void)0) : BaseLib::WriteAssertFailedToString(#test, __FILE__, __PRETTY_FUNCTION__, __LINE__, [test, msg] () { static_assert(test, msg);}) )

#else
#define MY_ASSERT(test)             ( (test) ? ((void)0) : BaseLib::WriteAssertFailedToString(#test, __FILE__, __FUNCSIG__, __LINE__, [] () { abort();}) )
#define MY_STATIC_ASSERT(test, msg) ( (test) ? ((void)0) : BaseLib::WriteAssertFailedToString(#test, __FILE__, __FUNCSIG__, __LINE__, [test, msg] () { static_assert(test, msg);}) )
#endif

#define STATIC_ASSERT(test, msg)  MY_STATIC_ASSERT(test, msg)

#undef NDEBUG
#ifndef NDEBUG
#       define ASSERT(test)         MY_ASSERT(test)
#       define TYPE_NAME(type)      typeid(type).name()
#else
#       define ASSERT(test)         ((void)0)
#       define TYPE_NAME(type)      ((void)0)
//#       define INTACT_NO_DEBUG_OUTPUT
#endif

#define RPC_INTERFACE_UUID "[" << __PRETTY_FUNCTION__ << "," << __LINE__ << "]"
