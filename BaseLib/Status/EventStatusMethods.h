#pragma once

#include"BaseLib/CommonDefines.h"
#include"BaseLib/Timestamp.h"
#include"BaseLib/Duration.h"
#include"BaseLib/Collection/IMap.h"
#include"BaseLib/Templates/Synchronization.h"

#include"BaseLib/Status/Rate.h"
#include"BaseLib/Status/StatusTracker.h"

namespace BaseLib { namespace Status {

template <typename StateType, typename T>
class StateStatusType : public StatusTracker<T>
{
public:
    StateStatusType(StateType stateType)
        : state_(stateType)
    { }

    virtual ~StateStatusType()
    { }

    StateType State() const
    {
        return state_;
    }

private:
    StateType state_;
};

/**
 * templated implementation of status of one state.
 *
 * TODO: See phd python scripts for examples of statuses.
 *
 * StateType is typically an enumerated value that deifines a state that an entity might be in, ex: EXECUTING
 */
template <typename StateType>
class StateStatus : public StatusTracker<int>
{
public:
    StateStatus(StateType stateType)
        : state_(stateType)
    { }

    virtual ~StateStatus()
    { }

    StateType State() const
    {
        return state_;
    }

private:
    StateType state_;
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
 *
 * http://metrics.dropwizard.io/3.1.0/manual/core/#sliding-window-reservoirs
 */
template <typename StateType, typename T>
class StateStatusTimestampedType : public StatusTrackerTimestamped<T>
{
public:
    StateStatusTimestampedType(StateType stateType, Duration windowDuration)
        : StatusTrackerTimestamped<T>(windowDuration)
        , state_(stateType)
    { }

    virtual ~StateStatusTimestampedType()
    { }

    StateType State() const
    {
        return state_;
    }

private:
    StateType state_;
};

template <typename StateType>
class StateStatusTimestamped : public StatusTrackerTimestamped<int>
{
public:
    StateStatusTimestamped(StateType stateType, Duration windowDuration)
        : StatusTrackerTimestamped<int>(windowDuration)
        , state_(stateType)
    { }

    virtual ~StateStatusTimestamped()
    { }

    StateType State() const
    {
        return state_;
    }

private:
    StateType state_;
};

}}
