#ifndef BaseLib_Policy_Ordering_h_IsIncluded
#define BaseLib_Policy_Ordering_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

/**
 * @brief The OrderingKind
 */
namespace OrderingKind
{
    enum Type
    {
        RANDOM,
        FIFO,
        LIFO,
        WFQ
    };
}

/**
 * @brief The Ordering class
 */
class DLL_STATE Ordering : public Templates::ComparableImmutableType<OrderingKind::Type>
{
public:
    Ordering(OrderingKind::Type kind = OrderingKind::FIFO)
        : Templates::ComparableImmutableType<OrderingKind::Type>(kind)
    {}
    virtual ~Ordering()
    {}

    OrderingKind::Type Kind() const
    {
        return this->Clone();
    }

    bool IsFIFO() const
    {
        return this->delegate() == OrderingKind::FIFO;
    }

    bool IsRandom() const
    {
        return this->delegate() == OrderingKind::RANDOM;
    }

    bool IsWFQ() const
    {
        return this->delegate() == OrderingKind::WFQ;
    }

    static Ordering Fifo()
    {
        return Ordering(OrderingKind::FIFO);
    }

    static Ordering Random()
    {
        return Ordering(OrderingKind::RANDOM);
    }

    static Ordering Lifo()
    {
        return Ordering(OrderingKind::LIFO);
    }
};

}}

#endif
