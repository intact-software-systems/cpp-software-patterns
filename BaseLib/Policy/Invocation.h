#ifndef BaseLib_Policy_Invocation_h_IsIncluded
#define BaseLib_Policy_Invocation_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * @brief The InvocationKind
 */
namespace InvocationKind
{
    enum Type
    {
        SYNC,
        ASYNC
    };
}

/**
 * @brief The Invocation class
 */
class DLL_STATE Invocation
        : public Templates::ComparableImmutableType<InvocationKind::Type>
{
public:
    Invocation(InvocationKind::Type kind = InvocationKind::SYNC)
        : Templates::ComparableImmutableType<InvocationKind::Type>(kind)
    {}
    virtual ~Invocation()
    {}

    InvocationKind::Type Kind() const
    {
        return this->Clone();
    }

    bool IsSync() const
    {
        return this->delegate() == InvocationKind::SYNC;
    }

    bool IsAsync() const
    {
        return this->delegate() == InvocationKind::ASYNC;
    }

    // --------------------------------------------
    // Static constructors
    // --------------------------------------------

    static Invocation Sync()
    {
        return Invocation(InvocationKind::SYNC);
    }

    static Invocation Async()
    {
        return Invocation(InvocationKind::ASYNC);
    }
};

}}

#endif
