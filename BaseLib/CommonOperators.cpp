#include "BaseLib/CommonOperators.h"
#include "BaseLib/CommonDefines.h"

namespace BaseLib
{

/*std::ostream& operator<<(std::ostream &ostr, const QString &s)
{
    ostr << s.toStdString();
    return ostr;
}


std::ostream& operator<<(std::ostream &ostr, const QStringList &s)
{
    for(QStringList::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
    {
        ostr << *it << CHAR_DELIMITER;
    }

    return ostr;
}*/

std::ostream& operator<<(std::ostream &ostr, const std::list<std::string> &listString)
{
    bool isFirst = true;
    for(std::list<std::string>::const_iterator it = listString.begin(), it_end = listString.end(); it != it_end; ++it)
    {
        if(isFirst)
            ostr << "(" << *it;
        else
            ostr << "," << *it;
        isFirst = false;
    }

    if(listString.empty() == false)
        ostr << ")";

    return ostr;
}

std::ostream& operator<<(std::ostream &ostr, const std::vector<std::string> &listString)
{
    bool isFirst = true;
    for(std::vector<std::string>::const_iterator it = listString.begin(), it_end = listString.end(); it != it_end; ++it)
    {
        if(isFirst)
            ostr << "(" << *it;
        else
            ostr << "," << *it;
        isFirst = false;
    }

    if(listString.empty() == false)
        ostr << ")";

    return ostr;
}


std::ostream &operator<<(std::ostream &ostr, const std::vector<unsigned char> &listString)
{
    bool isFirst = true;
    for(std::vector<unsigned char>::const_iterator it = listString.begin(), it_end = listString.end(); it != it_end; ++it)
    {
        if(isFirst)
            ostr << "(" << *it;
        else
            ostr << "," << *it;
        isFirst = false;
    }

    if(listString.empty() == false)
        ostr << ")";

    return ostr;
}

std::ostream& operator<<(std::ostream &ostr, const std::set<std::string> &listString)
{
    bool isFirst = true;
    for(std::set<std::string>::const_iterator it = listString.begin(), it_end = listString.end(); it != it_end; ++it)
    {
        std::string key;
        if(isFirst)
            ostr << "(" << key;
        else
            ostr << "," << key;
        isFirst = false;
    }

    if(listString.empty() == false)
        ostr << ")";

    return ostr;
}

std::ostream& operator<<(std::ostream &ostr, const struct timespec &ts)
{
    ostr << "(sec: " << ts.tv_sec << " nsec: " << ts.tv_nsec << ")";
    return ostr;
}

std::ostream &operator<<(std::ostream &ostr, const std::vector<long> &items)
{
    bool isFirst = true;

    for(std::vector<long>::const_iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
    {
        if(isFirst) ostr << "(" << *it;
        else ostr << "," << *it;

        isFirst = false;
    }

    if(!items.empty())
        ostr << ")";

    return ostr;
}

/*template<class T>
std::string operator+(std::string const &a, const T &b)
{
    std::ostringstream oss;
    oss << a << b;

    return oss.str();
}*/

}

