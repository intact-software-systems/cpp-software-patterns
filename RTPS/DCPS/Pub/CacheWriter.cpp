#include"RTPS/DCPS/Pub/CacheWriter.h"

namespace DCPS { namespace Publication
{

/**
 * @brief CacheWriter::CacheWriter
 */
CacheWriter::CacheWriter()
    : virtualMachineRtpsWriter_()
{}

/**
 * @brief CacheWriter::CacheWriter
 * @param virtualMachineRtpsWriter
 */
CacheWriter::CacheWriter(const RTPS::VirtualMachineWriter::Ptr &virtualMachineRtpsWriter)
    : virtualMachineRtpsWriter_(virtualMachineRtpsWriter)
{}

CacheWriter::~CacheWriter()
{}

void CacheWriter::OnObjectCreated(InstanceHandle key, SerializeWriter::Ptr value)
{
}

void CacheWriter::OnObjectDeleted(InstanceHandle key)
{
}

void CacheWriter::OnObjectModified(InstanceHandle key, SerializeWriter::Ptr value)
{
}

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

TODO: Serializing data here is wrong abstraction. It should be serialized prior to calling this writer.
      Instead, use a byte array that holds the serialized raw data.
------------------------------------------------------------*/
bool CacheWriter::Write(const NetworkLib::SerializeWriter::Ptr &buffer, const InstanceHandle &handle, BaseLib::Timestamp timestamp)
{
    ASSERT(virtualMachineRtpsWriter_);
    ASSERT(virtualMachineRtpsWriter_->GetWriter());

    // -------------------------------------------
    // Initialize RtpsData
    // -------------------------------------------
    RTPS::RtpsData::Ptr rtpsData( new RTPS::RtpsData( RTPS::SerializedPayload(buffer) ));
    rtpsData->SetTimestamp(RTPS::Timestamp());

    // -------------------------------------------
    // Add RtpsData to DataCache
    // -------------------------------------------
    RTPS::CacheChange::Ptr cacheChange = virtualMachineRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::ALIVE, rtpsData, handle);

    // -------------------------------------------
    // 	After the transition the following post-conditions hold:
    // -------------------------------------------
    ASSERT(virtualMachineRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());

    return true;
}


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
bool CacheWriter::Dispose(const InstanceHandle &handle)
{
    // -- debug --
    ASSERT(virtualMachineRtpsWriter_->GetWriter());
    // -- debug --

    if(virtualMachineRtpsWriter_->GetWriter()->GetTopicKind() == RTPS::TopicKindId::WithKey)
    {
        RTPS::CacheChange::Ptr cacheChange = virtualMachineRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::NOT_ALIVE_DISPOSED, RTPS::RtpsData::Ptr(), handle);

        // -------------------------------------------
        // 	After the transition the following post-conditions hold:
        // -------------------------------------------
        ASSERT(virtualMachineRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());
    }

    return true;
}

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
bool CacheWriter::Unregister(const InstanceHandle &handle)
{
    ASSERT(virtualMachineRtpsWriter_->GetWriter());

    if(virtualMachineRtpsWriter_->GetWriter()->GetTopicKind() == RTPS::TopicKindId::WithKey)
    {
        RTPS::CacheChange::Ptr cacheChange = virtualMachineRtpsWriter_->GetWriter()->NewChange(RTPS::ChangeKindId::NOT_ALIVE_UNREGISTERED, RTPS::RtpsData::Ptr(), handle);

        // -------------------------------------------
        // 	After the transition the following post-conditions hold:
        // -------------------------------------------
        ASSERT(virtualMachineRtpsWriter_->GetWriter()->GetDataCache().GetSequenceNumberMax() == cacheChange->GetSequenceNumber());
    }
    return true;
}

}}

