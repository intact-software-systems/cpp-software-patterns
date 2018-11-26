#ifndef BaseLib_Typename_h_IsIncluded
#define BaseLib_Typename_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"

#include"BaseLib/Export.h"

namespace BaseLib
{

class Typename
        : public Templates::ComparableImmutableType<std::string>
{
public:
    Typename(std::string name);
    virtual ~Typename();

    std::string Name() const;

    static Typename Create(std::string name)
    {
        return Typename(name);
    }

    friend std::ostream& operator<<(std::ostream &ostr, const Typename &name)
    {
        ostr << name.delegate();
        return ostr;
    }

};

}

#endif
