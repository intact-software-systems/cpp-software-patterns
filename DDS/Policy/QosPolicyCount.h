#ifndef DDS_Policy_QosPolicyCount_h_Included
#define DDS_Policy_QosPolicyCount_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/CorePolicyFwd.h"
#include"DDS/Export.h"
namespace DDS { namespace Policy
{

class DLL_STATE QosPolicyCount
{
public:
    // @TODO: Finish Implementation
    QosPolicyCount(DDS::Policy::QosPolicyId id, uint32_t count)
        : policyId_(id)
        , count_(count)
    { }

    QosPolicyCount(const QosPolicyCount& other)
        : policyId_(other.GetPolicyId())
        , count_(other.GetCount())
    { }

public:
    DDS::Policy::QosPolicyId GetPolicyId() const
    {
        return policyId_;
    }

    void SetPolicyId(DDS::Policy::QosPolicyId id)
    {
        policyId_ = id;
    }

    uint32_t GetCount() const
    {
        return count_;
    }

    void SetCount(uint32_t c)
    {
        count_ = c;
    }

private:
    DDS::Policy::QosPolicyId policyId_;
    uint32_t count_;
};

}}

#endif
