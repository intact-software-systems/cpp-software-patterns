#ifndef BaseLib_CommonOperators_h_Included
#define BaseLib_CommonOperators_h_Included

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Export.h"

namespace BaseLib
{
    //DLL_STATE std::ostream& operator<<(std::ostream &ostr, const QString &s);
    //DLL_STATE std::ostream& operator<<(std::ostream &ostr, const QStringList &s);

    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const std::list<std::string> &listString);
    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const std::vector<std::string> &listString);
    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const std::vector<unsigned char> &listString);
    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const std::set<std::string> &listString);

    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const struct timespec &ts);

    DLL_STATE std::ostream& operator<<(std::ostream &ostr, const std::vector<long> &items);

    //DLL_STATE std::ostream& operator+(std::string &str, const int &a);
    //DLL_STATE template<class T> std::string operator+(std::string const &a, const T &b);
}

#endif

