/*
 * TimeBasedFilter.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_TimeBasedFilter_h_Included
#define DDS_Policy_TimeBasedFilter_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This policy allows a DataReader to indicate that it does not necessarily want to see all values of each instance published
 * under the Topic. Rather, it wants to see at most one change every minimum_separation period.
 *
 * The TIME_BASED_FILTER applies to each instance separately, that is, the constraint is that the DataReader does not
 * want to see more than one sample of each instance per minumum_separation period.
 *
 * This setting allows a DataReader to further decouple itself from the DataWriter objects. It can be used to protect
 * applications that are running on a heterogeneous network where some nodes are capable of generating data much faster
 * than others can consume it. It also accommodates the fact that for fast-changing data different subscribers may have
 * different requirements as to how frequently they need to be notified of the most current values.
 *
 * The setting of a TIME_BASED_FILTER, that is, the selection of a minimum_separation with a value greater than zero
 * is compatible with all settings of the HISTORY and RELIABILITY QoS. The TIME_BASED_FILTER specifies the
 * samples that are of interest to the DataReader. The HISTORY and RELIABILITY QoS affect the behavior of the
 * middleware with respect to the samples that have been determined to be of interest to the DataReader, that is, they apply
 * after the TIME_BASED_FILTER has been applied.
 *
 * In the case where the reliability QoS kind is RELIABLE then in steady-state, defined as the situation where the
 * DataWriter does not write new samples for a period "long" compared to the minimum_separation, the system should
 * guarantee delivery the last sample to the DataReader.
 *
 * The setting of the TIME_BASED_FILTER minimum_separation must be consistent with the DEADLINE period. For
 * these two QoS policies to be consistent they must verify that "period >= minimum_separation." An attempt to set these
 * policies in an inconsistent manner when an entity is created via a set_qos operation will cause the operation to fail.
 */
class DLL_STATE TimeBasedFilter : public QosPolicyBase
{
public:
    TimeBasedFilter()
        : minSep_(BaseLib::Duration::Zero())
    { }
    TimeBasedFilter(const BaseLib::Duration& the_min_separation)
        : minSep_(the_min_separation)
    { }

    DEFINE_POLICY_TRAITS(TimeBasedFilter, 9, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *) const
    {

    }

    virtual void Read(NetworkLib::SerializeReader *)
    {

    }

public:
    void SetMinSeparation(const BaseLib::Duration& ms) 			{ minSep_ = ms; }
    BaseLib::Duration GetMinSeparation() 				const { return minSep_; }

private:
    BaseLib::Duration minSep_;
};

}}

#endif

