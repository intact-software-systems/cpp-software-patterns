#pragma once

#include"BaseLib/Duration.h"
#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy {

enum class DLL_STATE LivelinessKind
{
    AUTOMATIC,
    MANUAL_BY_PARTICIPANT,
    MANUAL_BY_TOPIC
};

class DLL_STATE Liveliness
{
public:
    Liveliness()
        : kind_(Policy::LivelinessKind::AUTOMATIC)
        , leaseDuration_(BaseLib::Duration::Infinite())
    { }

    Liveliness(Policy::LivelinessKind kind, BaseLib::Duration leaseDuration)
        : kind_(kind)
        , leaseDuration_(leaseDuration)
    { }

    Policy::LivelinessKind GetKind() const
    {
        return kind_;
    }

    BaseLib::Duration GetLeaseDuration() const
    {
        return leaseDuration_;
    }

    static Liveliness Automatic()
    {
        return Liveliness(Policy::LivelinessKind::AUTOMATIC, BaseLib::Duration::Infinite());
    }

    static Liveliness ManualByParticipant(BaseLib::Duration lease = BaseLib::Duration::Infinite())
    {
        return Liveliness(Policy::LivelinessKind::MANUAL_BY_PARTICIPANT, lease);
    }

    static Liveliness ManualByTopic(BaseLib::Duration lease = BaseLib::Duration::Infinite())
    {
        return Liveliness(Policy::LivelinessKind::MANUAL_BY_TOPIC, lease);
    }

private:
    Policy::LivelinessKind kind_;
    BaseLib::Duration      leaseDuration_;
};

}}
