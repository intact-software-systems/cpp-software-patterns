/*
 * TypeConsistencyEnforcement.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_TypeConsistencyEnforcement_h_Included
#define DDS_Policy_TypeConsistencyEnforcement_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief The TypeConsistencyEnforcement class
 */
class DLL_STATE TypeConsistencyEnforcement : public QosPolicyBase
{
public:
    TypeConsistencyEnforcement()
        : kind_(Policy::TypeConsistencyEnforcementKind::EXACT_TYPE_TYPE_CONSISTENCY)
    {}
    TypeConsistencyEnforcement(Policy::TypeConsistencyEnforcementKind::Type kind)
        : kind_(kind)
    {}

    virtual ~TypeConsistencyEnforcement()
    {}

    DEFINE_POLICY_TRAITS(TypeConsistencyEnforcement, 24, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt8((const char)kind_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        kind_ = (Policy::TypeConsistencyEnforcementKind::Type) reader->ReadInt8();
    }

public:
    void SetKind(Policy::TypeConsistencyEnforcementKind::Type kind)
    {
        kind_ = kind;
    }

    Policy::TypeConsistencyEnforcementKind::Type  GetKind() const
    {
        return kind_;
    }

private:
    Policy::TypeConsistencyEnforcementKind::Type kind_;
};

}}

#endif

