#ifndef RTPS_Elements_ParticipantMessageData_h_IsIncluded
#define RTPS_Elements_ParticipantMessageData_h_IsIncluded

#include"RTPS/IncludeExtLibs.h"
#include"RTPS/Elements/GuidPrefix.h"
#include"RTPS/Export.h"

namespace RTPS
{
/*
The Behavior module within the PIM (Section 8.4) defines the DataType ParticipantMessageData.
This type is the logical content of the BuiltinParticipantMessageWriter and
BuiltinParticipantMessageReader built-in Endpoints.

The PSM maps the ParticipantMessageData tpe into the following IDL:

struct ParticipantMessageData
{
    GuidPrefix_t participantGuidPrefix;
    octet [4] kind;
    sequence<octet> data;
};

The following values for the kind field are reserved by RTPS:

#define PARTICIPANT_MESSAGE_DATA_KIND_UNKNOWN {0x00, 0x00, 0x00, 0x00}
#define PARTICIPANT_MESSAGE_DATA_KIND_AUTOMATIC_LIVELINESS_UPDATE {0x00, 0x00, 0x00, 0x01}
#define PARTICIPANT_MESSAGE_DATA_KIND_MANUAL_LIVELINESS_UPDATE {0x00, 0x00, 0x00, 0x02}

RTPS also reserves for future use all values of the kind field where the most significant bit is
not set. Therefore:

    kind.value[0] & 0x80 == 0 // reserved by RTPS
    kind.value[0] & 0x80 == 1 // vendor specific kind

Implementations can decide the upper length of the data field but must be able to support at least 128 bytes.
Following the CDR encoding, the wire representation of the ParticipantMessageData structure is:

0...2...........8...............16..............24..............32
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   unsigned long data.length                   |
+---------------+---------------+---------------+---------------+
|                                                               |
~                   octet[] data.value                          ~
|                                                               |
+---------------+---------------+---------------+---------------+
*/

/**
 * @brief The ParticipantMessageData class
 */
class DLL_STATE ParticipantMessageData : public NetworkLib::NetObjectBase
{
public:
    ParticipantMessageData(
            const GuidPrefix &participantGuidPrefix,
//            char kind[4],
            NetworkLib::SerializeWriter::Ptr data)
        : participantGuidPrefix_(participantGuidPrefix)
//        , kind_(kind)
        , data_(data)
    {}
    virtual ~ParticipantMessageData()
    {}

    virtual void Write(SerializeWriter *writer) const
    {
        ASSERT(data_);

        writer->WriteInt32(data_->GetLength());
        writer->Write(data_->GetData(), data_->GetLength());
    }

    virtual void Read(SerializeReader *reader)
    {
        ASSERT(data_);

        int length = reader->ReadInt32();

        std::vector<char> payload(length, 0);
		reader->Read(&payload[0], length);
        data_->Write(&payload[0], payload.size());
    }

    GuidPrefix participantGuidPrefix() const;
    void setParticipantGuidPrefix(const GuidPrefix &participantGuidPrefix);

    NetworkLib::SerializeWriter::Ptr data() const;
    void setData(const NetworkLib::SerializeWriter::Ptr &data);

private:
    GuidPrefix  participantGuidPrefix_;
//    char        kind_[4];

    NetworkLib::SerializeWriter::Ptr 	data_;
};

}

#endif
