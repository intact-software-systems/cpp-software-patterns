#pragma once

#include <BaseLib/MutexLocker.h>
#include <BaseLib/Duration.h>
#include <BaseLib/Count.h>
#include <BaseLib/Event.h>
#include <BaseLib/Status/EventStatusMethods.h>

namespace BaseLib { namespace Status {

template <typename T>
struct Bucket
{
    T start;
    T end;
};

template <typename T>
struct MetricStatistics
{
    T minimum;
    T maximum;
    T median;

    double average           = 0.0f;
    int    instances         = 0;
    double standardDeviation = 0.0f;
    double variance          = 0.0f;

    Bucket<T> bucket;
    Duration  window;
};

/**
 * Idea is to store minimum, maximum, compute averages, etc
 *
    minimum = MAXIMUM_WEIGHT
    maximum = 0
    median = 0
    average = 0
    instances = 0
    standard_deviation = 0
    variance = 0

    typedef Policy::Range Bucket;

    std::list<Bucket> Compute(BucketSize size);
    std::list<Bucket> Compute(Duration bucketWindow);

 */
template <typename T>
class Metric
{
    typedef Collection::IMap<Timestamp, CountType<T>> TimestampedCount;

public:


private:

//    MetricStatistics<T> initialize(const TimestampedCount& timestampedCount_, Duration duration, Policy::Range range) const
//    {
//        MetricStatistics<T> metric;
//
//        for(typename TimestampedCount::const_reverse_iterator it = timestampedCount_.rbegin(), it_end = timestampedCount_.rend();
//            it != it_end;
//            ++it)
//        {
//            const Timestamp& timestamp = it->first;
//
//            if(timestamp.SinceStamped() < duration)
//            {
//                // initialize
//                metric.instances++;
//            }
//            else
//            {
//                break;
//            }
//        }
//
//        return metric;
//    }

private:
    MetricStatistics<T> statistics_;
    //Policy::Range bucketRanges;
    // Divide into buckets [0-10][10-20] ... etc

private:
    mutable TimestampedCount timestampedCount_;
    const Duration           window_;
};


/**
 * Idea is to store minimum, maximum, compute averages, etc
 *
    minimum = MAXIMUM_WEIGHT
    maximum = 0
    median = 0
    average = 0
    instances = 0
    standard_deviation = 0
    variance = 0
 */
template <typename StateType, typename T>
class EventStatusTracker : public StatusTrackerTimestamped<T>
{
    typedef MutexTypeLocker<EventStatusTracker<StateType, T>>                                      Locker;
    typedef Collection::IMap<StateType, std::shared_ptr<StateStatusTimestampedType<StateType, T>>> StateMap;
    typedef std::list<Concurrent::Event::Ptr>                                                      RecentStatuses;

public:
    EventStatusTracker(Duration windowDuration = Duration::FromMinutes(30), Count capacity = Count::Create(500))
        : StatusTrackerTimestamped<T>(windowDuration)
        , stateMap_()
        , recentStatuses_()
        , capacity_(capacity)
    { }

    virtual ~EventStatusTracker()
    { }

    std::shared_ptr<StateStatusTimestampedType<StateType, T>> State(StateType state) const
    {
        Locker locker(this);
        return getOrCreate(state);
    }

    typename Concurrent::Event::Ptr CurrentEvent() const
    {
        Locker locker(this);
        return currentEvent();
    }

    typename Concurrent::Event::Ptr PreviousEvent() const
    {
        Locker locker(this);
        return previousEvent();
    }

    StateType CurrentState() const
    {
        Locker locker(this);
        return currentState();
    }

    StateType PreviousState() const
    {
        Locker locker(this);
        return previousState();
    }

    std::shared_ptr<StateStatusTimestampedType<StateType, T>> Current() const
    {
        Locker locker(this);
        return stateMap_.get(currentState());
    }

    std::shared_ptr<StateStatusTimestampedType<StateType, T>> Previous() const
    {
        Locker locker(this);
        return stateMap_.get(previousState());
    }

    bool IsCurrent() const
    {
        Locker locker(this);
        return currentEvent() != nullptr;
    }

    bool IsPrevious() const
    {
        Locker locker(this);
        return previousEvent() != nullptr;
    }

