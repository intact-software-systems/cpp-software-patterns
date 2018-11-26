#ifndef RTPS_DCPS_Pub_CacheWriter_h_IsIncluded
#define RTPS_DCPS_Pub_CacheWriter_h_IsIncluded

#include"RTPS/CommonDefines.h"
#include"RTPS/DCPS/Interfaces/CacheWriter.h"
#include"RTPS/Machine/VirtualMachineWriter.h"
#include"RTPS/Cache/IncludeLibs.h"
#include"RTPS/Structure/RtpsWriter.h"

#include"RTPS/Export.h"

namespace DCPS { namespace Publication
{

/**
* @brief The DDS writer communicates with RTPS using these functions
*
* TODO: Maybe rename to CacheWriterToRTPS, CacheWriterRtps
* TODO: The RTPS should use RxData cache approaches which enables the VirtualMachine to act on incoming and outgoing data.
*
* @author KVik
*/
class DLL_STATE CacheWriter
        : public DCPS::Interfaces::CacheWriter
        , public BaseLib::Templates::KeyValueObjectObserver<BaseLib::InstanceHandle, NetworkLib::SerializeWriter::Ptr>
{
public:
    CacheWriter();
    CacheWriter(const RTPS::VirtualMachineWriter::Ptr &virtualMachineRtpsWriter);
    virtual ~CacheWriter();


    virtual void OnObjectCreated(InstanceHandle key, SerializeWriter::Ptr value);

    virtual void OnObjectDeleted(InstanceHandle key);

    virtual void OnObjectModified(InstanceHandle key, SerializeWriter::Ptr value);


    /*------------------------------------------------------------
    If using RTPS then add message.Data to RTPSWriter.dataCache:

    The transition performs the following logical actions in the virtual machine:

    the_rtps_writer := the_dds_writer.related_rtps_writer;
    a_change := the_rtps_writer.new_change(ALIVE, data, handle);
    the_rtps_writer.writer_cache.add_change(a_change);

    After the transition the following post-conditions hold:

    ASSERT(the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber)

    IF NOT using RTPS then
    --> other simpler implementations?
    ------------------------------------------------------------*/
    virtual bool Write(const NetworkLib::SerializeWriter::Ptr &buffer,
                       const InstanceHandle &handle,
                       BaseLib::Timestamp timestamp = BaseLib::Timestamp());

    /*------------------------------------------------------------
    This transition is triggered by the act of disposing a data-object previously written with the DDS DataWriter
    "the_dds_writer". The DataWriter::dispose() operation takes as parameter the InstanceHandle_t "handle" used to
    differentiate among different data-objects.

    This operation has no effect if the topicKind==NO_KEY.
    The transition performs the following logical actions in the virtual machine:

    the_rtps_writer := the_dds_writer.related_rtps_writer;
    if (the_rtps_writer.topicKind == WITH_KEY) {
        a_change := the_rtps_writer.new_change(NOT_ALIVE_DISPOSED, <nil>, handle);
        the_rtps_writer.writer_cache.add_change(a_change);
    }

    After the transition the following post-conditions hold:
    if (the_rtps_writer.topicKind == WITH_KEY) then
        the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber
    ------------------------------------------------------------*/
    virtual bool Dispose(const InstanceHandle &handle);

    /*------------------------------------------------------------
    This transition is triggered by the act of unregistering a data-object previously written with the DDS DataWriter
    "the_dds_writer". The DataWriter::unregister() operation takes as arguments the InstanceHandle_t "handle" used to
    differentiate among different data-objects.
    This operation has no effect if the topicKind==NO_KEY.
    The transition performs the following logical actions in the virtual machine:

    the_rtps_writer := the_dds_writer.related_rtps_writer;

    if (the_rtps_writer.topicKind == WITH_KEY) {
        a_change := the_rtps_writer.new_change(NOT_ALIVE_UNREGISTERED, <nil>, handle);
        the_rtps_writer.writer_cache.add_change(a_change);
    }

    After the transition the following post-conditions hold:

    if (the_rtps_writer.topicKind == WITH_KEY) then
        the_rtps_writer.writer_cache.get_seq_num_max() == a_change.sequenceNumber
    ------------------------------------------------------------*/
    virtual bool Unregister(const InstanceHandle &handle);

private:
    RTPS::VirtualMachineWriter::Ptr virtualMachineRtpsWriter_;
};

}}

#endif // RTPS_DCPS_Pub_CacheWriter_h_IsIncluded
