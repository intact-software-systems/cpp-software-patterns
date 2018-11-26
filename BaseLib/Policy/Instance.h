#ifndef BaseLib_Policy_Instance_h_IsIncluded
#define BaseLib_Policy_Instance_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

namespace InstanceKind
{
    enum class DLL_STATE Type : char
    {
        SINGLETON,
        INSTANCES
    };
}

class DLL_STATE Instance : public Templates::ComparableImmutableType<InstanceKind::Type>
{
public:
    Instance(InstanceKind::Type type)
        : Templates::ComparableImmutableType<InstanceKind::Type>(type)
    { }
    ~Instance()
    { }

    InstanceKind::Type Kind()
    {
        return this->Clone();
    }

    static Instance Singleton()
    {
        return Instance(InstanceKind::Type::SINGLETON);
    }

    static Instance Instances()
    {
        return Instance(InstanceKind::Type::INSTANCES);
    }
};

}}

#endif
