/*
 * DurabilityService.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_DurabilityService_h_Included
#define DDS_Policy_DurabilityService_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @brief
 *
 * This policy is used to configure the HISTORY QoS and the RESOURCE_LIMITS QoS used by the fictitious DataReader
 * and DataWriter used by the "persistence service". The "persistence service" is the one responsible for implementing the
 * DURABILITY kinds TRANSIENT and PERSISTENCE. See Section 7.1.3.4, "DURABILITY", on page 109.
 */
class DLL_STATE DurabilityService {
public:
    DurabilityService() :
        cleanupDelay_(BaseLib::Duration::Infinite()),
        historyKind_(Policy::HistoryKind::KEEP_LAST),
        historyDepth_(1),
        maxSamples_(Core::LENGTH_UNLIMITED),
        maxInstances_(Core::LENGTH_UNLIMITED),
        maxSamplesPerInstance_(Core::LENGTH_UNLIMITED)
    { }

    DurabilityService(const BaseLib::Duration& the_service_cleanup_delay,
                      Policy::HistoryKind::Type the_history_kind,
                      int32_t the_history_depth,
                      int32_t the_max_samples,
                      int32_t the_max_instances,
                      int32_t the_max_samples_per_instance)
    : cleanupDelay_(the_service_cleanup_delay),
      historyKind_(the_history_kind),
      historyDepth_(the_history_depth),
      maxSamples_(the_max_samples),
      maxInstances_(the_max_instances),
      maxSamplesPerInstance_(the_max_samples_per_instance)
    { }

    DEFINE_POLICY_TRAITS(DurabilityService, 22, DDS::Policy::RequestedOfferedKind::INDEPENDENT, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        // TODO: Write Duration
        writer->WriteInt32((int32_t) historyKind_);
        writer->WriteInt32(historyDepth_);
        writer->WriteInt32(maxSamples_);
        writer->WriteInt32(maxInstances_);
        writer->WriteInt32(maxSamplesPerInstance_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        // TODO: Read duration
        historyKind_ = (Policy::HistoryKind::Type) reader->ReadInt32();
        historyDepth_ = reader->ReadInt32();
        maxSamples_ = reader->ReadInt32();
        maxInstances_ = reader->ReadInt32();
        maxSamplesPerInstance_ = reader->ReadInt32();
    }

public:
    BaseLib::Duration 			GetCleanupDelay() 			{ return cleanupDelay_; }
    Policy::HistoryKind::Type 	GetHistoryKind()			{ return historyKind_; }
    int32_t						GetHistoryDepth()			{ return historyDepth_; }
    int32_t 					GetMaxSamples()				{ return maxSamples_; }
    int32_t						GetMaxInstances()			{ return maxInstances_; }
    int32_t						GetMaxSamplesPerInstance()	{ return maxSamplesPerInstance_; }

private:
    BaseLib::Duration 			cleanupDelay_;
    Policy::HistoryKind::Type 	historyKind_;

    int32_t historyDepth_;
    int32_t maxSamples_;
    int32_t maxInstances_;
    int32_t maxSamplesPerInstance_;
};

}} //  DDS::Policy

#endif // DDS_Policy_DurabilityService_h_Included
