#pragma once

#include "RxSignal/CommonDefines.h"
#include <RxSignal/Observer/QueueObserver.h>
#include "RxSignal/Collection/RingBufferQueueObservable.h"

namespace BaseLib { namespace Collection {

/**
 * TODO: Use WaitSets in RingBufferQueue and then attach GuardConditions for each Queue
 * in this RingBufferQueueMap. This applies both to waiting for data, and waiting for capacity.
 */
template <typename T, typename QID, typename Lock = ReadWriteLock>
class RingBufferQueueMap
    : public Observer::QueueObserver1<T>
      , public ENABLE_SHARED_FROM_THIS_T3(RingBufferQueueMap, T, QID, Lock)
    //, public Templates::Notifyable<RingBufferQueueMap<T, QID, Lock> >
{
public:
    typedef RingBufferQueueObservable<T, Lock>     Queue;
    typedef std::map<QID, std::shared_ptr<Queue>>  MapQueue;
    typedef std::pair<QID, std::shared_ptr<Queue>> PairQueue;
    typedef std::vector<QID>                       VectorQID;

public:
    RingBufferQueueMap(unsigned int capacity = 20)
        : capacity_(capacity)
    { }

    virtual ~RingBufferQueueMap()
    { }

    CLASS_TRAITS(RingBufferQueueMap)

    typename RingBufferQueueMap::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    std::shared_ptr<Queue> GetQueue(QID id)
    {
        WriteLocker lock(&mutex_);
        if(queues_.find(id) == queues_.end())
        {
            std::shared_ptr<Queue> buffer(new Queue(RingBufferQueuePolicy::LimitedTo(capacity_)));

            buffer->Connect(this);

            queues_.insert(PairQueue(id, buffer));
            return buffer;
        }

        return queues_.at(id);
    }

    bool RemoveQueue(QID id)
    {
        WriteLocker lock(&mutex_);

        typename MapQueue::iterator it = queues_.find(id);

        if(it != queues_.end())
        {
            std::shared_ptr<Queue> queue = it->second;

            // -- debug --
            ASSERT(queue);
            // -- debug --

            queue->Disconnect(this);
        }

        return queues_.erase(id) == 1;
    }

    void ClearAll()
    {
        WriteLocker                     lock(&mutex_);
        for(typename MapQueue::iterator it = queues_.begin(), it_end = queues_.end(); it != it_end; ++it)
        {
            std::shared_ptr<Queue> queue = it->second;

            // -- debug --
            ASSERT(queue);
            // -- debug --

            queue->Clear();
        }
    }

    VectorQID SelectQueuesWithData(int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);
        waitForData_.wait(&mutex_, msecs);

        VectorQID queuesWithData;

        for(typename MapQueue::iterator it = queues_.begin(), it_end = queues_.end(); it != it_end; ++it)
        {
            std::shared_ptr<Queue> queue = it->second;

            // -- debug --
            ASSERT(queue);
            // -- debug --

            if(!queue->IsEmpty())
            {
                queuesWithData.push_back(it->first);
            }
        }

        return queuesWithData;
    }

    VectorQID SelectQueuesWithSize(int64 minimumSize, int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);
        waitForData_.wait(&mutex_, msecs);

        VectorQID queuesWithData;

        for(typename MapQueue::iterator it = queues_.begin(), it_end = queues_.end(); it != it_end; ++it)
        {
            std::shared_ptr<Queue> queue = it->second;

            // -- debug --
            ASSERT(queue);
            // -- debug --

            if(queue->Length() >= minimumSize)
            {
                queuesWithData.push_back(it->first);
            }
        }

        return queuesWithData;
    }

    bool IsQueue(QID id) const
    {
        ReadLocker lock(&mutex_);
        return queues_.find(id) != queues_.end();
    }

    bool IsEmpty() const
    {
        ReadLocker lock(&mutex_);
        return queues_.empty();
    }

    unsigned int Length() const
    {
        ReadLocker lock(&mutex_);
        return queues_.size();
    }

    // -----------------------------------
    // Inteface QueueObserver1
    // -----------------------------------

    virtual void OnDataIn(T)
    {
        WriteLocker lock(&mutex_);
        waitForData_.wakeAll();
    }

    virtual void OnDataOut(T)
    {
        // Anything to do?
    }

private:
    const unsigned int capacity_;

    MapQueue queues_;

private:
    mutable Lock          mutex_;
    mutable WaitCondition waitForData_;
};

}}
