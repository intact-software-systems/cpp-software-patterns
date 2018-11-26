#include "BaseLib/Collection/RingBufferQueuePolicy.h"

namespace BaseLib { namespace Collection {

RingBufferQueuePolicy::RingBufferQueuePolicy()
    : capacity_(1)
    , overwrite_(false)
{ }

RingBufferQueuePolicy::RingBufferQueuePolicy(const Policy::MaxLimit<int>& capacity, bool overwrite)
    : capacity_(capacity)
    , overwrite_(overwrite)
{ }

RingBufferQueuePolicy::RingBufferQueuePolicy(const RingBufferQueuePolicy& policy)
    : capacity_(policy.capacity_)
    , overwrite_(policy.overwrite_)
{ }

RingBufferQueuePolicy::~RingBufferQueuePolicy()
{ }

int RingBufferQueuePolicy::Limit() const
{
    return capacity_.Limit();
}

bool RingBufferQueuePolicy::IsOverwrite() const
{
    return overwrite_;
}

RingBufferQueuePolicy RingBufferQueuePolicy::Overwrite(int capacity)
{
    return RingBufferQueuePolicy(Policy::MaxLimit<int>::LimitTo(Policy::LimitKind::INCLUSIVE, capacity), true);
}

RingBufferQueuePolicy RingBufferQueuePolicy::LimitedTo(int capacity)
{
    return RingBufferQueuePolicy(Policy::MaxLimit<int>::LimitTo(Policy::LimitKind::INCLUSIVE, capacity), false);
}

}}

