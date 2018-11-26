#include "RTPS/Elements/ParticipantMessageData.h"

namespace RTPS
{

GuidPrefix ParticipantMessageData::participantGuidPrefix() const
{
    return participantGuidPrefix_;
}

void ParticipantMessageData::setParticipantGuidPrefix(const GuidPrefix &participantGuidPrefix)
{
    participantGuidPrefix_ = participantGuidPrefix;
}
NetworkLib::SerializeWriter::Ptr ParticipantMessageData::data() const
{
    return data_;
}

void ParticipantMessageData::setData(const NetworkLib::SerializeWriter::Ptr &data)
{
    data_ = data;
}



}
