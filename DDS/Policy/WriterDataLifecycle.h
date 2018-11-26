/*
 * WriterDataLifecycle.h
 *
 *  Created on: Sep 9, 2012
 *      Author: knuthelv
 */

#ifndef DDS_Policy_WriterDataLifecycle_h_Included
#define DDS_Policy_WriterDataLifecycle_h_Included

#include"DDS/CommonDefines.h"
#include"DDS/Policy/QosPolicyBase.h"
#include"DDS/Export.h"

namespace DDS { namespace Policy
{

/**
 * @abstract
 *
 * This policy controls the behavior of the DataWriter with regards to the lifecycle of the data-instances it manages, that is,
 * the data-instances that have been either explicitly registered with the DataWriter using the register operations (see
 * Section 7.1.2.4.2.5 and Section 7.1.2.4.2.6) or implicitly by directly writing the data (see Section 7.1.2.4.2.11 and Section
 * 7.1.2.4.2.12).
 *
 * The autodispose_unregistered_instances flag controls the behavior when the DataWriter unregisters an instance by
 * means of the unregister operations (see Section 7.1.2.4.2.7, "unregister_instance" and Section 7.1.2.4.2.8,
 * "unregister_instance_w_timestamp"):
 *
 * • The setting 'autodispose_unregistered_instances = TRUE' causes the DataWriter to dispose the instance each time it
 * 	   is unregistered. The behavior is identical to explicitly calling one of the dispose operations (Section 7.1.2.4.2.13,
 * 	  "dispose" and Section 7.1.2.4.2.14, "dispose_w_timestamp") on the instance prior to calling the unregister operation.
 * • The setting 'autodispose_unregistered_instances = FALSE' will not cause this automatic disposition upon
 * 	  unregistering. The application can still call one of the dispose operations prior to unregistering the instance and
 * 	  accomplish the same effect. Refer to Section 7.1.3.23.3, "Semantic difference between unregister_instance and
 * 	  dispose” for a description of the consequences of disposing and unregistering instances.
 *
 * Note that the deletion of a DataWriter automatically unregisters all data-instances it manages (Section 7.1.2.4.1.6,
 * "delete_datawriter"). Therefore the setting of the autodispose_unregistered_instances flag will determine whether
 * instances are ultimately disposed when the DataWriter is deleted either directly by means of the
 * Publisher::delete_datawriter operation or indirectly as a consequence of calling delete_contained_entities on the
 * Publisher or the DomainParticipant that contains the DataWriter.
 */
class DLL_STATE WriterDataLifecycle : public QosPolicyBase
{
public:
    WriterDataLifecycle(bool the_autodispose = true)
        : autodispose_(the_autodispose)
    { }

    DEFINE_POLICY_TRAITS(WriterDataLifecycle, 16, DDS::Policy::RequestedOfferedKind::NOT_APPLICABLE, true)

public:
    virtual void Write(NetworkLib::SerializeWriter *writer) const
    {
        writer->WriteInt8(autodispose_);
    }

    virtual void Read(NetworkLib::SerializeReader *reader)
    {
        autodispose_ = reader->ReadInt8();
    }

public:
    void SetAutodispose(bool b) 		{ autodispose_ = b; }
    bool GetAutodispose() 		const 	{ return autodispose_; }

public:
    static WriterDataLifecycle AutoDisposeUnregisteredInstances()
    {
        return WriterDataLifecycle(true);
    }

    static WriterDataLifecycle ManuallyDisposeUnregisteredInstances()
    {
        return WriterDataLifecycle(false);
    }

private:
    bool autodispose_;
};

}}

#endif
