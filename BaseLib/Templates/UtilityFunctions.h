#ifndef BaseLib_Templates_UtilityFunctions_h_Included
#define BaseLib_Templates_UtilityFunctions_h_Included

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <limits>

#include"BaseLib/Export.h"
namespace BaseLib
{

template<typename T>
DLL_STATE inline std::string Stringify(T const& x)
{
  std::ostringstream out;
  out << x;
  return out.str();
}

template<>
DLL_STATE inline std::string Stringify<bool>(bool const& x)
{
  std::ostringstream out;
  out << std::boolalpha << x;
  return out.str();
}

template<>
DLL_STATE inline std::string Stringify<double>(double const& x)
{
  const int sigdigits = std::numeric_limits<double>::digits10;
  // or perhaps std::numeric_limits<double>::max_digits10 if that is available on your compiler
  std::ostringstream out;
  out << std::setprecision(sigdigits) << x;
  return out.str();
}

template<>
DLL_STATE inline std::string Stringify<float>(float const& x)
{
  const int sigdigits = std::numeric_limits<float>::digits10;
  // or perhaps std::numeric_limits<float>::max_digits10 if that is available on your compiler
  std::ostringstream out;
  out << std::setprecision(sigdigits) << x;
  return out.str();
}

template<>
DLL_STATE inline std::string Stringify<long double>(const long double& x)
{
  const int sigdigits = std::numeric_limits<long double>::digits10;
  // or perhaps std::numeric_limits<long_double>::max_digits10 if that is available on your compiler
  std::ostringstream out;
  out << std::setprecision(sigdigits) << x;
  return out.str();
}

//template<>
//DLL_STATE inline std::string Stringify<wchar_t>(const wchar_t &buf)
//{
//}

} // namespace BaseLib

#endif // BaseLib_Templates_UtilityFunctions_h_Included
