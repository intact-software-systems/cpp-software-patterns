/*
 * Reliability.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_Reliability_h_Included
#define DDS_Policy_Reliability_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{
/**
 * @abstract
 *
 * This policy indicates the level of reliability requested by a DataReader or offered by a DataWriter. These levels are
 * ordered, BEST_EFFORT being lower than RELIABLE. A DataWriter offering a level is implicitly offering all levels
 * below.
 *
 * The setting of this policy has a dependency on the setting of the RESOURCE_LIMITS policy. In case the RELIABILITY
 * kind is set to RELIABLE the write operation on the DataWriter may block if the modification would cause data to be lost
 * or else cause one of the limits specified in the RESOURCE_LIMITS to be exceeded. Under these circumstances, the
 * RELIABILITY max_blocking_time configures the maximum duration the write operation may block.
 * If the RELIABILITY kind is set to RELIABLE, data-samples originating from a single DataWriter cannot be made
 * available to the DataReader if there are previous data-samples that have not been received yet due to a communication
 * error. In other words, the service will repair the error and retransmit data-samples as needed in order to reconstruct a
 * correct snapshot of the DataWriter history before it is accessible by the DataReader.
 *
 * If the RELIABILITY kind is set to BEST_EFFORT, the service will not retransmit missing data-samples. However for
 * data-samples originating from any one DataWriter the service will ensure they are stored in the DataReader history in the
 * same order they originated in the DataWriter. In other words, the DataReader may miss some data-samples but it will
 * never see the value of a data-object change from a newer value to an order value.
 *
 * The value offered is considered compatible with the value requested if and only if the inequality "offered kind >=
 * requested kind" evaluates to 'TRUE.' For the purposes of this inequality, the values of RELIABILITY kind are
 * considered ordered such that BEST_EFFORT < RELIABLE.
 */
class DLL_STATE Reliability : public QosPolicyBase
{
public:
    Reliability()
        : kind_(Policy::ReliabilityKind::BEST_EFFORT)
        , maxBlockingTime_(BaseLib::Duration::Zero())
    { }

    Reliability(Policy::ReliabilityKind::Type the_kind, const BaseLib::Duration& the_max_blocking_time)
        : kind_(the_kind)
        , maxBlockingTime_(the_max_blocking_time)
    { }

    DEFINE_POLICY_TRAITS(Reliability, 11, DDS::Policy::RequestedOfferedKind::COMPATIBLE, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *) const
    {

    }

    virtual void Read(NetworkLib::SerializeReader *)
    {

    }

public:
    void SetKind(Policy::ReliabilityKind::Type the_kind) 	{ kind_ = the_kind; }
    void SetMaxBlokingTime(const BaseLib::Duration& d) 		{ maxBlockingTime_ = d; }

    Policy::ReliabilityKind::Type GetKind() 	const { return kind_; }
    const BaseLib::Duration GetMaxBlockingTime() const { return maxBlockingTime_; }

public:
    static Reliability Reliable(const BaseLib::Duration& d = BaseLib::Duration::Infinite())
    {
        return Reliability(Policy::ReliabilityKind::RELIABLE, d);
    }

    static Reliability BestEffort()
    {
        return Reliability(Policy::ReliabilityKind::BEST_EFFORT, BaseLib::Duration::Zero());
    }

private:
    Policy::ReliabilityKind::Type 		kind_;
    BaseLib::Duration    				maxBlockingTime_;
};

}}

#endif
