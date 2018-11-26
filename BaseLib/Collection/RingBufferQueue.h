#pragma once

#include <BaseLib/ReadLocker.h>
#include <BaseLib/Status/FlowStatus.h>
#include <BaseLib/Templates/ContextObjectPtr.h>
#include <BaseLib/CommonDefines.h>

#include "RingBufferQueuePolicy.h"
#include "Collection.h"

namespace BaseLib { namespace Collection {

template <typename T>
class RingBufferQueueState
{
public:
    RingBufferQueueState(int capacity = 1)
        : typeQueue_(std::max(capacity, 1))
        , front_(0)
        , rear_(0)
        , length_(0)
    { }

    virtual ~RingBufferQueueState()
    { }

    void clearAndInitialize(const int& capacity)
    {
        typeQueue_ = std::vector<T>(std::max(capacity, 1));

        front_  = 0;
        rear_   = 0;
        length_ = 0;
    }

    bool enqueue(const T& t, bool isOverwrite, const int& capacity)
    {
        if(!isCapacity(isOverwrite, capacity))
        {
            return false;
        }

        typeQueue_[rear_] = t;
        moveRear(1, capacity, isOverwrite);

        return true;
    }

    bool dequeue(const int& capacity, T& t)
    {
        if(isEmpty(capacity))
        {
            return false;
        }

        t = typeQueue_[front_];
        moveFront(1, capacity);

        return true;
    }

    bool pop(int numElements, const int& capacity)
    {
        if(queueSize(capacity) < numElements)
        {
            return false;
        }

        moveFront(numElements, capacity);

        return true;
    }

    bool peek(int position, const int& capacity, T& t) const
    {
        if(!isDataAt(position, capacity))
        {
            IWARNING() << "Peek position " << position << " exceeds available data " << queueSize(capacity);
            return false;
        }

        int realPos = getBufferPosition(position, capacity);

        ASSERT(realPos <= capacity);

        t = typeQueue_[realPos];
        return true;
    }

    int remaining(const int& capacity) const
    {
        return capacity - length_;
        //if(remaining == 0)
//        {
//            int remaining = (rear_ >= front_)
//                   ? capacity - rear_ - front_
//                   : capacity - front_ + rear_;

//            bool isFirst = rear_ > front_;
//            IINFO() << "Remaining " << remaining << ". First=" << isFirst << ": " << capacity - rear_ - front_ << " Second=" << !isFirst << ": " << capacity - front_ + rear_;
//        }
//        return (rear_ > front_)
//                ? capacity - rear_ - front_
//                : capacity - front_ + rear_;
    }

    int queueSize(const int& capacity) const
    {
        return capacity - remaining(capacity);
    }

    bool isEmpty(const int& capacity) const
    {
        return remaining(capacity) == 0;
    }

    bool isCapacity(bool isOverwrite, int capacity) const
    {
        return !isOverwrite
               ? remaining(capacity) > 0
               : true;
    }

    bool isCapacityFor(int numElements, bool isOverwrite, const int& capacity) const
    {
        return !isOverwrite
               ? remaining(capacity) >= numElements && numElements <= capacity
               : true;
    }

    bool isDataAt(int position, const int& capacity) const
    {
        int realPos = getBufferPosition(position, capacity);

        return realPos >= queueSize(capacity);
    }

    bool isNumElementsInQueue(int numElements, const int& capacity) const
    {
        return numElements >= 0 && queueSize(capacity) >= numElements;
    }

private:
    void moveFront(int num, const int& capacity)
    {
        front_ = (front_ + num) % capacity;

        length_ = length_ - num;
        ASSERT(length_ >= 0);
    }

    void moveRear(int num, const int& capacity, bool isOverwrite)
    {
        rear_   = (rear_ + num) % capacity;
        length_ = length_ + num;

        if(isOverwrite && rear_ == front_)
        {
//            IINFO() << " rear_ = " << rear_ << " front_ = " << front_;
//            IINFO() << "Before: moved front in enqueue! Capacity : "  << capacity << " Size = " << this->queueSize(capacity) << ". Remaining = " << remaining(capacity);

            moveFront(1, capacity);

//            IINFO() << "After: moved front in enqueue! Capacity : "  << capacity << " Size = " << this->queueSize(capacity) << ". Remaining = " << remaining(capacity);
//            ASSERT(remaining(capacity) == 1);
        }
    }

    int getBufferPosition(int position, const int& capacity) const
    {
        return (rear_ + position) % capacity;
    }

private:
    std::vector<T> typeQueue_;

