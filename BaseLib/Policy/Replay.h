#ifndef BaseLib_Policy_Replay_h_IsIncluded
#define BaseLib_Policy_Replay_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

namespace ReplayKind
{
    enum Type
    {
        No,
        OnSubscribe,
        OnUnsubscribe,
        OnComplete,
        OnTrigger,
    };
}


class DLL_STATE Replay
        : public Templates::ComparableImmutableType<ReplayKind::Type>
{
public:
    Replay(ReplayKind::Type kind = ReplayKind::OnSubscribe)
        : Templates::ComparableImmutableType<ReplayKind::Type>(kind)
    {}
    virtual ~Replay()
    {}

    ReplayKind::Type Kind() const
    {
        return this->delegate();
    }

    bool IsNo() const
    {
        return this->delegate() == ReplayKind::No;
    }

    bool IsOnSubscribe() const
    {
        return this->delegate() == ReplayKind::OnSubscribe;
    }

    bool IsOnUnsubscribe() const
    {
        return this->delegate() == ReplayKind::OnUnsubscribe;
    }

    bool IsOnComplete() const
    {
        return this->delegate() == ReplayKind::OnComplete;
    }

    bool IsOnTrigger() const
    {
        return this->delegate() == ReplayKind::OnTrigger;
    }

    // ------------------------------------------
    // factory functions
    // ------------------------------------------

    static Replay No()
    {
        static Replay obj(ReplayKind::No);
        return obj;
    }

    static Replay OnSubscribe()
    {
        static Replay obj(ReplayKind::OnSubscribe);
        return obj;
    }

    static Replay OnUnsubscribe()
    {
        static Replay obj(ReplayKind::OnUnsubscribe);
        return obj;
    }

    static Replay OnComplete()
    {
        static Replay obj(ReplayKind::OnComplete);
        return obj;
    }

    static Replay OnTrigger()
    {
        static Replay obj(ReplayKind::OnTrigger);
        return obj;
    }
};

}}

#endif
