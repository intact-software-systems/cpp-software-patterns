#pragma once

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE ReactionKind : char
{
    ESCALATE = 'E',
    RESTART  = 'R',
    RESUME   = 'C',
    STOP     = 'H'
};

class DLL_STATE Reaction
    : public Templates::ComparableImmutableType<ReactionKind>
{
public:
    Reaction(ReactionKind kind)
        : Templates::ComparableImmutableType<ReactionKind>(kind)
    { }

    virtual ~Reaction()
    { }

    ReactionKind Kind() const
    {
        return this->Clone();
    }


    bool IsEscalate() const
    {
        return this->Clone() == ReactionKind::ESCALATE;
    }

    bool IsRestart() const
    {
        return this->Clone() == ReactionKind::RESTART;
    }

    bool IsResume() const
    {
        return this->Clone() == ReactionKind::RESUME;
    }

    bool IsStop() const
    {
        return this->Clone() == ReactionKind::STOP;
    }


    static Reaction Escalate()
    {
        return Reaction(ReactionKind::ESCALATE);
    }

    static Reaction Restart()
    {
        return Reaction(ReactionKind::RESTART);
    }

    static Reaction Resume()
    {
        return Reaction(ReactionKind::RESUME);
    }

    static Reaction Stop()
    {
        return Reaction(ReactionKind::STOP);
    }


    friend std::ostream& operator<<(std::ostream& ostr, const Reaction& t)
    {
        ostr << (char) t.delegate();
        return ostr;
    }
};

}}
