#ifndef RTPS_DCPS_Interfaces_CacheWriter_h_IsIncluded
#define RTPS_DCPS_Interfaces_CacheWriter_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Export.h"

namespace DCPS { namespace Interfaces
{

/**
* @brief The DDS writer communicates with RTPS using these functions
*
* @author KVik
*/
class DLL_STATE CacheWriter
{
public:
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
    virtual bool Write(const SerializeWriter::Ptr &buffer, const InstanceHandle &handle, BaseLib::Timestamp timestamp) = 0;

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
    virtual bool Dispose(const InstanceHandle &handle) = 0;

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
    virtual bool Unregister(const InstanceHandle &handle) = 0;
};

}}

#endif // RTPS_DCPS_Interfaces_CacheWriter_h_IsIncluded
