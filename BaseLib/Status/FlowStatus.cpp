#include "BaseLib/Status/FlowStatus.h"

namespace BaseLib { namespace Status
{

FlowStatus::FlowStatus(Duration windowDuration)
    : enqueued_(Kind::ENQUEUED, windowDuration)
    , dequeued_(Kind::DEQUEUED, windowDuration)
    , rejected_(Kind::REJECTED, windowDuration)
    , filtered_(Kind::FILTERED, windowDuration)
{ }

FlowStatus::~FlowStatus()
{ }

void FlowStatus::Enqueue(int num)
{
    enqueued_.Next(num);

    Locker locker(this);
    queueSize_ += num;
}

void FlowStatus::Dequeue(int num)
{
    dequeued_.Next(num);

    Locker locker(this);
    queueSize_ = std::max(0, queueSize_ - num);
}

void FlowStatus::Reject(int num)
{
    rejected_.Next(num);
}

void FlowStatus::Filter(int num)
{
    filtered_.Next(num);
}

Count FlowStatus::QueueSize() const
{
    Locker locker(this);
    return queueSize_;
}

StateStatusTimestamped<FlowStatus::Kind> FlowStatus::Enqueued() const
{
    return enqueued_;
}

StateStatusTimestamped<FlowStatus::Kind> FlowStatus::Dequeued() const
{
    return dequeued_;
}

StateStatusTimestamped<FlowStatus::Kind> FlowStatus::Rejected() const
{
    return rejected_;
}

StateStatusTimestamped<FlowStatus::Kind> FlowStatus::Filtered() const
{
    return filtered_;
}

Fraction FlowStatus::DeliveryRatio() const
{
    return Fraction(dequeued_.TotalSum().Number(), enqueued_.TotalSum().Number());
}

}}