    Duration TimeSinceFirstAccessFromTo(StateType from, StateType to)
    {
        Locker locker(this);

        auto fromStatus = getOrCreate(from);
        auto toStatus   = getOrCreate(to);

        return Duration::FromMilliseconds(fromStatus->TimeSinceFirstAccess().InMillis() - fromStatus->TimeSinceFirstAccess().InMillis());
    }

    Duration TimeSinceMostRecentAccessFromTo(StateType from, StateType to)
    {
        Locker locker(this);

        auto fromStatus = getOrCreate(from);
        auto toStatus   = getOrCreate(to);

        return Duration::FromMilliseconds(fromStatus->TimeSinceMostRecentAccess().InMillis() - fromStatus->TimeSinceMostRecentAccess().InMillis());
    }

    // ------------------------------------------------------
    // Update counters and timestamps
    // ------------------------------------------------------

    template <typename Source>
    void Next(const Source* source, StateType state, T num)
    {
        StatusTrackerTimestamped<T>::Next(num);

        Locker locker(this);
        next(state, num);
        next<Source>(source, state, num);
    }

    virtual void NextState(StateType state, T num)
    {
        StatusTrackerTimestamped<T>::Next(num);

        Locker locker(this);
        next(state, num);
        next<EventStatusTracker<StateType, T>>(this, state, num);
    }

    virtual void Reset()
    {
        StatusTrackerTimestamped<T>::Reset();

        Locker locker(this);
        reset();
    }

private:
    void next(StateType state, T num)
    {
        std::shared_ptr<StateStatusTimestampedType<StateType, T>> status = getOrCreate(state);
        status->Next(num);
    }

    template <typename Source>
    void next(const Source* source, StateType state, T num)
    {
        Concurrent::Event::Ptr event = Concurrent::Event::Create<StateType, Source>(
            new Concurrent::Event2<StateType, T>(
                state,
                state,
                num
            ),
            state,
            source
        );

        recentStatuses_.push_front(event);

        if(int(recentStatuses_.size()) > this->capacity_.Number())
        {
            recentStatuses_.pop_back();
        }
    }

    void reset()
    {
        stateMap_.clear();
        recentStatuses_.clear();
    }

    StateType currentState() const
    {
        Concurrent::Event::Ptr event = currentEvent();
        if(!event)
        {
            IWARNING() << "No current state";
            return StateType();
        }
        return extractState(event);
    }

    StateType previousState() const
    {
        Concurrent::Event::Ptr event = previousEvent();
        if(!event)
        {
            IWARNING() << "No previous state";
            return StateType();
        }
        return extractState(event);
    }

    StateType extractState(const Concurrent::Event::Ptr& event) const
    {
        std::shared_ptr<Concurrent::Event2<StateType, T>> eventData = std::dynamic_pointer_cast<Concurrent::Event2<StateType, T> >(event->Data());
        ASSERT(eventData);

        return eventData->Arg1();
    }

    typename Concurrent::Event::Ptr previousEvent() const
    {
        RecentStatuses::const_iterator it;
        RecentStatuses::const_iterator it_end;
        int                            position = 1;

        for(it = recentStatuses_.begin(), it_end = recentStatuses_.end();
            it != it_end;
            ++it, ++position)
        {
            if(position == 2)
            {
                return *it;
            }
        }

        return Concurrent::Event::Ptr();
    }

    typename Concurrent::Event::Ptr currentEvent() const
    {
        return recentStatuses_.empty()
               ? Concurrent::Event::Ptr()
               : recentStatuses_.front();
    }

    std::shared_ptr<StateStatusTimestampedType<StateType, T>> getOrCreate(StateType state) const
    {
        typename StateMap::const_iterator it = stateMap_.find(state);
        if(it == stateMap_.end())
        {
            std::shared_ptr<StateStatusTimestampedType<StateType, T>> status(
                new StateStatusTimestampedType<StateType, T>(
                    state,
                    this->windowDuration_
                )
            );

            stateMap_.put(state, status);
            return status;
        }
        else
        {
            std::shared_ptr<StateStatusTimestampedType<StateType, T>> status = it->second;
            ASSERT(status);
            return status;
        }
    }

private:
    mutable StateMap stateMap_;
    RecentStatuses   recentStatuses_;
    const Count      capacity_;
};

}}
