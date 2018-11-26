#pragma once

#include"RxSignal/CommonDefines.h"
#include"RxSignal/Subject/QueueSubject.h"

namespace BaseLib { namespace Collection {

/**
 * TODO: Subject is not fully implemented yet!
 */
template <typename T, typename Lock = ReadWriteLock>
class RingBufferQueueObservable
    : public RingBufferQueue<T, Lock>
      , public Subject::QueueSubject1<T>
{
    typedef RingBufferQueue<T, Lock> Q;

public:
    RingBufferQueueObservable(RingBufferQueuePolicy policy = RingBufferQueuePolicy::LimitedTo(20))
        : RingBufferQueue<T, Lock>(policy)
    { }

    virtual ~RingBufferQueueObservable()
    { }

    CLASS_TRAITS(RingBufferQueueObservable)

    // --------------------------------------------
    // Enqueue, Dequeue and Peek
    // --------------------------------------------

    virtual bool TryEnqueue(const T& t)
    {
        bool enqueued = Q::template TryEnqueue(t);

        if(enqueued)
        {
            this->OnDataIn(t);
            return true;
        }
        return false;
    }

    virtual bool Enqueue(const T& t, int64 msecs = LONG_MAX)
    {
        bool enqueued = Q::template Enqueue(t, msecs);

        if(enqueued)
        {
            this->OnDataIn(t);
            return true;
        }
        return false;
    }

    template <typename Container>
    bool Enqueue(const Container& container, int64 msecs = LONG_MAX)
    {
        bool enqueued = Q::template Enqueue<Container>(container, msecs);
        if(enqueued)
        {
            // TODO: this->OnDataIn(t);
            return true;
        }
        return false;
    }
};

}}