    int front_;
    int rear_;
    int length_;
};

/**
 * @brief The RingBufferQueue class
 *
 * Policies:
 *  - Support overwriting exisiting elements if capacity exchausted
 *  - Add FlowStatus to keep track of throughput?
 *
 * TODO: Wakeup on enqueue/dequeue is not working. Use Templates::Notifyable
 */
template <typename T, typename Lock = ReadWriteLock>
class RingBufferQueue
    : public QueueCollection<T>
      , public Templates::ContextObjectShared<RingBufferQueuePolicy, RingBufferQueueState<T>, Status::FlowStatus>
      , public ENABLE_SHARED_FROM_THIS_T2(RingBufferQueue, T, Lock)
{
public:
    RingBufferQueue(RingBufferQueuePolicy policy = RingBufferQueuePolicy::LimitedTo(20))
        : Templates::ContextObjectShared<RingBufferQueuePolicy, RingBufferQueueState<T>, Status::FlowStatus>
        (
            new RingBufferQueuePolicy(policy),
            new RingBufferQueueState<T>(policy.Limit()),
            new Status::FlowStatus()
        )
    { }

    virtual ~RingBufferQueue()
    { }

    CLASS_TRAITS(RingBufferQueue)

    typename RingBufferQueue::Ptr GetPtr()
    {
        return this->shared_from_this();
    }

    // --------------------------------------------
    // Enqueue, Dequeue and Peek
    // --------------------------------------------

    virtual bool TryEnqueue(const T& t)
    {
        WriteLocker lock(&mutex_);
        return enqueue(t);
    }

    virtual bool Enqueue(const T& t, int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);

        waitForCapacity(msecs, 1);
        return enqueue(t);
    }

    template <typename Container>
    bool Enqueue(const Container& container, int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);

        waitForCapacity(msecs, container.size());
        return enqueue<Container>(container);
    }

    virtual T TryDequeue()
    {
        WriteLocker lock(&mutex_);
        return dequeue(0);
    }

    virtual T Dequeue(int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);
        return dequeue(msecs);
    }

    template <typename Container>
    Container Dequeue(int numElements, int64 msecs)
    {
        WriteLocker lock(&mutex_);
        return dequeue<Container>(numElements, msecs);
    }

    template <typename Container>
    Container DequeueAll(int64 msecs = LONG_MAX)
    {
        WriteLocker lock(&mutex_);
        return dequeue<Container>(queueSize(), msecs);
    }

    template <typename Container>
    Container DequeueUpto(int numElements)
    {
        WriteLocker lock(&mutex_);
        return dequeue<Container>(std::min(queueSize(), numElements), 0);
    }

    virtual T Peek(int64 msecs = LONG_MAX) const
    {
        ReadLocker lock(&mutex_);
        return peek(1, msecs);
    }

    template <typename Container>
    Container Peek(int numElements, int64 msecs) const
    {
        ReadLocker lock(&mutex_);
        return peek<Container>(numElements, msecs);
    }

    template <typename Container>
    Container PeekAll(int64 msecs = LONG_MAX) const
    {
        ReadLocker lock(&mutex_);
        return peek<Container>(queueSize(), msecs);
    }

    virtual bool Pop(int numElements)
    {
        WriteLocker lock(&mutex_);
        return this->data()->pop(numElements, this->config()->Limit());
    }

    // --------------------------------------------
    // Check status of ring buffer, etc
    // --------------------------------------------

    virtual int Length() const
    {
        ReadLocker lock(&mutex_);
        return queueSize();
    }

    virtual bool IsEmpty() const
    {
        ReadLocker lock(&mutex_);
        return this->data()->isEmpty(this->config()->Limit());
    }

    virtual bool IsFull() const
    {
        ReadLocker lock(&mutex_);
        return !this->data()->isCapacity(this->config()->IsOverwrite(), this->config()->Limit());
    }

    virtual int Capacity() const
    {
        ReadLocker lock(&mutex_);
        return this->config()->Limit();
    }

    virtual void Clear()
    {
        WriteLocker lock(&mutex_);
        this->data()->clearAndInitialize(this->config()->Limit());
    }

    virtual void WakeAll() const
    {
        ReadLocker lock(&mutex_);
        this->waitForData_.wakeAll();
        this->waitForCapacity_.wakeAll();
    }

    virtual void WakeOne() const
    {
        ReadLocker lock(&mutex_);
        this->waitForData_.wakeOne();
        this->waitForCapacity_.wakeOne();
    }

    virtual bool WaitForData(int numElements, int64 msecs) const
    {
        ReadLocker lock(&mutex_);
        return waitForData(msecs, numElements);
    }

    virtual bool WaitForCapacity(int numElements, int64 msecs) const
    {
        ReadLocker lock(&mutex_);
        return waitForCapacity(msecs, numElements);
    }

