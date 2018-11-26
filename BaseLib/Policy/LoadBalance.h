#ifndef BaseLib_Policy_LoadBalance_h_IsIncluded
#define BaseLib_Policy_LoadBalance_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

/**
 * @brief The LoadPolicyKind
 */
namespace LoadBalanceKind
{
    enum Type
    {
        ROUND_ROBIN,
        FILL_UP_FIRST
    };
}

/**
 * @brief The QosLoadPolicy class
 */
class DLL_STATE LoadBalance
        : public Templates::ComparableImmutableType<LoadBalanceKind::Type>
{
public:
    LoadBalance(LoadBalanceKind::Type kind)
        : Templates::ComparableImmutableType<LoadBalanceKind::Type>(kind)
    {}
    ~LoadBalance()
    {}

    LoadBalanceKind::Type Kind() const
    {
        return this->Clone();
    }

    bool operator==(const LoadBalance &policy) const
    {
        return this->delegate() == policy.delegate();
    }

    bool operator!=(const LoadBalance &policy) const
    {
        return this->delegate() != policy.delegate();
    }
};

}}

#endif
