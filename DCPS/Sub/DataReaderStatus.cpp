#include "DCPS/Sub/DataReaderStatus.h"

namespace DCPS { namespace Subscription
{

DataReaderStatus::DataReaderStatus()
{
}

DataReaderStatus::~DataReaderStatus()
{

}

template<>
Status::LivelinessChangedStatus& DataReaderStatus::status<Status::LivelinessChangedStatus>()
{
    return livelinessStatus_;
}

template<>
const Status::LivelinessChangedStatus& DataReaderStatus::status<Status::LivelinessChangedStatus>() const
{
    return livelinessStatus_;
}


template<>
Status::SampleRejectedStatus& DataReaderStatus::status<Status::SampleRejectedStatus>()
{
    return sampleRejectedStatus_;
}

template<>
const Status::SampleRejectedStatus& DataReaderStatus::status<Status::SampleRejectedStatus>() const
{
    return sampleRejectedStatus_;
}


template<>
Status::SampleLostStatus& DataReaderStatus::status<Status::SampleLostStatus>()
{
    return sampleLostStatus_;
}

template<>
const Status::SampleLostStatus& DataReaderStatus::status<Status::SampleLostStatus>() const
{
    return sampleLostStatus_;
}


template<>
Status::RequestedDeadlineMissedStatus& DataReaderStatus::status<Status::RequestedDeadlineMissedStatus>()
{
    return deadlineMissedStatus_;
}

template<>
const Status::RequestedDeadlineMissedStatus& DataReaderStatus::status<Status::RequestedDeadlineMissedStatus>() const
{
    return deadlineMissedStatus_;
}


template<>
Status::RequestedIncompatibleQosStatus& DataReaderStatus::status<Status::RequestedIncompatibleQosStatus>()
{
    return incompatibleQosStatus_;
}

template<>
const Status::RequestedIncompatibleQosStatus& DataReaderStatus::status<Status::RequestedIncompatibleQosStatus>() const
{
    return incompatibleQosStatus_;
}


template<>
Status::SubscriptionMatchedStatus& DataReaderStatus::status<Status::SubscriptionMatchedStatus>()
{
    return subscriptionMatchedStatus_;
}

template<>
const Status::SubscriptionMatchedStatus& DataReaderStatus::status<Status::SubscriptionMatchedStatus>() const
{
    return subscriptionMatchedStatus_;
}


}} // namespace DCPS::Subscription
