#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Collection/IMap.h"

#include"BaseLib/Status/Rate.h"
#include"BaseLib/Templates/Synchronization.h"

namespace BaseLib { namespace Status {

template <typename T>
class StatusTracker : public Templates::LockableType<StatusTracker<T>>
{
    typedef MutexTypeLocker<StatusTracker<T>> Locker;

public:
    StatusTracker() : first_(true)
    { }

    virtual ~StatusTracker()
    { }

    // ------------------------------------------------------
    // Update counters and timestamps
    // ------------------------------------------------------

    virtual void Next(T num)
    {
        Locker locker(this);

        if(isFirst())
        {
            first(num);
        }
        else
        {
            next(num);
        }
    }

    virtual void Reset(T num)
    {
        Locker locker(this);
        reset();
        first(num);
    }

    virtual void Reset()
    {
        Locker locker(this);
        reset();
    }

    // ------------------------------------------------------
    // Count accesses
    // ------------------------------------------------------

    CountType<T> TotalSum() const
    {
        Locker locker(this);
        return totalSum_;
    }

    CountType<T> TotalInstances() const
    {
        Locker locker(this);
        return totalInstances_;
    }

    bool IsInstances() const
    {
        Locker locker(this);
        return !totalInstances_.IsZero();
    }

    // ------------------------------------------------------
    // Access absolute timestamps
    // ------------------------------------------------------

    Timestamp FirstAccess() const
    {
        Locker locker(this);
        return firstAccess_;
    }

    Timestamp MostRecentAccess() const
    {
        Locker locker(this);
        return mostRecentAccess_;
    }

    // ------------------------------------------------------
    // Compute time since a point in time
    // ------------------------------------------------------

    Duration TimeSinceFirstAccess() const
    {
        Locker locker(this);
        return firstAccess_.SinceStamped();
    }

    Duration TimeSinceMostRecentAccess() const
    {
        Locker locker(this);
        return mostRecentAccess_.SinceStamped();
    }

    TimeRate AsRate() const
    {
        Locker locker(this);
        return TimeRate(totalSum_, mostRecentAccess_.TimeSinceEpoch());
    }

    bool IsStatus() const
    {
        Locker locker(this);
        return !first_;
    }

    // --------------------------------------
    // Operators
    // --------------------------------------

    friend std::ostream& operator<<(std::ostream& ostr, const StatusTracker<T>& status)
    {
        ostr << "["
        << status.TotalSum()
        << ", "
        << status.TimeSinceMostRecentAccess()
        << ", "
        << status.AsRate()
        << "]";

        return ostr;
    }

private:
    void first(T num)
    {
        ++totalInstances_;

        totalSum_ += num;
        firstAccess_ = Timestamp::Now();
        first_       = false;
    }

    void next(T num)
    {
        ++totalInstances_;

        totalSum_ += num;
        mostRecentAccess_ = Timestamp::Now();
    }

    void reset()
    {
        totalInstances_   = CountType<T>();
        totalSum_         = CountType<T>();
        firstAccess_      = Timestamp::Now();
        mostRecentAccess_ = Timestamp::Now();
        first_            = true;
    }

    bool isFirst() const
    {
        return first_;
    }

private:
    bool         first_;
    CountType<T> totalInstances_;
    CountType<T> totalSum_;

    Timestamp firstAccess_;
    Timestamp mostRecentAccess_;
};

/**
 * templated implementation of status of one state.
 *
 * TODO: See phd python scripts for examples of statuses.
 *
 * StateType is typically an enumerated value that defines a state that an entity might be in, ex: EXECUTING
 *
 * TODO: https://github.com/Netflix/Hystrix/wiki/How-it-Works#CircuitBreaker
 *  - Add int numWindows (aka. buckets)
 *
 * https://github.com/Netflix/Hystrix/blob/master/hystrix-core/src/main/java/com/netflix/hystrix/util/HystrixRollingNumber.java
 * https://github.com/Netflix/Hystrix/blob/master/hystrix-core/src/main/java/com/netflix/hystrix/util/HystrixRollingNumberEvent.java
 */
template <typename T>
class StatusTrackerTimestamped : public StatusTracker<T>
{
    typedef Collection::IMap <Timestamp, CountType<T>>    TimestampedCount;
    typedef MutexTypeLocker <StatusTrackerTimestamped<T>> Locker;

public:
    StatusTrackerTimestamped(Duration windowDuration = Duration::FromMinutes(10))
        : windowDuration_(windowDuration)
    { }

    virtual ~StatusTrackerTimestamped()
    { }