private:

    // --------------------------------------------
    // private implementation
    // --------------------------------------------

    bool enqueue(const T& t)
    {
        bool enqueued = this->data()->enqueue(t, this->config()->IsOverwrite(), this->config()->Limit());

        if(enqueued)
        {
            this->status()->Enqueue(1);
        }
        else
        {
            this->status()->Reject(1);
            IWARNING() << "Queue is full! Discarding enqueue(" << t << "). " << "Status " << *this->status();
        }

        return enqueued;
    }

    template <typename Container>
    bool enqueue(const Container& container)
    {
        if(!hasCapacity(container.size()))
        {
            IWARNING() << "Queue cannot fit " << container.size() << " elements!";
            return false;
        }

        for(typename Container::const_iterator it = container.begin(), it_end = container.end(); it != it_end; ++it)
        {
            bool isEnqueued = enqueue(*it);

            ASSERT(isEnqueued);
        }

        return true;
    }

    T dequeue()
    {
        T    t;
        bool dequeued = this->data()->dequeue(this->config()->Limit(), t);

        if(dequeued)
        {
            this->status()->Dequeue(1);
        }
        else
        {
            this->status()->Reject(1);
            ICRITICAL() << "Queue is empty! Status " << *this->status();
        }

        return t;
    }

    T dequeue(int64 msecs)
    {
        waitForData(msecs, 1);
        return dequeue();
    }

    template <typename Container>
    bool dequeue(Container& container, int numElements)
    {
        if(numElements <= 0 || queueSize() < numElements)
        {
            IWARNING() << "Queue does not have " << numElements << " only " << queueSize();
            return false;
        }

        for(int i = 0; i < numElements && queueSize() > 0; ++i)
        {
            container.push_back(this->dequeue());
        }

        return true;
    }

    template <typename Container>
    Container dequeue(int numElements, int64 msecs)
    {
        if(numElements <= 0)
        {
            return Container();
        }

        waitForData(msecs, numElements);

        Container container;

        bool isDequeued = dequeue<Container>(container, numElements);

        // -- debug --
        ASSERT(isDequeued);
        // -- debug --

        return container;
    }

    T peek(int position) const
    {
        T t;
        this->data()->peek(position, this->config()->Limit(), t);
        return t;
    }

    T peek(int position, int64 msecs) const
    {
        bool hasData = this->waitForDataAt(msecs, position);
        if(!hasData)
        {
            return T();
        }
        return peek(position);
    }

    template <typename Container>
    bool peek(Container& container, int numElements) const
    {
        if(!this->data()->isNumElementsInQueue(numElements, this->config()->Limit()))
        {
            IWARNING() << "Queue does not have " << numElements << " only " << queueSize();
            return false;
        }

        int position = 0;

        for(int i = 0; i < numElements; i++)
        {
            ASSERT(this->data()->isDataAt(position, this->config()->Limit()));

            container.push_back(this->peek(position));

            position = (position + 1) % this->config()->Limit();
        }

        return true;
    }

    template <typename Container>
    Container peek(int numElements, int64 msecs) const
    {
        if(numElements <= 0) return Container();

        this->waitForData(msecs, numElements);

        Container container;

        bool isPeeked = peek<Container>(container, numElements);

        // -- debug --
        ASSERT(isPeeked);
        // -- debug --

        return container;
    }

    bool waitForCapacity(int64 msecs, int numElements) const
    {
        if(numElements < 0)
        {
            IWARNING() << "num elements is sub zero " << numElements;
            return false;
        }

        ElapsedTimer timer(msecs);

        while(!hasCapacity(numElements) && !timer.HasExpired())
        {
            bool isWoken = waitForCapacity_.wait(&mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return hasCapacity(numElements);
    }

    bool waitForData(int64 msecs, int numElements) const
    {
        if(numElements < 0)
        {
            IWARNING() << "num elements is sub zero " << numElements;
            return false;
        }

        ElapsedTimer timer(msecs);

        while((queueSize() < numElements) && !timer.HasExpired())
        {
            bool isWoken = waitForData_.wait(&mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return queueSize() >= numElements;
    }

    bool waitForDataAt(int64 msecs, int position) const
    {
        if(position < 0)
        {
            IWARNING() << "num position is sub zero " << position;
            return false;
        }

        ElapsedTimer timer(msecs);

        while(!isDataAt(position) && !timer.HasExpired())
        {
            bool isWoken = waitForData_.wait(&mutex_, timer.Remaining().InMillis());
            if(isWoken) break;
        }

        return isDataAt(position);
    }

    bool isDataAt(int position) const
    {
        return this->data()->isDataAt(position, this->config()->Limit());
    }

    int queueSize() const
    {
        return this->data()->queueSize(this->config()->Limit());
    }

    bool hasCapacity(int numElements) const
    {
        return this->data()->isCapacityFor(numElements, this->config()->IsOverwrite(), this->config()->Limit());
    }

private:
    mutable Lock          mutex_;
    mutable WaitCondition waitForData_;
    mutable WaitCondition waitForCapacity_;
};

}}
