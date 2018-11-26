#pragma once

#include"RxObserver/SubjectGroup.h"

namespace BaseLib { namespace Concurrent
{

template <typename EventType>
class EventSource
{
public:
    virtual ~EventSource() {}

    virtual Subscriptions::Ptr Subscribe(std::shared_ptr<EventType> observer, ObserverPolicy policy) const = 0;

    virtual bool Initialize(typename SubjectGroup<EventType>::Ptr subject) = 0;

    virtual bool IsInitialized() const = 0;
};

}}