    CountType<T> SumInWindow() const
    {
        Locker locker(this);
        return getSumInWindow(windowDuration_);
    }

    CountType<T> SumInWindow(Duration duration) const
    {
        Locker locker(this);
        return getSumInWindow(duration);
    }

    CountType<T> NumOfInstances() const
    {
        Locker locker(this);
        return getNumberOfInstancesInWindow(windowDuration_);
    }

    CountType<T> NumOfInstancesIn(Duration duration) const
    {
        Locker locker(this);
        return getNumberOfInstancesInWindow(duration);
    }

    Collection::IMap<Timestamp, T> Instances() const
    {
        Locker locker(this);
        return getIstancesIn(windowDuration_);
    }

    Collection::IMap<Timestamp, T> InstancesIn(Duration duration) const
    {
        Locker locker(this);
        return getIstancesIn(duration);
    }

    Duration WindowSpan() const
    {
        Locker locker(this);
        return windowDuration_;
    }

    Status::TimeRate SumAsTimeRate() const
    {
        Locker locker(this);
        return Status::TimeRate(getSumInWindow(windowDuration_), windowDuration_);
    }

    Status::TimeRate SumAsTimeRate(Duration duration) const
    {
        Locker locker(this);
        return Status::TimeRate(getSumInWindow(duration), duration);
    }

    Status::TimeRate InstancesAsTimeRate() const
    {
        Locker locker(this);
        return Status::TimeRate(getNumberOfInstancesInWindow(windowDuration_), windowDuration_);
    }

    Status::TimeRate InstancesAsTimeRate(Duration duration) const
    {
        Locker locker(this);
        return Status::TimeRate(getNumberOfInstancesInWindow(duration), duration);
    }

    // ------------------------------------------------------
    // Update counters and timestamps
    // ------------------------------------------------------

    virtual void Next(T num)
    {
        StatusTracker<T>::Next(num);

        Locker locker(this);
        updateWindow(windowDuration_, num);
    }

    virtual void Reset(T num)
    {
        StatusTracker<T>::Reset(num);

        Locker locker(this);
        resetWindow();
        updateWindow(windowDuration_, num);
    }

    virtual void Reset()
    {
        StatusTracker<T>::Reset();

        Locker locker(this);
        resetWindow();
    }

private:

    // ------------------------------------------------------
    // update count window
    // ------------------------------------------------------

    CountType<T> getSumInWindow(const Duration& duration) const
    {
        CountType<T> sum;

        for(auto it = timestampedCount_.rbegin(), it_end = timestampedCount_.rend(); it != it_end; ++it)
        {
            const Timestamp& timestamp = it->first;

            if(timestamp.SinceStamped() < duration)
            {
                sum += it->second;
            }
            else
            {
                break;
            }
        }

        return sum;
    }

    CountType<T> getNumberOfInstancesInWindow(const Duration& duration) const
    {
        CountType<T> counter;

        for(auto it = timestampedCount_.rbegin(), it_end = timestampedCount_.rend(); it != it_end; ++it)
        {
            const Timestamp& timestamp = it->first;

            if(timestamp.SinceStamped() < duration)
            {
                ++counter;
            }
            else
            {
                break;
            }
        }

        return counter;
    }

    Collection::IMap<Timestamp, T> getIstancesIn(const Duration& duration) const
    {
        Collection::IMap<Timestamp, T> counter;

        for(auto it = timestampedCount_.rbegin(), it_end = timestampedCount_.rend(); it != it_end; ++it)
        {
            const Timestamp& timestamp = it->first;
            const CountType<T> countType = it->second;

            if(timestamp.SinceStamped() < duration)
            {
                counter.put(it->first, countType.delegate());
            }
            else
            {
                break;
            }
        }

        return counter;
    }

    // ------------------------------------------------------
    // update count window
    // ------------------------------------------------------

    void updateWindow(const Duration& windowDuration, T num)
    {
        timestampedCount_.put(Timestamp::Now(), num);

        pruneWindow(windowDuration);
    }

    void pruneWindow(const Duration& windowDuration)
    {
        // Remove outdated entries
        typename TimestampedCount::iterator it = timestampedCount_.begin();
        while(it != timestampedCount_.end())
        {
            const Timestamp& timestamp = it->first;
            if(timestamp.SinceStamped() >= windowDuration)
            {
                typename TimestampedCount::iterator eraseIt = it;
                ++it;
                timestampedCount_.erase(eraseIt);
            }
            else
            {
                break;
            }
        }
    }

    void resetWindow()
    {
        timestampedCount_.clear();
    }

protected:
    mutable TimestampedCount timestampedCount_;
    const Duration           windowDuration_;
};

}}
