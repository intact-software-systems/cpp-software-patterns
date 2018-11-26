#pragma once

#include"RxObserver/IncludeExtLibs.h"
#include"RxObserver/Export.h"

namespace BaseLib { namespace Concurrent
{

class DLL_STATE SubjectDescription
        : public Templates::ComparableImmutableType<Templates::AnyKey>
{
public:
    SubjectDescription(std::string name = "", std::string domain = "1");
    virtual ~SubjectDescription();

    std::string Name() const;
    std::string Domain() const;


    static SubjectDescription Default()
    {
        static SubjectDescription def("app", "local");
        return def;
    }

private:
    std::string name_;
    std::string domainId_;
};

}}
