#ifndef BaseLib_Collection_BlockingQueue_h_IsIncluded
#define BaseLib_Collection_BlockingQueue_h_IsIncluded

#include "BaseLib/CommonDefines.h"
#include "BaseLib/ProtectedVar.h"
#include "BaseLib/Templates/ProtectedBool.h"
#include "BaseLib/Collection/PCList.h"
#include "BaseLib/ElapsedTimer.h"

namespace BaseLib { namespace Collection
{

/**
 * @brief The BlockingQueue class is a FIFO bounded blocking queue backed by a PCList<T>
 *
 * The head of the queue is that element that has been on the queue the longest time. The tail of the queue
 * is that element that has been on the queue the shortest time. New elements are inserted at the tail of
 * the queue, and the queue retrieval operations obtain elements at the head of the queue.
 *
 * This is a classic "bounded buffer", in which a fixed-sized array holds elements inserted by producers and
 * extracted by consumers. Once created, the capacity cannot be increased. Attempts to put an element into
 * a full queue will result in the operation blocking; attempts to take an element from an empty queue will
 * similarly block.
 */
template<typename T, typename Container = PCList<T> >
class BlockingQueue
{
public:
    BlockingQueue(int64 capacity = LONG_MAX)
        : capacity_(capacity)
        , isInterrupted_(false)
        , numWaiters_(0)
        , list_()
    {}
    virtual ~BlockingQueue()
    {}

public:
    T Dequeue(int64 msecs = LONG_MAX)
    {
        return list_.consumer_pop_front(msecs);
    }

    T DequeueBack(int64 msecs = LONG_MAX)
    {
        return list_.consumer_pop_back(msecs);
    }

    bool EnqueueFront(T x, int64 msecs = LONG_MAX)
    {
        if(waitForCapacity(msecs) == false)
            return false;

        list_.producer_push_front(x);

        return true;
    }

    bool Enqueue(T x, int64 msecs = LONG_MAX)
    {
        if(waitForCapacity(msecs) == false)
            return false;

        list_.producer_push_back(x);

        if(list_.length() > capacity_)
            IDEBUG() << "Violated capacity: " << list_.length();

        return true;
    }

    bool HasCapacity() const
    {
        return capacity_ > list_.length();
    }

    bool IsEmpty() const
    {
        return list_.empty();
    }

    unsigned int Length() const
    {
        return list_.length();
    }

    int64 Capacity() const
    {
        return capacity_;
    }

    void SetCapacity(int64 capacity)
    {
        capacity_ = capacity;
    }

    void Destruct()
    {
        isInterrupted_ = true;
        list_.wakeAll();
    }

    int NumWaiters() const
    {
        return numWaiters_;
    }

    void WakeAll() const
    {
        list_.wakeAll();
    }

    void WakeOne() const
    {
        list_.wakeOne();
    }

    Container& list()
    {
        return list_;
    }

    const Container& list() const
    {
        return list_;
    }

private:
    /**
     * @brief waitForCapacity
     * @param msecs
     * @return
     */
    bool waitForCapacity(int64 msecs = LONG_MAX)
    {
        ElapsedTimer timer(msecs);

        while(list_.length() >= capacity_.get() && isInterrupted_.get() == false)
        {
            ++numWaiters_;

            bool isWoken = this->list_.wait(msecs);

            --numWaiters_;

            ASSERT(numWaiters_ >= 0);

            if(isWoken == false || isInterrupted_.get() == true)
            {
                break;
            }
            else if(timer.HasExpired())
                break;
        }

        // NB! Not safe !!!
        //if(isInterrupted_ == true && numWaiters == 0)
        //{
        //    isInterrupted_ = false;
        //    break;
        //}

        return (list_.length() < capacity_.get());
    }

    /*bool waitForCapacity(int64 msecs = LONG_MAX) const
    {
        list_.wait(msecs_);

        return (list_.length() >= capacity);
    }*/
    /*bool waitForCapacity(int64 msecs = LONG_MAX) const
    {
        while(list_.length() < capacity_)
        {
            numWaiters_++;

            bool isWoken = this->list_.wait(msecs_);

            numWaiters_--;

            ASSERT(numWaiters_ >= 0);

            //if(isInterrupted_ == true && numWaiters == 0)
            //{
            //    isInterrupted_ = false;
            //    break;
            //}
            if(isWoken == false || isInterrupted_ == true)
            {
                break;
            }
        }

        return (list_.length() >= capacity);
    }*/

private:
    ProtectedVar<int64> capacity_;

    Templates::ProtectedBool isInterrupted_;

    ProtectedVar<int> numWaiters_;

    Container               list_;
};

}}

#endif

