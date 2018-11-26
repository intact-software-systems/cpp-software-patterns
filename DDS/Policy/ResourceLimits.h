/*
 * ResourceLimits.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_ResourceLimits_h_Included
#define DDS_Policy_ResourceLimits_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Policy/PolicyKind.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @abstract
 *
 * This policy controls the resources that the Service can use in order to meet the requirements imposed by the application
 * and other QoS settings.
 *
 * If the DataWriter objects are communicating samples faster than they are ultimately taken by the DataReader objects, the
 * middleware will eventually hit against some of the QoS-imposed resource limits. Note that this may occur when just a
 * single DataReader cannot keep up with its corresponding DataWriter. The behavior in this case depends on the setting for
 * the RELIABILITY QoS. If reliability is BEST_EFFORT, then the Service is allowed to drop samples. If the reliability is
 * RELIABLE, the Service will block the DataWriter or discard the sample at the DataReader 28in order not to lose existing
 * samples.
 *
 * The constant LENGTH_UNLIMITED may be used to indicate the absence of a particular limit. For example setting
 * max_samples_per_instance to LENGH_UNLIMITED will cause the middleware to not enforce this particular limit.
 *
 * The setting of RESOURCE_LIMITS max_samples must be consistent with the max_samples_per_instance. For these
 * two values to be consistent they must verify that "max_samples >= max_samples_per_instance."
 *
 * The setting of RESOURCE_LIMITS max_samples_per_instance must be consistent with the HISTORY depth. For these
 * two QoS to be consistent, they must verify that "depth <= max_samples_per_instance."
 *
 * An attempt to set this policy to inconsistent values when an entity is created via a set_qos operation will cause the
 * operation to fail.
 */
class DLL_STATE ResourceLimits : public QosPolicyBase
{
public:
    ResourceLimits()
        : maxSamples_(Core::LENGTH_UNLIMITED)
        , maxInstances_(Core::LENGTH_UNLIMITED)
        , maxSamplesPerInstance_(Core::LENGTH_UNLIMITED)
    { }

    ResourceLimits(int32_t the_max_samples,
                   int32_t the_max_instances,
                   int32_t the_max_samples_per_instance)
        : maxSamples_(the_max_samples)
        , maxInstances_(the_max_instances)
        , maxSamplesPerInstance_(the_max_samples_per_instance)
    { }

    DEFINE_POLICY_TRAITS(ResourceLimits, 14, DDS::Policy::RequestedOfferedKind::INDEPENDENT, false)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt32(maxSamples_);
        writer->WriteInt32(maxInstances_);
        writer->WriteInt32(maxSamplesPerInstance_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        maxSamples_ = reader->ReadInt32();
        maxInstances_ = reader->ReadInt32();
        maxSamplesPerInstance_ = reader->ReadInt32();
    }

public:
    static ResourceLimits Unlimited()
    {
        return ResourceLimits();
    }


public:
    void SetMaxSamples(int32_t samples) 								{ maxSamples_ = samples; }
    void SetMaxInstances(int32_t the_max_instances) 					{ maxInstances_ = the_max_instances; }
    void SetMaxSamplesPerInstance(int32_t the_max_samples_per_instance) { maxSamplesPerInstance_ = the_max_samples_per_instance; }

    int32_t GetMaxSamples() 			const { return maxSamples_; }
    int32_t GetMaxInstances() 			const { return maxInstances_; }
    int32_t GetMaxSamplesPerInstance() 	const { return maxSamplesPerInstance_; }

private:
    int32_t maxSamples_;
    int32_t maxInstances_;
    int32_t maxSamplesPerInstance_;
};

}}

#endif
