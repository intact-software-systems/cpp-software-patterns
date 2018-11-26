#include "DCPS/Pub/DataWriterStatus.h"

namespace DCPS { namespace Publication
{

DataWriterStatus::DataWriterStatus()
{
}

DataWriterStatus::~DataWriterStatus()
{

}

template<>
Status::LivelinessLostStatus& DataWriterStatus::status<Status::LivelinessLostStatus>()
{
    return livelinessLostStatus_;
}

template<>
const Status::LivelinessLostStatus& DataWriterStatus::status<Status::LivelinessLostStatus>() const
{
    return livelinessLostStatus_;
}

template<>
Status::OfferedDeadlineMissedStatus& DataWriterStatus::status<Status::OfferedDeadlineMissedStatus>()
{
    return offeredDeadlineMissedStatus_;
}

template<>
const Status::OfferedDeadlineMissedStatus& DataWriterStatus::status<Status::OfferedDeadlineMissedStatus>() const
{
    return offeredDeadlineMissedStatus_;
}


template<>
Status::OfferedIncompatibleQosStatus& DataWriterStatus::status<Status::OfferedIncompatibleQosStatus>()
{
    return offeredIncompatibleQosStatus_;
}

template<>
const Status::OfferedIncompatibleQosStatus& DataWriterStatus::status<Status::OfferedIncompatibleQosStatus>() const
{
    return offeredIncompatibleQosStatus_;
}


template<>
Status::PublicationMatchedStatus& DataWriterStatus::status<Status::PublicationMatchedStatus>()
{
    return publicationMatchedStatus_;
}

template<>
const Status::PublicationMatchedStatus& DataWriterStatus::status<Status::PublicationMatchedStatus>() const
{
    return publicationMatchedStatus_;
}


}} // namespace DCPS::Publication
