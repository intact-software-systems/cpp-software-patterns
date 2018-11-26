/*
 * Liveliness.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Liveliness_h_Included
#define DDS_Policy_Liveliness_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

class DLL_STATE Liveliness : public QosPolicyBase
{
public:
    Liveliness()
        : kind_(Policy::LivelinessKind::AUTOMATIC)
        , leaseDuration_(BaseLib::Duration::Infinite())
    { }

    Liveliness(Policy::LivelinessKind::Type kind, const BaseLib::Duration &leaseDuration)
        : kind_(kind)
        , leaseDuration_(leaseDuration)
    { }

    DEFINE_POLICY_TRAITS(Liveliness, 8, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32((int32_t) kind_);
        // TODO: Write Duration
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::LivelinessKind::Type) reader->ReadInt32();
        // TODO: Read Duration
    }

public:
    void SetKind(Policy::LivelinessKind::Type kind) 				{ kind_ = kind; }
    void SetLeaseDuration(const BaseLib::Duration& leaseDuration) 	{ leaseDuration_ = leaseDuration; }

    Policy::LivelinessKind::Type 	GetKind() 			const 	{ return kind_; }
    BaseLib::Duration 				GetLeaseDuration() 	const 	{ return leaseDuration_; }

public:
    static Liveliness Automatic()
    {
        return Liveliness(Policy::LivelinessKind::AUTOMATIC, BaseLib::Duration::Infinite());
    }

    static Liveliness ManualByParticipant(const BaseLib::Duration& lease = BaseLib::Duration::Infinite())
    {
        return Liveliness(Policy::LivelinessKind::MANUAL_BY_PARTICIPANT, lease);
    }

    static Liveliness ManualByTopic(const BaseLib::Duration& lease = BaseLib::Duration::Infinite())
    {
        return Liveliness(Policy::LivelinessKind::MANUAL_BY_TOPIC, lease);
    }

private:
    Policy::LivelinessKind::Type    kind_;
    BaseLib::Duration     			leaseDuration_;
};

}}

#endif
