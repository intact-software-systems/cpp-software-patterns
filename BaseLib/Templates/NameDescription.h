#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Templates/AnyKeyType.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Templates {

class DLL_STATE NameDescription
    : public Templates::ComparableImmutableType<Templates::AnyKey>
{
public:
    NameDescription();
    NameDescription(std::string name);
    NameDescription(std::string name, std::string domain);

    virtual ~NameDescription();

    std::string Name() const;
    std::string Domain() const;


    static NameDescription Default()
    {
        static NameDescription def("app", "local");
        return def;
    }

    friend std::ostream& operator<<(std::ostream& ostr, const NameDescription& item)
    {
        if(!item.domainId_.empty())
        {
            ostr << item.domainId_ << ".";
        }

        ostr << item.name_;
        return ostr;
    }

private:
    std::string name_;
    std::string domainId_;
};

}}
